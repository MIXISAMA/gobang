package mdwuser

import (
	"crypto/rand"
	"crypto/rsa"
	"crypto/x509"
	"encoding/pem"
	"errors"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/utils"
)

func (middleware *Middleware) sendPublicKey(conn *idtcp.Conn, publicKey *rsa.PublicKey) error {

	pubKeyBytes, err := x509.MarshalPKIXPublicKey(publicKey)
	if err != nil {
		return err
	}

	pemData := pem.EncodeToMemory(&pem.Block{
		Type:  "PUBLIC KEY",
		Bytes: pubKeyBytes,
	})

	var s = utils.MakeSerializer(pemData)
	s.WriteBytes8(pemData)
	s.WriteBytes8(middleware.serverUuid)

	_, err = conn.Write(middleware.c_PublicKey, s.Raw)
	return err
}

func (middleware *Middleware) sendAuthorizationFailed(conn *idtcp.Conn, instruction uint16) error {
	var s utils.Serializer
	s.WriteBoolean(false)
	_, err := conn.Write(instruction, s.Raw)
	return err
}

func (middleware *Middleware) authorization(request *idtcp.Request) (*User, error) {

	var s1 = utils.MakeSerializer(request.Data)

	_, err := s1.ReadUint16()
	if err != nil {
		return nil, err
	}

	ciphertext, err := s1.ReadBytes8()
	if err != nil {
		return nil, err
	}

	privateKey := request.Payloads[Key].(*Payload).PrivateKey
	plaintext, err := rsa.DecryptPKCS1v15(rand.Reader, privateKey, ciphertext)
	if err != nil {
		return nil, err
	}

	var s2 = utils.MakeSerializer(plaintext)

	username, err := s2.ReadString8()
	if err != nil {
		return nil, err
	}

	password, err := s2.ReadBytes8()
	if err != nil {
		return nil, err
	}

	err = middleware.database.authorization(username, password)
	if errors.Is(err, ErrUserNotFound) {
		err = middleware.database.register(username, password)
		if err != nil {
			return nil, err
		}
	} else if err != nil {
		return nil, err
	}

	return middleware.database.getUser(request.Conn, username)
}
