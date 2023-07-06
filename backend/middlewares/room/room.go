package room

import (
	"container/list"

	"github.com/MIXISAMA/gobang/backend/idtcp"
)

var MaxRooms = 200

type Room struct {
	Name     string
	MaxUsers int
}

type Middleware struct {
	rooms []Room
	users *list.List
}

func NewMiddleware(rooms []Room) *Middleware {
	numOfRooms := len(rooms)
	if numOfRooms > MaxRooms {
		numOfRooms = MaxRooms
	}
	middleware := new(Middleware)
	middleware.users = list.New()
	middleware.rooms = make([]Room, numOfRooms)
	for i := range middleware.rooms {
		middleware.rooms[i] = rooms[i]
	}

	return middleware
}

var Key = new(idtcp.MiddlewareKey)

type DistributePayload struct {
	Room *Room
}

func (middleware *Middleware) ProcessConnection(
	payloads idtcp.PayloadMap,
	processConnection func(idtcp.PayloadMap) (*idtcp.Conn, error),
) (*idtcp.Conn, error) {
	return processConnection(payloads)
}

func (middleware *Middleware) ProcessDistribution(
	request *idtcp.Request,
	processDistribution func(*idtcp.Request) error,
) error {
	request.DistPayloads[Key] = &DistributePayload{}
	return processDistribution(request)
}

func (middleware *Middleware) Size() int {
	return len(middleware.rooms)
}
