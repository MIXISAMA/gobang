package mdwuser

import (
	"crypto/rand"
	"crypto/rsa"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwfatal"
	"github.com/google/uuid"
)

type Middleware struct {
	s_JoinRoom    uint16
	c_PublicKey   uint16
	c_YouJoinRoom uint16
	database      *Database
	serverUuid    []byte
}

func NewMiddleware(
	s_JoinRoom uint16,
	c_PublicKey uint16,
	c_YouJoinRoom uint16,
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
		s_JoinRoom:    s_JoinRoom,
		c_PublicKey:   c_PublicKey,
		c_YouJoinRoom: c_YouJoinRoom,
		database:      database,
		serverUuid:    uuidBytes,
	}, nil
}

var Key = new(idtcp.MiddlewareKey)

type Payload struct {
	User       *User
	PrivateKey *rsa.PrivateKey
}

func (middleware *Middleware) ProcessConnect(
	payloads idtcp.PayloadMap,
	processConnect func(idtcp.PayloadMap) (*idtcp.Conn, error),
) (*idtcp.Conn, error) {

	privateKey, err := rsa.GenerateKey(rand.Reader, 1024)
	if err != nil {
		return nil, err
	}

	payloads[&Key] = &Payload{
		User:       nil,
		PrivateKey: privateKey,
	}

	conn, err := processConnect(payloads)
	if err != nil {
		return conn, err
	}

	err = middleware.sendPublicKey(conn, &privateKey.PublicKey)
	return conn, err
}

func (middleware *Middleware) ProcessDisconnect(
	conn *idtcp.Conn,
	payloads idtcp.PayloadMap,
	processDisconnect func(*idtcp.Conn, idtcp.PayloadMap),
) {
	processDisconnect(conn, payloads)
}

func (middleware *Middleware) ProcessDistribute(
	request *idtcp.Request,
	processDistribute func(*idtcp.Request) error,
) error {

	if request.Instruction == middleware.s_JoinRoom {
		user, err := middleware.authorization(request)
		if err != nil {
			middleware.sendAuthorizationFailed(request.Conn, middleware.c_YouJoinRoom)
			return err
		}
		request.Payloads[&Key].(*Payload).User = user
	}

	user := request.Payloads[&Key].(*Payload).User
	if user == nil {
		return mdwfatal.NewExecution(request.Conn, 0, "user has not passed the authorization")
	}

	err := processDistribute(request)
	if err != nil {
		return err
	}

	return nil
}
