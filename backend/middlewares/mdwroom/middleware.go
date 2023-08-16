package mdwroom

import (
	"github.com/MIXISAMA/gobang/backend/idtcp"
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
	// delete user from room when disconnected
	middleware.processUserLeave(ctx.Payloads)
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

	if ctx.Instruction == middleware.s_LeaveRoom {
		middleware.processUserLeave(ctx.Payloads)
	}

	return nil
}

func (middleware *Middleware) SetMdwKey(val int) {
	Key = val
}

func (middleware *Middleware) NewPayload() interface{} {
	return &Payload{Room: nil}
}
