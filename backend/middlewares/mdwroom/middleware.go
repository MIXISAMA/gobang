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

var Key = new(idtcp.MiddlewareKey)

type Payload struct {
	Room *Room
}

func (middleware *Middleware) ProcessConnect(
	payloads idtcp.PayloadMap,
	processConnect func(idtcp.PayloadMap) (*idtcp.Conn, error),
) (*idtcp.Conn, error) {
	payloads[&Key] = &Payload{Room: nil}
	return processConnect(payloads)
}

func (middleware *Middleware) ProcessDisconnect(
	conn *idtcp.Conn,
	payloads idtcp.PayloadMap,
	processDisconnect func(*idtcp.Conn, idtcp.PayloadMap),
) {
	// TODO: delete user from room when disconnected
	room := payloads[&Key].(*Payload).Room
	user := payloads[&mdwuser.Key].(*mdwuser.Payload).User
	room.leave(user)
	processDisconnect(conn, payloads)
}

func (middleware *Middleware) ProcessDistribute(
	request *idtcp.Request,
	processDistribute func(*idtcp.Request) error,
) error {

	if request.Instruction == middleware.s_JoinRoom {
		err := middleware.receiveJoinRoom(request)
		if err != nil {
			return err
		}
	}

	_ = processDistribute(request)
	// TODO

	if request.Instruction == middleware.s_LeaveRoom {
		err := middleware.receiveUserLeaveRoom(request)
		if err != nil {
			return err
		}
	}

	return nil
}
