package game

import (
	"errors"
)

type Player interface{}

type Room struct {
	whitePlayer *Player
	blackPlayer *Player
	regret      byte
	tie         byte
	ready       byte
	isPlaying   bool
	chess       Chess
}

func (room Room) init() {
	room.whitePlayer = nil
	room.blackPlayer = nil
	room.regret = SPACE
	room.tie = SPACE
	room.ready = SPACE
	room.isPlaying = false
	room.chess.Init()
}

func (room *Room) Join(player *Player, color byte) error {
	if color == BLACK {
		if room.blackPlayer != nil {
			return errors.New("black player has exist")
		}
		room.blackPlayer = player
	} else if color == WHITE {
		if room.whitePlayer != nil {
			return errors.New("white player has exist")
		}
		room.whitePlayer = player
	} else {
		return errors.New("wrong color")
	}
	return nil
}

func (room *Room) Leave(player *Player) error {
	color := SPACE
	if player == room.blackPlayer {
		room.blackPlayer = nil
		color = BLACK
	} else if player == room.whitePlayer {
		room.whitePlayer = nil
		color = WHITE
	} else {
		return errors.New("wrong player")
	}
	if room.regret == color {
		room.regret = SPACE
	}
	if room.tie == color {
		room.tie = SPACE
	}
	if room.ready == color {
		room.ready = SPACE
	}
	room.isPlaying = false
	return nil
}

func (room *Room) Ready(player *Player) (bool, error) {
	if player == room.blackPlayer {
		if room.ready == WHITE {
			room.ready = SPACE
			return true, nil
		}
		room.ready = BLACK
		return false, nil
	}
	if player == room.whitePlayer {
		if room.ready == BLACK {
			room.ready = SPACE
			return true, nil
		}
		room.ready = WHITE
		return false, nil
	}
	return false, errors.New("wrong player")
}
