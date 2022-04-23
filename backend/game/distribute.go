package game

import (
	"github.com/MIXISAMA/gobang/backend/server"
)

const (
	S_PlayerJoinRoom      uint16 = 0x0000
	S_PlayerLeaveRoom     uint16 = 0x0002
	S_PlayerReady         uint16 = 0x0004
	S_PlayerStone         uint16 = 0x0000
	S_PlayerRegretRequest uint16 = 0x0000
	S_PlayerRegretPermit  uint16 = 0x0000
	S_PlayerRegretReject  uint16 = 0x0000
	S_PlayerAdmitDefeat   uint16 = 0x0000

	S_OnlookerJoinRoom  uint16 = 0x0001
	S_OnlookerLeaveRoom uint16 = 0x0003
)

var Endpoints = []func(*server.IdtcpMessage) error{
	S_PlayerJoinRoom: JoinRoomAsPlayer,
	// S_OnlookerJoinRoom: JoinRoomAsOnlooker,
	// S_PlayerReady:      PlayerReady,
	// 0x03: RetractRequest,
	// 0x04: Resign,
	// 0x05: Stone,
}
