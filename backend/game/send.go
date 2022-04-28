package game

import (
	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/server"
)

const (
	C_GenericErrorNotification uint16 = 0x0000
	C_AllRoomInformation       uint16 = 0x0001
	C_Join_Room                uint16 = 0x0002
	C_Leave_Room               uint16 = 0x0003
	C_Message                  uint16 = 0x0004
)

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
