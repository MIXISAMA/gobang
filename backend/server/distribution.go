package server

import (
	"errors"

	"github.com/MIXISAMA/gobang/backend/idtcp"
)

var endpoints = [...]func(*idtcp.Message) error{
	S_PlayerJoinRoom:   JoinRoomAsPlayer,
	S_OnlookerJoinRoom: JoinRoomAsOnlooker,
	S_PlayerReady:      PlayerReady,
	// 0x03: RetractRequest,
	// 0x04: Resign,
	// 0x05: Stone,
}

func Distribute(msg *idtcp.Message) error {

	if msg.Instruction < 0 || int(msg.Instruction) >= len(endpoints) {
		return errors.New("error instruction")
	}

	endpoints[msg.Instruction](msg)

	return nil
}
