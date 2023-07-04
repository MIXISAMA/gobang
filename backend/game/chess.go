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

func (chess Chess) Init() {
	chess.cursor = 0
	for i := range chess.board {
		for j := range chess.board[i] {
			chess.board[i][j] = SPACE
		}
	}
}

func (chess Chess) WhosTurn() byte {
	return byte(chess.cursor % 2)
}

func (chess *Chess) Stone(coor byte) (bool, error) {

	r, c := parseCoor(coor)
	if !isVaildCoor(r, c) {
		return false, errors.New("stone out of bounds")
	}

	if chess.board[r][c] != SPACE {
		return false, errors.New("there is already a chess piece here")
	}

	chess.record[chess.cursor] = coor
	chess.board[r][c] = chess.WhosTurn()
	chess.cursor++

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
			return true, nil
		}
	}
	return false, nil
}

func parseCoor(coor byte) (int, int) {
	return int(coor & 0x0F), int((coor >> 4) & 0x0F)
}

func isVaildCoor(r int, c int) bool {
	return 0 <= r && r < 15 && 0 <= c && c < 15
}

var dr = [4]int{0, 1, 1, 1}
var dc = [4]int{1, 1, 0, -1}
