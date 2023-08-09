package game

import (
	"github.com/stretchr/testify/assert"
	"testing"
)

func encodeCoor(r int, c int) byte {
	var coor byte
	coor = byte(c & 0x0F)
	coor = coor << 4
	coor = coor | byte(r&0x0F)
	return coor
}

func TestGame(t *testing.T) {
	assert := assert.New(t)
	chess := Chess{}
	chess.Init()

	// this should be
	// ......O........
	// .OX.OX.........
	// ..OOX..........
	// ..OX...........
	// ..X............
	// .X.............
	// ...............
	// ...............
	// ...............
	// ...............
	// ...............
	// ...............
	// ...............
	// ...............
	// ...............

	err := chess.Stone(0, encodeCoor(1, 1)) // O
	assert.Nil(err)
	err = chess.Stone(1, encodeCoor(1, 2)) // X
	assert.Nil(err)
	err = chess.Stone(2, encodeCoor(2, 2)) // O
	assert.Nil(err)
	err = chess.Stone(3, encodeCoor(3, 3)) // X
	assert.Nil(err)
	err = chess.Stone(4, encodeCoor(3, 2)) // O
	assert.Nil(err)
	err = chess.Stone(5, encodeCoor(4, 2)) // X
	assert.Nil(err)
	err = chess.Stone(6, encodeCoor(2, 3)) // O
	assert.Nil(err)
	err = chess.Stone(7, encodeCoor(2, 4)) // X
	assert.Nil(err)
	err = chess.Stone(8, encodeCoor(1, 4)) // O
	assert.Nil(err)
	err = chess.Stone(9, encodeCoor(1, 5)) // X
	assert.Nil(err)
	err = chess.Stone(10, encodeCoor(0, 6)) // O
	assert.Nil(err)
	err = chess.Stone(11, encodeCoor(5, 1)) // X
	assert.Nil(err)
	assert.True(chess.DidYouWin())
}
