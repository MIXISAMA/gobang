package mdwuser

import (
	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwfatal"
	"github.com/MIXISAMA/gobang/backend/utils"
	"github.com/google/uuid"
)

type Middleware struct {
	s_JoinRoom    uint16
	c_YouJoinRoom uint16
	database      *Database
	serverUuid    []byte
}

func NewMiddleware(
	s_JoinRoom uint16,
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
		c_YouJoinRoom: c_YouJoinRoom,
		database:      database,
		serverUuid:    uuidBytes,
	}, nil
}

var Key int

type Payload struct {
	User *User
}

type InstructionJoinRoom struct {
	RoomID   uint8
	Username string `len_bytes:"1"`
	Password []byte `len_bytes:"2"`
	Role     utils.Char
}

func (middleware *Middleware) ProcessConnect(ctx *idtcp.ConnectContext) (*idtcp.Conn, error) {
	ctx.Payloads[Key] = &Payload{User: nil}
	return ctx.Next()
}

func (middleware *Middleware) ProcessDisconnect(ctx *idtcp.DisconnectContext) {
	ctx.Next()
}

func (middleware *Middleware) ProcessDistribute(ctx *idtcp.DistributeContext) error {

	if ctx.Instruction == middleware.s_JoinRoom {
		user, err := middleware.authenticate(ctx.Conn, ctx.Data)
		if err != nil {
			middleware.sendAuthenticationFailed(ctx.Conn, middleware.c_YouJoinRoom)
			return err
		}
		ctx.Payloads[Key].(*Payload).User = user
	}

	user := ctx.Payloads[Key].(*Payload).User
	if user == nil {
		return mdwfatal.NewExecution(ctx.Conn, 0, "user has not passed the authentication")
	}

	return ctx.Next()
}

func (middleware *Middleware) SetMdwKey(val int) {
	Key = val
}

func (middleware *Middleware) NewPayload() interface{} {
	return &Payload{User: nil}
}
