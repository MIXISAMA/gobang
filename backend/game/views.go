package game

import (
	"errors"
	"fmt"
	"log"

	"github.com/MIXISAMA/gobang/backend/config"
	"github.com/MIXISAMA/gobang/backend/server"
)

const Version = "0.1"

var Gd = GlobalData{
	Rooms:     make([]*Room, 0),
	UserWho:   make(map[string]*server.User),
	RoomWhose: make(map[*server.User]*Room),
}

func LoadGdFromConfig(conf *config.Config) error {

	Gd.Rooms = make([]*Room, 0)
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
		Gd.Rooms = append(Gd.Rooms, room)

	}
	return nil

}

func ReceiveJoinRoom(msg *server.IdtcpMessage) error {

	room, name, is_player, err := DecodeJoinRoom(msg.Data)
	if err != nil {
		return err
	}

	err = msg.User.Rename(name)
	if err != nil {
		return err
	}

	err = Gd.AddUserRoom(msg.User, room, is_player)
	if err != nil {
		// push msg
		return err
	}

	err = SendAllRoomInformation(msg.User.Conn, room)
	if err != nil {
		Gd.RemoveUser(msg.User)
		log.Println("failed to send all room info")
		return err
	}

	for i := range room.Users {
		err := SendJoinRoom(room.Users[i].Conn, room, msg.User)
		if err != nil {
			continue
		}
	}

	return nil
}

func LeaveRoom(user *server.User) error {

	room, err := Gd.RemoveUser(user)
	if err != nil {
		return err
	}

	for i := range room.Users {
		err := SendLeaveRoom(room.Users[i].Conn, user)
		if err != nil {
			continue
		}
	}
	return nil
}

func ReceiveLeaveRoom(msg *server.IdtcpMessage) error {
	return LeaveRoom(msg.User)
}

func ReceiveMessage(msg *server.IdtcpMessage) error {

	room, err := Gd.GetRoom(msg.User)
	if err != nil {
		return err
	}

	text, err := DecodeString(msg.Data)
	if err != nil {
		return err
	}

	for i := range room.Users {
		err := SendMessage(room.Users[i].Conn, msg.User.Name, text)
		if err != nil {
			continue
		}
	}
	return nil

}

/*

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

*/

func OnDisconnect(user *server.User) {

	LeaveRoom(user)

}

func UdpPipe(msg *server.UdpMessage) error {

	s := server.MakeSerializer(msg.Data)

	version, err := s.ReadString()
	if err != nil {
		return err
	}

	if version != Version {
		return fmt.Errorf("inconsistent version %s", version)
	}

	data, err := EncodeRooms()
	if err != nil {
		return err
	}

	_, err = msg.Conn.WriteToUDP(data, msg.Addr)
	return err

}
