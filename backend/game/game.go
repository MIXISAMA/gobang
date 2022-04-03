package game

import (
	"errors"
)

type User interface {
}

type Onlooker struct {
	Name string
}

type Player struct {
	Name  string
	Ready bool
}

type Room struct {
	Name          string
	Players       []*Player
	Onlookers     []*Onlooker
	BlackPlayer   *Player
	IsPlaying     bool
	ChessBoard    [15][15]byte
	Record        []byte
	MaxOnlookers  int
	RegretRequest bool
}

func (room *Room) initChessBoard() {
	for i, rows := range room.ChessBoard {
		for j := range rows {
			room.ChessBoard[i][j] = 0xFF
		}
	}
}

func (room *Room) Clean() {
	room.IsPlaying = false
	room.RegretRequest = false
	room.initChessBoard()
	room.Record = nil
}

func (room *Room) SwitchColor() {

	for i := range room.Players {
		if room.BlackPlayer != room.Players[i] {
			room.BlackPlayer = room.Players[i]
			return
		}
	}

	room.BlackPlayer = nil

}

func NewRoom(name string, maxOnlookers int) *Room {
	room := new(Room)
	room.Name = name
	room.MaxOnlookers = maxOnlookers
	room.Clean()
	return room
}

func (room *Room) FindOnlookerByName(name string) *Onlooker {
	for i := range room.Onlookers {
		onlooker := room.Onlookers[i]
		if onlooker.Name == name {
			return onlooker
		}
	}
	return nil
}

func (room *Room) FindPlayerByName(name string) *Player {
	for i := range room.Players {
		player := room.Players[i]
		if player.Name == name {
			return player
		}
	}
	return nil
}

func (room *Room) JoinPlayer(playerName string) (joinPlayer *Player, err error) {

	playerCount := len(room.Players)

	if playerCount >= 2 {
		return nil, errors.New("this room is full")
	}

	joinPlayer = room.FindPlayerByName(playerName)

	if joinPlayer != nil {
		return nil, errors.New("same name player has been exist in this room")
	}

	joinPlayer = new(Player)
	joinPlayer.Name = playerName
	joinPlayer.Ready = false

	if room.BlackPlayer == nil {
		room.BlackPlayer = joinPlayer
	}

	room.Players = append(room.Players, joinPlayer)

	return joinPlayer, nil
}

func (room *Room) LeavePlayer(name string) (leavePlayer *Player, err error) {

	for i := range room.Players {
		if room.Players[i].Name == name {
			leavePlayer = room.Players[i]
			room.Players = append(room.Players[:i], room.Players[i+1:]...)
			break
		}
	}

	if leavePlayer == nil {
		return nil, errors.New("this player is not exist in this room")
	}

	return leavePlayer, nil

}

func (room *Room) JoinOnlooker(onlookerName string) (joinOnlooker *Onlooker, err error) {

	if len(room.Onlookers) >= room.MaxOnlookers {
		return nil, errors.New("this room is full")
	}

	joinOnlooker = room.FindOnlookerByName(onlookerName)

	if joinOnlooker != nil {
		return nil, errors.New("same name player has been exist in this room")
	}

	joinOnlooker = new(Onlooker)
	joinOnlooker.Name = onlookerName
	room.Onlookers = append(room.Onlookers, joinOnlooker)

	return joinOnlooker, nil

}

func (room *Room) LeaveOnlooker(name string) (leaveOnlooker *Onlooker, err error) {

	for i := range room.Onlookers {
		if room.Onlookers[i].Name == name {
			leaveOnlooker = room.Onlookers[i]
			room.Onlookers = append(room.Onlookers[:i], room.Onlookers[i+1:]...)
			break
		}
	}

	if leaveOnlooker == nil {
		return nil, errors.New("this player is not exist in this room")
	}

	return leaveOnlooker, nil

}

func (room *Room) Stone(coor byte) error {

	if !room.IsPlaying {
		return errors.New("this room is not playing")
	}

	r := coor & 0x0F
	c := coor >> 4

	if r == 15 || c == 15 {
		return errors.New("out of chess board")
	}
	if room.ChessBoard[r][c] != 0xFF {
		return errors.New("there is already a chess piece here")
	}

	room.Record = append(room.Record, coor)
	room.ChessBoard[r][c] = byte(len(room.Record) % 2)

	return nil
}

func (room *Room) PlayerStone(player *Player, step byte, coor byte) error {

	if !room.IsPlaying {
		return errors.New("this room is not playing")
	}

	if int(step) != len(room.Record) {
		return errors.New("wrong step")
	}

	var correctPlayer *Player
	if (room.BlackPlayer == room.Players[0]) == (len(room.Record)%2 == 0) {
		correctPlayer = room.Players[0]
	} else {
		correctPlayer = room.Players[1]
	}

	if player != correctPlayer {
		return errors.New("not correct player")
	}

	return room.Stone(coor)
}
