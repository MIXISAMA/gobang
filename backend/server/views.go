package server

import (
	"errors"
	"net"

	"github.com/MIXISAMA/gobang/backend/game"
	"github.com/MIXISAMA/gobang/backend/idtcp"
)

type Room struct {
	game.Room
	MaxOnlookers int
	Onlookers    []string
	ConnMap      map[string]*idtcp.Conn
}

func NewRoom(name string, maxOnlookers int) *Room {
	return &Room{
		Room:         *game.NewRoom(name),
		MaxOnlookers: maxOnlookers,
	}
}

type Rooms []Room

func (rooms *Rooms) String() string {
	var str string
	for i := range *rooms {
		str += (*rooms)[i].Name
		str += " | "
	}
	return str
}

func (rooms *Rooms) Set(value string) error {
	*rooms = append(*rooms, *NewRoom(value, 256))
	return nil
}

var RoomList Rooms

func (room *Room) checkInConnMap(conn *idtcp.Conn, name string) bool {
	for n, c := range room.ConnMap {
		if conn == c || name == n {
			return true
		}
	}
	return false
}

func JoinRoomAsPlayer(msg *idtcp.Message) error {

	s := MakeSerializer(msg.Data)

	roomId, err := s.ReadUint16()
	if err != nil || int(roomId) >= len(RoomList) {
		return errors.New("wrong room id")
	}
	room := RoomList[roomId]

	playerName, err := s.ReadString()
	if err != nil {
		return err
	}
	if len(playerName) > 64 {
		return errors.New("name is too long")
	}

	if room.checkInConnMap(msg.Connect, playerName) {
		return errors.New(playerName + " has been in this room")
	}

	joinPlayer, err := room.PlayerJoin(playerName)
	if err != nil {
		return err
	}

	room.ConnMap[playerName] = msg.Connect

	err = room.sendAll(msg.Connect)
	if err != nil {
		return err
	}

	err = room.sendJoinPlayerToOthers(joinPlayer)
	if err != nil {
		return err
	}

	return nil
}

func JoinRoomAsOnlooker(msg *idtcp.Message) error {

	s := MakeSerializer(msg.Data)

	roomId, err := s.ReadUint16()
	if err != nil || int(roomId) >= len(RoomList) {
		return errors.New("wrong room id")
	}
	room := RoomList[roomId]

	onlookerName, err := s.ReadString()
	if err != nil {
		return err
	}
	if len(onlookerName) > 64 {
		return errors.New("name is too long")
	}

	if room.checkInConnMap(msg.Connect, onlookerName) {
		return errors.New(onlookerName + " has been in this room")
	}

	err = room.sendAll(msg.Connect)
	if err != nil {
		return err
	}

	err = room.sendJoinOnlookerToOthers(onlookerName)
	if err != nil {
		return err
	}

	return nil

}

func LeaveRoomAsPlayer(msg *idtcp.Message) error {

	s := MakeSerializer(msg.Data)

	roomId, err := s.ReadUint16()
	if err != nil || int(roomId) >= len(RoomList) {
		return errors.New("wrong room id")
	}
	room := RoomList[roomId]

	playerName, err := s.ReadString()
	if err != nil {
		return err
	}
	if len(playerName) > 64 {
		return errors.New("name is too long")
	}

	if !room.checkInConnMap(msg.Connect, playerName) {
		return errors.New(playerName + " is not in this room")
	}

	leavePlayer, err := room.PlayerLeave(playerName)
	if err != nil {
		return err
	}

	delete(room.ConnMap, leavePlayer.Name)

	room.sendLeavePlayerToOthers(playerName)
	sendPlayerLeaveSuccess(msg.Connect)

	return nil
}

func (room *Room) LeaveOnlooker(name string) error {
	for i := range room.Onlookers {
		if name == room.Onlookers[i] {
			room.Onlookers = append(room.Onlookers[:i], room.Onlookers[i+1:]...)
			return nil
		}
	}
	return errors.New(name + " is not in onlookers")
}

func LeaveRoomAsOnlooker(msg *idtcp.Message) error {

	s := MakeSerializer(msg.Data)

	roomId, err := s.ReadUint16()
	if err != nil || int(roomId) >= len(RoomList) {
		return errors.New("wrong room id")
	}
	room := RoomList[roomId]

	onlookerName, err := s.ReadString()
	if err != nil {
		return err
	}
	if len(onlookerName) > 64 {
		return errors.New("name is too long")
	}

	if !room.checkInConnMap(msg.Connect, onlookerName) {
		return errors.New(onlookerName + " is not in this room")
	}

	err = room.LeaveOnlooker(onlookerName)
	if err != nil {
		return err
	}

	delete(room.ConnMap, onlookerName)

	room.sendLeaveOnlookerToOthers(onlookerName)
	sendOnlookerLeaveSuccess(msg.Connect)

	return nil
}

// func PlayerReady(msg *idtcp.Message) error {

// 	if gameRoom.IsPlaying {
// 		return errors.New("this room is playing")
// 	}

// 	readyUser, ok := roomUserMap[msg.Connect]
// 	if !ok {
// 		return errors.New("this player has not been exist in this room")
// 	}

// 	readyPlayer, ok := readyUser.(game.Player)
// 	if !ok {
// 		return errors.New("this player is an onlooker")
// 	}

// 	if readyPlayer.Ready {
// 		return errors.New("this player has ready")
// 	}

// 	readyPlayer.Ready = true

// 	if len(gameRoom.Players) == 2 &&
// 		gameRoom.Players[0].Ready &&
// 		gameRoom.Players[1].Ready {

// 		gameRoom.Players[0].Ready = false
// 		gameRoom.Players[1].Ready = false
// 		gameRoom.IsPlaying = true

// 		for _, noticeConn := range roomConnMap {
// 			noticeConn.Write(C_GameStart, nil)
// 		}

// 		return nil
// 	}

// 	for _, noticeConn := range roomConnMap {
// 		noticeConn.Write(C_PlayerReady, []byte(readyPlayer.Name))
// 	}

// 	return nil
// }

// func PlayerStone(msg *idtcp.Message) error {

// 	stoneUser, ok := roomUserMap[msg.Connect]
// 	if !ok {
// 		return errors.New("not user")
// 	}

// 	stonePlayer, ok := stoneUser.(*game.Player)
// 	if !ok {
// 		return errors.New("not player")
// 	}

// 	if len(msg.Data) != 2 {
// 		return errors.New("payload length error")
// 	}

// 	step := msg.Data[0]
// 	coor := msg.Data[1]

// 	err := gameRoom.PlayerStone(stonePlayer, step, coor)
// 	if err != nil {
// 		return err
// 	}

// 	for _, noticeConn := range roomConnMap {
// 		noticeConn.Write(C_PlayerStone, msg.Data)
// 	}

// 	return nil
// }

func SendAllRoom(data []byte, conn *net.UDPConn, addr *net.UDPAddr) {
	var s Serializer
	err := s.WriteUint16_Int(len(RoomList))
	if err != nil {
		return
	}

	for i := range RoomList {

		room := &RoomList[i]

		err = s.WriteUint16_Int(i)
		if err != nil {
			return
		}

		err = s.WriteString(room.Name)
		if err != nil {
			return
		}

		for j := range room.Players {
			player := room.Players[j]
			if player != nil {
				s.WriteByte(0xFF)
				s.WriteString(player.Name)
			} else {
				s.WriteByte(0x00)
			}
		}
		s.WriteUint16_Int(len(room.Onlookers))

	}
	conn.WriteToUDP(s.Raw, addr)
}
