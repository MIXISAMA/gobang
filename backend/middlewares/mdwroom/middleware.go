package mdwroom

import (
	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
)

var MaxRooms = 200

type Middleware struct {
	s_JoinRoom      uint16
	s_LeaveRoom     uint16
	c_YouJoinRoom   uint16
	c_OtherJoinRoom uint16
	c_UserLeaveRoom uint16
	c_GameOver      uint16
	Rooms           []*Room
}

type ConfigRoom struct {
	Name     string
	MaxUsers int
}

type InstructionClientYouJoinRoom struct {
	IsSuccess     bool
	RoomName      string `len_bytes:"1"`
	MaxUsers      uint8
	BlackUsername string   `len_bytes:"1"`
	WhileUsername string   `len_bytes:"1"`
	Onlookers     []string `len_bytes:"1" array_len_bytes:"1"`
	ReadyPlayer   byte
	IsPlaying     bool
	RegretPlayer  byte
	TiePlayer     byte
	Records       []byte `len_bytes:"1"`
}

func NewMiddleware(
	s_JoinRoom uint16,
	s_LeaveRoom uint16,
	c_YouJoinRoom uint16,
	c_OtherJoinRoom uint16,
	c_UserLeaveRoom uint16,
	c_GameOver uint16,
	rooms []*ConfigRoom,
) *Middleware {
	maxRooms := MaxRooms
	if len(rooms) < maxRooms {
		maxRooms = len(rooms)
	}
	middleware := Middleware{
		s_JoinRoom:      s_JoinRoom,
		s_LeaveRoom:     s_LeaveRoom,
		c_YouJoinRoom:   c_YouJoinRoom,
		c_OtherJoinRoom: c_OtherJoinRoom,
		c_UserLeaveRoom: c_UserLeaveRoom,
		c_GameOver:      c_GameOver,
		Rooms:           make([]*Room, maxRooms),
	}
	for i := range middleware.Rooms {
		middleware.Rooms[i] = NewRoom(rooms[i].Name, rooms[i].MaxUsers)
	}
	return &middleware
}

var Key int

type Payload struct {
	Room *Room
}

func (middleware *Middleware) ProcessConnect(ctx *idtcp.ConnectContext) (*idtcp.Conn, error) {
	return ctx.Next()
}

func (middleware *Middleware) ProcessDisconnect(ctx *idtcp.DisconnectContext) {
	// TODO: delete user from room when disconnected
	room := ctx.Payloads[Key].(*Payload).Room
	user := ctx.Payloads[mdwuser.Key].(*mdwuser.Payload).User
	room.leave(user)
	ctx.Next()
}

func (middleware *Middleware) ProcessDistribute(ctx *idtcp.DistributeContext) error {

	if ctx.Instruction == middleware.s_JoinRoom {
		err := middleware.receiveJoinRoom(ctx.Conn, ctx.Data, ctx.Payloads)
		if err != nil {
			return err
		}
	}

	_ = ctx.Next()
	// TODO

	if ctx.Instruction == middleware.s_LeaveRoom {
		err := middleware.receiveUserLeaveRoom(ctx.Payloads)
		if err != nil {
			return err
		}
	}

	return nil
}

func (middleware *Middleware) SetMdwKey(val int) {
	Key = val
}

func (middleware *Middleware) NewPayload() interface{} {
	return &Payload{Room: nil}
}
