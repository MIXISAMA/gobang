package game

import (
	"errors"

	"github.com/MIXISAMA/gobang/backend/server"
)

type Room struct {
	Name      string
	Users     []*server.User
	MaxUsers  int
	Players   [2]*server.User
	Repentant *server.User
	Ready     *server.User
	IsPlaying bool
	Chess     Chess
}

func NewRoom(name string, maxUsers int) *Room {

	return &Room{
		Name:      name,
		Users:     make([]*server.User, 0),
		MaxUsers:  maxUsers,
		IsPlaying: false,
		Chess:     *NewChess(),
	}

}

func (room *Room) Clean() {

	room.IsPlaying = false
	room.Repentant = nil
	room.Ready = nil
	room.Chess = *NewChess()
	room.Players[0], room.Players[1] = room.Players[1], room.Players[0]

}

func (room *Room) LocateUser(user *server.User) (int, error) {

	for i := range room.Users {
		if user == room.Users[i] {
			return i, nil
		}
	}
	return -1, errors.New("user not found")

}

func (room *Room) LocatePlayer(player *server.User) (int, error) {

	for i := range room.Players {
		if player == room.Players[i] {
			return i, nil
		}
	}
	return -1, errors.New("player not found")

}

func (room *Room) UserJoin(user *server.User, isPlayer bool) error {

	if _, err := room.LocateUser(user); err == nil {
		return errors.New("this user is exist")
	}

	if isPlayer {

		if room.IsPlaying {
			return errors.New("this room is playing")
		}

		p1 := room.Players[BLACK]
		p2 := room.Players[WHITE]

		if p1 != nil && p2 != nil {
			return errors.New("this room is full")
		}

		if p1 == nil {
			room.Players[BLACK] = user
		} else if p2 == nil {
			room.Players[WHITE] = user
		}

	}

	room.Users = append(room.Users, user)
	return nil

}

func (room *Room) UserLeave(user *server.User) error {

	userIdx, err := room.LocateUser(user)
	if err != nil {
		return errors.New("this room has not this user")
	}

	room.Users = append(room.Users[:userIdx], room.Users[userIdx+1:]...)

	playerIdx, err := room.LocatePlayer(user)
	if err != nil {
		// yes! it return nil
		return nil
	}

	room.Players[playerIdx] = nil
	room.Clean()
	return nil

}

func (room *Room) Stone(player *server.User, coor byte) error {

	if !room.IsPlaying {
		return errors.New("this room is not playing")
	}

	idx, err := room.LocatePlayer(player)
	if err != nil {
		return errors.New("this user is not a player")
	}

	color := byte(idx)
	return room.Chess.Stone(coor, color)

}
