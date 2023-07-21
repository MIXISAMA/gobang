package server

import (
	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
	"github.com/MIXISAMA/gobang/backend/utils"
)

const (
	C_FatalError    uint16 = 0
	C_PublicKey     uint16 = 1
	C_YouJoinRoom   uint16 = 2
	C_UserInfo      uint16 = 3
	C_OtherJoinRoom uint16 = 4
	C_UserLeaveRoom uint16 = 5
	C_PlayerReady   uint16 = 6
	C_GameStart     uint16 = 7
	C_Stone         uint16 = 8
	C_PlayerRegret  uint16 = 9
	C_AgreeRegret   uint16 = 10
	C_PlayerTie     uint16 = 11
	C_AgreeTie      uint16 = 12
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

func SendGameOver(conn *idtcp.Conn, color byte) error {
	return sendByte(conn, C_GameOver, color)
}

func SendMessage(conn *idtcp.Conn, username string, text string) error {

	var s utils.Serializer

	s.WriteString8(username)
	s.WriteString16(text)

	_, err := conn.Write(C_Message, s.Raw)
	return err
}
