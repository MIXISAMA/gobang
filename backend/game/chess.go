package game

import "errors"

const (
	SPACE byte = 0xFF
	BLACK byte = 0x00
	WHITE byte = 0x01
)

type Chess struct {
	Board  [15][15]byte
	Record []byte
}

func NewChess() *Chess {
	var chess = Chess{Record: make([]byte, 0)}
	for i := range chess.Board {
		for j := range chess.Board[i] {
			chess.Board[i][j] = SPACE
		}
	}
	return &chess
}

func (chess *Chess) WhosTurn() byte {
	return byte(len(chess.Record) % 2)
}

func (chess *Chess) Stone(coor byte, color byte) error {

	if color != chess.WhosTurn() {
		return errors.New("wrong color")
	}

	r := coor & 0x0F
	c := (coor >> 4) & 0x0F

	if r >= 15 || c >= 15 {
		return errors.New("out of chess board")
	}
	if chess.Board[r][c] != SPACE {
		return errors.New("there is already a chess piece here")
	}

	chess.Record = append(chess.Record, coor)
	chess.Board[r][c] = color
	return nil

}
