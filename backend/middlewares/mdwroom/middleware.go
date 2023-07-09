package mdwroom

import "github.com/MIXISAMA/gobang/backend/idtcp"

var MaxRooms = 200

type Middleware struct {
	Rooms []*Room
}

func NewMiddleware(rooms []*Room) *Middleware {
	middleware := Middleware{
		Rooms: make([]*Room, len(rooms[:MaxRooms])),
	}
	for i := range middleware.Rooms {
		room := *rooms[i]
		room.Index = i
		middleware.Rooms[i] = &room
	}
	return &middleware
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
