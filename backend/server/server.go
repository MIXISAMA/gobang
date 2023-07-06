package server

import (
	"github.com/MIXISAMA/gobang/backend/game"
	"github.com/MIXISAMA/gobang/backend/middlewares/room"
	"github.com/MIXISAMA/gobang/backend/middlewares/user"
)

const Version = "0.1"

type Server struct {
	game game.Game
	user *user.Middleware
	room *room.Middleware
}

func NewServer(
	name string,
	user *user.Middleware,
	room *room.Middleware,
) *Server {
	server := new(Server)
	server.game.MakeRoom(room.Size())
	server.user = user
	server.room = room
	return server
}
