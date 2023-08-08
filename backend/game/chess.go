package game

import "errors"

const (
	SPACE byte = 0xFF
	BLACK byte = 0x00
	WHITE byte = 0x01
)

type Chess struct {
	board  [15][15]byte
	record [15 * 15]byte
	cursor int
}

func (chess *Chess) Init() {
	chess.cursor = 0
	for i := range chess.board {
		for j := range chess.board[i] {
			chess.board[i][j] = SPACE
		}
	}
}

// 0 will be black's turn, 1 will be white's
func (chess *Chess) WhoseTurn() byte {
	return byte(chess.cursor % 2)
}

func (chess *Chess) Stone(move int, coor byte) error {
	if move != chess.cursor {
		return errors.New("move number is wrong")
	}

	r, c := parseCoor(coor)
	if !isVaildCoor(r, c) {
		return errors.New("stone out of bounds")
	}

	if chess.board[r][c] != SPACE {
		return errors.New("there is already a chess piece here")
	}

	chess.record[chess.cursor] = coor
	chess.board[r][c] = chess.WhoseTurn()
	chess.cursor++

	return nil
}

func (chess *Chess) DidYouWin() bool {
	if chess.cursor <= 0 {
		return false
	}

	r, c := parseCoor(chess.record[chess.cursor-1])

	for i := range dr {
		count := 1
		for j := 1; j <= 4; j++ {
			rr := r + j*dr[i]
			cc := c + j*dc[i]
			if !isVaildCoor(rr, cc) || chess.board[r][c] != chess.board[rr][cc] {
				break
			}
			count++
		}
		for j := -1; j >= -4; j-- {
			rr := r + j*dr[i]
			cc := c + j*dc[i]
			if !isVaildCoor(rr, cc) || chess.board[r][c] != chess.board[rr][cc] {
				break
			}
			count++
		}
		if count >= 5 {
			return true
		}
	}
	return false
}

func (chess *Chess) Regret(color byte) error {
	if color == chess.WhoseTurn() {
		if chess.cursor-1 < 0 {
			return errors.New("can not regret")
		}
		chess.cursor -= 1
	} else {
		if chess.cursor-2 < 0 {
			return errors.New("can not regret")
		}
		chess.cursor -= 2
	}
	return nil
}

func (chess *Chess) GetRecords() []byte {
	return chess.record[:chess.cursor]
}

func parseCoor(coor byte) (r int, c int) {
	return int(coor & 0x0F), int((coor >> 4) & 0x0F)
}

func isVaildCoor(r int, c int) bool {
	return 0 <= r && r < 15 && 0 <= c && c < 15
}

var dr = [4]int{0, 1, 1, 1}
var dc = [4]int{1, 1, 0, -1}
