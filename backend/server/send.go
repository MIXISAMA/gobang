package server

import (
	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwroom"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
	"github.com/MIXISAMA/gobang/backend/utils"
)

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

func sendVoid(conn *idtcp.Conn, instruction uint16) error {
	_, err := conn.Write(instruction, make([]byte, 0))
	return err
}

func sendByte(conn *idtcp.Conn, instruction uint16, b byte) error {
	var s utils.Serializer
	err := s.WriteByte(b)
	if err != nil {
		return err
	}
	_, err = conn.Write(instruction, s.Raw)
	return err
}

func sendBoolean(conn *idtcp.Conn, instruction uint16, b bool) error {
	var s utils.Serializer
	s.WriteBoolean(b)
	_, err := conn.Write(instruction, s.Raw)
	return err
}

func SendString(conn *idtcp.Conn, instruction uint16, str string) error {
	var s utils.Serializer
	s.WriteString8(str)
	_, err := conn.Write(instruction, s.Raw)
	return err
}

func SendYouJoinRoom(
	conn *idtcp.Conn,
	isSuccess bool,
	room *mdwroom.Room,
) error {

	var s utils.Serializer

	s.WriteBoolean(isSuccess)

	if !isSuccess {
		_, err := conn.Write(C_YouJoinRoom, s.Raw)
		return err
	}

	s.WriteString8(room.Name)

	s.WriteUint8_Int(room.MaxUsers)

	var blackUsername = room.BlackPlayer.(*mdwuser.User).GetUsername("")
	var whiteUsername = room.WhitePlayer.(*mdwuser.User).GetUsername("")

	s.WriteString8(blackUsername)
	s.WriteString8(whiteUsername)

	s.WriteUint8_Int(room.Users.Len())
	for i := room.Users.Front(); i != nil; i = i.Next() {
		user := i.Value.(*mdwuser.User)
		if user == room.BlackPlayer || user == room.WhitePlayer {
			continue
		}
		s.WriteString8(user.Username)
	}

	s.WriteByte(room.ReadyColor)
	s.WriteBoolean(room.IsPlaying)
	s.WriteByte(room.RegretColor)
	s.WriteByte(room.TieColor)

	s.WriteBytes8(room.Chess.GetRecords())

	_, err := conn.Write(C_YouJoinRoom, s.Raw)
	return err
}

func SendUserInfo(conn *idtcp.Conn, user *mdwuser.User) error {

	var s utils.Serializer

	s.WriteString8(user.Username)
	s.WriteUint32(user.NumOfWins)
	s.WriteUint32(user.NumOfTies)
	s.WriteUint32(user.NumOfLosses)
	s.WriteUint32(user.NumOfMatches)
	s.WriteUint64(user.GameDuration)

	_, err := conn.Write(C_UserInfo, s.Raw)
	return err
}

func SendMessage(conn *idtcp.Conn, username string, text string) error {

	var s utils.Serializer

	s.WriteString8(username)
	s.WriteString16(text)

	_, err := conn.Write(C_Message, s.Raw)
	return err
}
