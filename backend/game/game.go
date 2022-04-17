package game

import (
	"errors"
)

const (
	BLACK = 0x00
	WHITE = 0x01
)

type Player struct {
	Name          string
	Ready         bool
	Color         byte
	RegretRequest bool
	Room          *Room
}

type Room struct {
	Name       string
	Players    [2]*Player
	IsPlaying  bool
	ChessBoard [15][15]byte
	Record     []byte
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

	p1 := room.Players[0]
	p2 := room.Players[1]

	if p1 != nil && p2 != nil {
		p1.Color, p2.Color = p2.Color, p1.Color
	}

	if p1 != nil {
		p1.RegretRequest = false
	}

	if p2 != nil {
		p2.RegretRequest = false
	}

	room.initChessBoard()
	room.Record = nil

}

func NewRoom(name string) *Room {
	room := new(Room)
	room.Name = name
	room.Clean()
	return room
}

func (room *Room) PlayerJoin(playerName string) (joinPlayer *Player, err error) {

	if room.IsPlaying {
		return nil, errors.New("this room is playing")
	}

	p1 := room.Players[0]
	p2 := room.Players[1]

	if p1 != nil && p2 != nil {
		return nil, errors.New("this room is full")
	}

	joinPlayer = &Player{
		Name:          playerName,
		Color:         BLACK,
		Ready:         false,
		RegretRequest: false,
		Room:          room,
	}

	if p1 == nil && p2 == nil {
		room.Players[0] = joinPlayer
		return joinPlayer, nil
	}

	if p1 == nil && p2 != nil {
		if p2.Name == playerName {
			return nil, errors.New("same name player has been exist in this room")
		}
		room.Players[0] = joinPlayer
	} else { // p1 != nil && p2 == nil
		if p1.Name == playerName {
			return nil, errors.New("same name player has been exist in this room")
		}
		room.Players[1] = joinPlayer
	}

	if p1.Color == p2.Color {
		joinPlayer.Color = WHITE
	}

	return joinPlayer, nil
}

func (room *Room) FindPlayer(playerName string) (player *Player, err error) {
	for i := range room.Players {
		if room.Players[i].Name == playerName {
			return room.Players[i], nil
		}
	}
	return nil, errors.New("this player is not exist in this room")
}

func (room *Room) FindPlayerIndex(playerName string) (i int, err error) {
	for i = range room.Players {
		if room.Players[i].Name == playerName {
			return i, nil
		}
	}
	return -1, errors.New("this player is not exist in this room")
}

func (room *Room) PlayerLeave(playerName string) (leavePlayer *Player, err error) {

	if room.IsPlaying {
		return nil, errors.New("this room is playing")
	}

	i, err := room.FindPlayerIndex(playerName)
	if err != nil {
		return nil, err
	}

	leavePlayer = room.Players[i]
	room.Players[i] = nil
	return

}

func (player Player) Stone(coor byte) error {

	room := player.Room

	if !room.IsPlaying {
		return errors.New("this room is not playing")
	}

	if (len(room.Record)%2 == 0) == (player.Color == WHITE) {
		return errors.New("not correct player")
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
	room.ChessBoard[r][c] = player.Color

	return nil

}
