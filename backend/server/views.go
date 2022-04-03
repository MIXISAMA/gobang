package server

import (
	"errors"
	"flag"

	"github.com/MIXISAMA/gobang/backend/game"
	"github.com/MIXISAMA/gobang/backend/idtcp"
)

var (
	MaxOnlookers = flag.Int("max_onlookers", 10, "max onlookers")
	RoomName     = flag.String("name", "Gobang Room", "room name")
	gameRoom     *game.Room
	roomConnMap  map[string]*idtcp.Conn
	roomUserMap  map[*idtcp.Conn]game.User
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

const (
	C_PlayerJoinRoom        uint16 = 0x0000
	C_PlayerJoinRoomSuccess uint16 = 0x0000
	C_PlayerLeaveRoom       uint16 = 0x0001
	C_PlayerBlack           uint16 = 0x0002
	C_PlayerReady           uint16 = 0x0001
	C_PlayerStone           uint16 = 0x0000
	C_PlayerRegretRequest   uint16 = 0x0003
	C_PlayerAdmitDefeat     uint16 = 0x0000

	C_OnlookerJoinRoom        uint16 = 0x0002
	C_OnlookerJoinRoomSuccess uint16 = 0x0000
	C_OnlookerLeaveRoom       uint16 = 0x0000

	C_GameStart uint16 = 0x0005
	C_GameEnd   uint16 = 0x0006
)

func OnReady() {
	gameRoom = game.NewRoom(*RoomName, *MaxOnlookers)
}

func checkInRoomConnMap(conn *idtcp.Conn, name string) bool {
	for n, c := range roomConnMap {
		if conn == c || name == n {
			return true
		}
	}
	return false
}

func sendPlayers(conn *idtcp.Conn) {
	for i := range gameRoom.Players {
		player := gameRoom.Players[i]
		conn.Write(C_PlayerJoinRoom, []byte(player.Name))
		if player.Ready {
			conn.Write(C_PlayerReady, []byte(player.Name))
		}
	}
	if gameRoom.BlackPlayer != nil {
		conn.Write(C_PlayerBlack, []byte(gameRoom.BlackPlayer.Name))
	}
}

func sendOnlookers(conn *idtcp.Conn) {
	for i := range gameRoom.Onlookers {
		go conn.Write(C_OnlookerJoinRoom, []byte(gameRoom.Onlookers[i].Name))
	}
}

func JoinRoomAsPlayer(msg *idtcp.Message) error {

	playerName := string(msg.Data)

	if checkInRoomConnMap(msg.Connect, playerName) {
		return errors.New(playerName + "has been in this room")
	}

	joinPlayer, err := gameRoom.JoinPlayer(playerName)
	if err != nil {
		return err
	}

	if gameRoom.BlackPlayer == joinPlayer {
		for _, noticeConn := range roomConnMap {
			go func(noticeConn *idtcp.Conn) {
				noticeConn.Write(C_PlayerJoinRoom, []byte(playerName))
				noticeConn.Write(C_PlayerBlack, []byte(playerName))
			}(noticeConn)
		}
	} else {
		for _, noticeConn := range roomConnMap {
			noticeConn.Write(C_PlayerJoinRoom, []byte(playerName))
		}
	}

	roomConnMap[playerName] = msg.Connect
	roomUserMap[msg.Connect] = joinPlayer

	msg.Connect.Write(C_PlayerJoinRoomSuccess, []byte(gameRoom.Name))

	go sendPlayers(msg.Connect)
	go sendOnlookers(msg.Connect)

	return nil
}

func JoinRoomAsOnlooker(msg *idtcp.Message) error {

	onlookerName := string(msg.Data)

	if checkInRoomConnMap(msg.Connect, onlookerName) {
		return errors.New(onlookerName + "has been in this room")
	}

	joinOnlooker, err := gameRoom.JoinOnlooker(onlookerName)
	if err != nil {
		return err
	}

	for _, noticeConn := range roomConnMap {
		go noticeConn.Write(C_OnlookerJoinRoom, []byte(onlookerName))
	}

	roomConnMap[onlookerName] = msg.Connect
	roomUserMap[msg.Connect] = joinOnlooker

	msg.Connect.Write(C_OnlookerJoinRoomSuccess, []byte(gameRoom.Name))

	go sendPlayers(msg.Connect)
	go sendOnlookers(msg.Connect)

	return nil

}

func LeaveRoomAsPlayer(msg *idtcp.Message) error {

	playerName := string(msg.Data)

	leavePlayer, err := gameRoom.LeavePlayer(playerName)

	if err != nil {
		return err
	}

	delete(roomConnMap, leavePlayer.Name)
	delete(roomUserMap, msg.Connect)

	for _, noticeConn := range roomConnMap {
		noticeConn.Write(C_PlayerLeaveRoom, []byte(leavePlayer.Name))
	}

	return nil
}

func LeaveRoomAsOnlooker(msg *idtcp.Message) error {

	onlookerName := string(msg.Data)

	leaveOnlooker, err := gameRoom.LeavePlayer(onlookerName)

	if err != nil {
		return err
	}

	delete(roomConnMap, leaveOnlooker.Name)
	delete(roomUserMap, msg.Connect)

	for _, noticeConn := range roomConnMap {
		noticeConn.Write(C_OnlookerLeaveRoom, []byte(leaveOnlooker.Name))
	}

	return nil
}

func PlayerReady(msg *idtcp.Message) error {

	if gameRoom.IsPlaying {
		return errors.New("this room is playing")
	}

	readyUser, ok := roomUserMap[msg.Connect]
	if !ok {
		return errors.New("this player has not been exist in this room")
	}

	readyPlayer, ok := readyUser.(game.Player)
	if !ok {
		return errors.New("this player is an onlooker")
	}

	if readyPlayer.Ready {
		return errors.New("this player has ready")
	}

	readyPlayer.Ready = true

	if len(gameRoom.Players) == 2 &&
		gameRoom.Players[0].Ready &&
		gameRoom.Players[1].Ready {

		gameRoom.Players[0].Ready = false
		gameRoom.Players[1].Ready = false
		gameRoom.IsPlaying = true

		for _, noticeConn := range roomConnMap {
			noticeConn.Write(C_GameStart, nil)
		}

		return nil
	}

	for _, noticeConn := range roomConnMap {
		noticeConn.Write(C_PlayerReady, []byte(readyPlayer.Name))
	}

	return nil
}

func PlayerStone(msg *idtcp.Message) error {

	stoneUser, ok := roomUserMap[msg.Connect]
	if !ok {
		return errors.New("not user")
	}

	stonePlayer, ok := stoneUser.(*game.Player)
	if !ok {
		return errors.New("not player")
	}

	if len(msg.Data) != 2 {
		return errors.New("payload length error")
	}

	step := msg.Data[0]
	coor := msg.Data[1]

	err := gameRoom.PlayerStone(stonePlayer, step, coor)
	if err != nil {
		return err
	}

	for _, noticeConn := range roomConnMap {
		noticeConn.Write(C_PlayerStone, msg.Data)
	}

	return nil
}
