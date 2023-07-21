package server

import (
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwroom"
)

const Version = "0.1"
const MaxServerNameLen = 64

type Server struct {
	name string
	room *mdwroom.Middleware
}

func NewServer(name string, room *mdwroom.Middleware) *Server {
	maxServerNameLen := MaxServerNameLen
	if len(name) < maxServerNameLen {
		maxServerNameLen = len(name)
	}
	return &Server{
		name: name[:maxServerNameLen],
		room: room,
	}
}
