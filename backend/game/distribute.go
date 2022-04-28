package game

import (
	"github.com/MIXISAMA/gobang/backend/server"
)

const (
	S_GenericErrorNotification uint16 = 0x0000
	S_AllRoomInformation       uint16 = 0x0001
	S_Join_Room                uint16 = 0x0002
	S_Leave_Room               uint16 = 0x0003
	S_Message                  uint16 = 0x0004
	// S_PlayerReady              uint16 = 0x0004
	// S_PlayerStone              uint16 = 0x0000
	// S_PlayerRegretRequest      uint16 = 0x0000
	// S_PlayerRegretPermit       uint16 = 0x0000
	// S_PlayerRegretReject       uint16 = 0x0000
	// S_PlayerAdmitDefeat        uint16 = 0x0000
)

func Empty(*server.IdtcpMessage) error {
	return nil
}

var Endpoints = []func(*server.IdtcpMessage) error{
	S_GenericErrorNotification: Empty,
	S_AllRoomInformation:       Empty,
	S_Join_Room:                ReceiveJoinRoom,
	S_Leave_Room:               ReceiveLeaveRoom,
	S_Message:                  ReceiveMessage,
}
