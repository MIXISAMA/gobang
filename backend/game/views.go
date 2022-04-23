package game

import (
	"errors"

	"github.com/MIXISAMA/gobang/backend/config"
	"github.com/MIXISAMA/gobang/backend/server"
)

var RoomList []Room

func LoadRoomListFromConfig(conf *config.Config) error {

	RoomList = make([]Room, 0)
	for i := range conf.Rooms {

		roomName := conf.Rooms[i].Name
		maxUsers := conf.Rooms[i].MaxUsers
		if len(roomName) >= 64 {
			return errors.New("the length of the room name cannot exceed 64 bytes")
		}
		if maxUsers >= 256 || maxUsers <= 2 {
			return errors.New("the max nubmer of users is between 2 and 255")
		}

		room := NewRoom(roomName, maxUsers)
		RoomList = append(RoomList, *room)

	}
	return nil

}

func JoinRoomAsPlayer(msg *server.IdtcpMessage) error {

	roomId, playerName, err := SerializerUint16String(msg.Data)
	if err != nil {
		return err
	}

	if int(roomId) >= len(RoomList) {
		return errors.New("wrong room id")
	}
	room := RoomList[roomId]

	err = msg.User.Rename(playerName)
	if err != nil {
		return err
	}

	err = room.PlayerJoin(msg.User)
	if err != nil {
		return err
	}

	// err = room.sendAll(msg.Connect)
	// if err != nil {
	// 	return err
	// }

	// err = room.sendJoinPlayerToOthers(joinPlayer)
	// if err != nil {
	// 	return err
	// }

	return nil
}

/*
import (
	"errors"
	"fmt"
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

var RoomMap map[*idtcp.Conn]*Room = make(map[*idtcp.Conn]*Room)

func NewRoom(name string, maxOnlookers int) *Room {
	return &Room{
		Room:         *game.NewRoom(name),
		MaxOnlookers: maxOnlookers,
		ConnMap:      make(map[string]*idtcp.Conn),
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

		s.WriteBoolean(room.IsPlaying)

		for j := range room.Players {
			player := room.Players[j]
			if player != nil {
				s.WriteByte(0xFF)
				s.WriteString(player.Name)
			} else {
				s.WriteByte(0x00)
			}
		}

		err = s.WriteUint16_Int(len(room.Onlookers))
		if err != nil {
			return
		}

		err = s.WriteUint16_Int(room.MaxOnlookers)
		if err != nil {
			return
		}

	}
	conn.WriteToUDP(s.Raw, addr)
}

// func disconnection(idtcp.Conn)
// {

// }

*/

func UdpPipe(msg *server.UdpMessage) error {
	return nil
}
