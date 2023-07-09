package mdwroom

import (
	"container/list"

	"github.com/MIXISAMA/gobang/backend/game"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
)

type Room struct {
	game.Room
	Index    int
	Name     string
	MaxUsers int
	Users    *list.List
}

func NewRoom(name string, maxUser int) *Room {
	return &Room{
		Room:     *game.NewRoom(),
		Name:     name,
		MaxUsers: maxUser,
		Users:    list.New(),
	}
}

func (room *Room) FindUser(username string) *mdwuser.User {
	for i := room.Users.Front(); i != room.Users.Back(); i = i.Next() {
		user := i.Value.(*mdwuser.User)
		if user.Username == username {
			return user
		}
	}
	return nil
}
