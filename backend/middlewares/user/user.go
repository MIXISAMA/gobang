package user

import (
	"crypto/rand"
	"crypto/rsa"
	"crypto/x509"
	"database/sql"
	"encoding/pem"
	"errors"

	_ "github.com/mattn/go-sqlite3"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/utils"
	"github.com/google/uuid"
)

type User struct {
	Username     string
	NumOfWins    uint32
	NumOfTies    uint32
	NumOfLosses  uint32
	NumOfMatches uint32
	GameDuration uint64
}

type Middleware struct {
	instructionPublicKey     uint16
	instructionAuthorization uint16
	databasePath             string
	serverUuid               []byte
}

func NewMiddleware(
	instructionPublicKey uint16,
	instructionAuthorization uint16,
	databasePath string,
	serverUuid string,
) (*Middleware, error) {

	u, err := uuid.Parse(serverUuid)
	if err != nil {
		return nil, err
	}

	ub, err := u.MarshalBinary()
	if err != nil {
		return nil, err
	}

	db, err := sql.Open("sqlite3", databasePath)
	if err != nil {
		return nil, err
	}
	defer db.Close()

	_, err = db.Exec("PRAGMA locking_mode = EXCLUSIVE;")
	if err != nil {
		return nil, err
	}

	createTableSQL := `
		CREATE TABLE IF NOT EXISTS user (
			username TEXT PRIMARY KEY,
			password BLOB,
			wins     INTEGER DEFAULT 0,
			ties     INTEGER DEFAULT 0,
			losses   INTEGER DEFAULT 0,
			matches  INTEGER DEFAULT 0,
			duration INTEGER DEFAULT 0
		);
	`
	_, err = db.Exec(createTableSQL)
	if err != nil {
		return nil, err
	}

	return &Middleware{
		instructionPublicKey:     instructionPublicKey,
		instructionAuthorization: instructionAuthorization,
		databasePath:             databasePath,
		serverUuid:               ub,
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
		err := middleware.authorization(request)
		if err != nil {
			return err
		}
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

	_, err = conn.Write(middleware.instructionPublicKey, s.Raw)
	return err
}

func (middleware *Middleware) authorization(request *idtcp.Request) error {

	var s1 = utils.MakeSerializer(request.Data)

	_, err := s1.ReadUint16()
	if err != nil {
		return err
	}

	ciphertext, err := s1.ReadBytes8()
	if err != nil {
		return err
	}

	privateKey := request.ConnPayloads[Key].(*ConnectionPayload).PrivateKey
	plaintext, err := rsa.DecryptPKCS1v15(rand.Reader, privateKey, ciphertext)
	if err != nil {
		return err
	}

	var s2 = utils.MakeSerializer(plaintext)

	username, err := s2.ReadString8()
	if err != nil {
		return err
	}

	password, err := s2.ReadBytes8()
	if err != nil {
		return err
	}

	user, err := middleware.addUserToDatabase(username, password)
	if err != nil {
		return err
	}

	request.ConnPayloads[Key].(*ConnectionPayload).User = user
	return nil
}

func (middleware *Middleware) addUserToDatabase(username string, password []byte) (*User, error) {
	db, err := sql.Open("sqlite3", middleware.databasePath)
	if err != nil {
		return nil, err
	}
	defer db.Close()

	stmt1, err := db.Prepare(`
		SELECT wins, ties, losses, matches, duration
		FROM user
		WHERE username = ? AND password = ?`)
	if err != nil {
		return nil, err
	}
	defer stmt1.Close()

	var user = User{
		Username:     username,
		NumOfWins:    0,
		NumOfTies:    0,
		NumOfLosses:  0,
		NumOfMatches: 0,
		GameDuration: 0,
	}

	row := stmt1.QueryRow(username, password)
	err = row.Scan(
		&user.NumOfWins,
		&user.NumOfTies,
		&user.NumOfLosses,
		&user.NumOfMatches,
		&user.GameDuration,
	)
	if err != nil {
		stmt2, err := db.Prepare(`
			INSERT INTO user (username, password)
			VALUES (?, ?)`)
		if err != nil {
			return nil, err
		}
		defer stmt2.Close()
		_, err = stmt2.Exec(username, password)
		if err != nil {
			return nil, err
		}
	}

	return &user, nil
}
