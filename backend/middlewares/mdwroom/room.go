package mdwroom

import (
	"container/list"
	"errors"
	"log"

	"github.com/MIXISAMA/gobang/backend/game"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
)

const MaxUsers = 200

type Room struct {
	game.Room
	Name     string
	MaxUsers int
	Users    *list.List
}

func NewRoom(name string, maxUser int) *Room {
	if maxUser > MaxUsers {
		maxUser = MaxUsers
	}
	if maxUser < 2 {
		maxUser = 2
	}
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

func (room *Room) joinAsPlayer(user *mdwuser.User) error {
	err := room.Room.Join(user)
	if err != nil {
		return err
	}
	room.Users.PushBack(user)
	return nil
}

func (room *Room) joinAsOnlooker(user *mdwuser.User) error {
	if room.MaxUsers-2 <= room.Users.Len()-room.PlayerCount() {
		return errors.New("the room is full of onlookers")
	}
	room.Users.PushBack(user)
	return nil
}

func (room *Room) leave(user *mdwuser.User) error {
	log.Printf("user %v is leaving", user.Username)
	err := room.Leave(user)
	switch err {
	case game.ErrNotPlayer: // do nothing
	default:
		return err
	}

	for i := room.Users.Front(); i != room.Users.Back(); i = i.Next() {
		if i.Value == user {
			room.Users.Remove(i)
			break
		}
	}
	return nil
}
