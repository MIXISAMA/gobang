package server

import (
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwroom"
)

const Version = "0.1"
const MaxServerNameLen = 200

type Server struct {
	name string
	room *mdwroom.Middleware
}

func NewServer(name string, room *mdwroom.Middleware) *Server {
	return &Server{
		name: name[:MaxServerNameLen],
		room: room,
	}
}
