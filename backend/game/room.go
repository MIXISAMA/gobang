package game

import (
	"errors"
)

type Player interface{}

type Room struct {
	WhitePlayer Player
	BlackPlayer Player
	RegretColor byte
	TieColor    byte
	ReadyColor  byte
	IsPlaying   bool
	Chess       Chess
}

func NewRoom() *Room {
	room := Room{
		WhitePlayer: nil,
		BlackPlayer: nil,
		RegretColor: SPACE,
		TieColor:    SPACE,
		ReadyColor:  SPACE,
		IsPlaying:   false,
	}
	room.Chess.Init()
	return &room
}

func (room *Room) Join(player Player, color byte) error {
	if color == BLACK {
		if room.BlackPlayer != nil {
			return errors.New("black player has exist")
		}
		room.BlackPlayer = player
	} else if color == WHITE {
		if room.WhitePlayer != nil {
			return errors.New("white player has exist")
		}
		room.WhitePlayer = player
	} else {
		return errors.New("wrong color")
	}
	return nil
}

func (room *Room) Leave(player Player) error {
	color := SPACE
	if player == room.BlackPlayer {
		room.BlackPlayer = nil
		color = BLACK
	} else if player == room.WhitePlayer {
		room.WhitePlayer = nil
		color = WHITE
	} else {
		return errors.New("wrong player")
	}
	if room.RegretColor == color {
		room.RegretColor = SPACE
	}
	if room.TieColor == color {
		room.TieColor = SPACE
	}
	if room.ReadyColor == color {
		room.ReadyColor = SPACE
	}
	room.IsPlaying = false
	return nil
}

func (room *Room) Ready(player Player) (bool, error) {
	if player == room.BlackPlayer {
		if room.ReadyColor == WHITE {
			room.ReadyColor = SPACE
			return true, nil
		}
		room.ReadyColor = BLACK
		return false, nil
	}
	if player == room.WhitePlayer {
		if room.ReadyColor == BLACK {
			room.ReadyColor = SPACE
			return true, nil
		}
		room.ReadyColor = WHITE
		return false, nil
	}
	return false, errors.New("wrong player")
}

func (room *Room) Opponent(player Player) (Player, error) {
	if room.BlackPlayer == player {
		return room.WhitePlayer, nil
	}
	if room.WhitePlayer == player {
		return room.BlackPlayer, nil
	}
	return nil, errors.New("not player")
}
