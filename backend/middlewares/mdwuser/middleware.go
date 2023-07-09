package mdwuser

import (
	"crypto/rand"
	"crypto/rsa"
	"errors"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/google/uuid"
)

type Middleware struct {
	instructionPublicKey     uint16
	instructionAuthorization uint16
	instructionAuthFailed    uint16
	database                 *Database
	serverUuid               []byte
}

func NewMiddleware(
	instructionPublicKey uint16,
	instructionAuthorization uint16,
	instructionAuthFailed uint16,
	databasePath string,
	serverUuid string,
) (*Middleware, error) {

	u, err := uuid.Parse(serverUuid)
	if err != nil {
		return nil, err
	}

	uuidBytes, err := u.MarshalBinary()
	if err != nil {
		return nil, err
	}

	database, err := NewDatabase(databasePath)
	if err != nil {
		return nil, err
	}

	return &Middleware{
		instructionPublicKey:     instructionPublicKey,
		instructionAuthorization: instructionAuthorization,
		instructionAuthFailed:    instructionAuthFailed,
		database:                 database,
		serverUuid:               uuidBytes,
	}, nil
}

var Key = new(idtcp.MiddlewareKey)

type ConnectionPayload struct {
	User       *User
	PrivateKey *rsa.PrivateKey
}

type DistributePayload struct {
}

func (middleware *Middleware) ProcessConnection(
	payloads idtcp.PayloadMap,
	processConnection func(idtcp.PayloadMap) (*idtcp.Conn, error),
) (*idtcp.Conn, error) {

	privateKey, err := rsa.GenerateKey(rand.Reader, 1024)
	if err != nil {
		return nil, err
	}

	payloads[Key] = &ConnectionPayload{
		User:       nil,
		PrivateKey: privateKey,
	}

	conn, err := processConnection(payloads)
	if err != nil {
		return conn, err
	}

	err = middleware.sendPublicKey(conn, &privateKey.PublicKey)
	return conn, err
}

func (middleware *Middleware) ProcessDistribution(
	request *idtcp.Request,
	processDistribution func(*idtcp.Request) error,
) error {

	request.DistPayloads[Key] = &ConnectionPayload{}

	if request.Instruction == middleware.instructionAuthorization {
		user, err := middleware.authorization(request)
		if err != nil {
			middleware.sendAuthorizationFailed(request.Conn, middleware.instructionAuthFailed)
			return err
		}
		request.ConnPayloads[Key].(*ConnectionPayload).User = user
	}

	user := request.ConnPayloads[Key].(*ConnectionPayload).User
	if user == nil {
		return errors.New("user has not passed the authorization")
	}

	err := processDistribution(request)
	if err != nil {
		return err
	}

	return nil
}
