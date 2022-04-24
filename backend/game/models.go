package game

import (
	"errors"

	"github.com/MIXISAMA/gobang/backend/server"
)

type GlobalData struct {
	Rooms     []*Room
	RoomWhose map[*server.User]*Room
	UserWho   map[string]*server.User
}

func (gd *GlobalData) AddUserRoom(user *server.User, room *Room) error {

	if _, ok := gd.RoomWhose[user]; ok {
		return errors.New("this user is exist")
	}

	if _, ok := gd.UserWho[user.Name]; ok {
		return errors.New("same name user is exist")
	}

	gd.RoomWhose[user] = room
	gd.UserWho[user.Name] = user

	return nil
}

func (gd *GlobalData) RemoveUser(user *server.User) {

	delete(gd.RoomWhose, user)
	delete(gd.UserWho, user.Name)

}
