package server

import "github.com/MIXISAMA/gobang/backend/idtcp"

const (
	C_Version       uint16 = 0
	C_FatalError    uint16 = 1
	C_PublicKey     uint16 = 2
	C_YouJoinRoom   uint16 = 3
	C_UserInfo      uint16 = 4
	C_OtherJoinRoom uint16 = 5
	C_UserLeaveRoom uint16 = 6
	C_PlayerReady   uint16 = 7
	C_GameStart     uint16 = 8
	C_Stone         uint16 = 9
	C_PlayerRegret  uint16 = 10
	C_AgreeRegret   uint16 = 11
	C_PlayerTie     uint16 = 12
	C_GameOver      uint16 = 13
	C_Message       uint16 = 14
)

func sendVoid(conn *idtcp.Conn, instruction uint16) {
	conn.Write(instruction, make([]byte, 0))
}

func SendAllRoomInformation(conn *idtcp.Conn, room *Room) error {

	data, err := EncodeAllRoomInformation(room)
	if err != nil {
		return err
	}

	_, err = conn.Write(C_AllRoomInformation, data)
	return err

}

func SendJoinRoom(
	conn *idtcp.Conn,
	room *Room,
	user *server.User,
) error {

	data, err := EncodeUserInRoom(room, user)
	if err != nil {
		return err
	}

	_, err = conn.Write(C_Join_Room, data)
	return err
}

func SendLeaveRoom(conn *idtcp.Conn, user *server.User) error {

	data, err := EncodeString(user.Name)
	if err != nil {
		return err
	}

	_, err = conn.Write(C_Leave_Room, data)
	return err
}

func SendMessage(conn *idtcp.Conn, username string, text string) error {

	data, err := EncodeStringString(username, text)
	if err != nil {
		return err
	}

	_, err = conn.Write(C_Message, data)
	return err
}
