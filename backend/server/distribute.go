package server

import (
	"github.com/MIXISAMA/gobang/backend/idtcp"
)

const (
	S_Version      uint16 = 0
	S_FatalError   uint16 = 1
	S_JoinRoom     uint16 = 2
	S_UserInfo     uint16 = 3
	S_LeaveRoom    uint16 = 4
	S_PlayerStone  uint16 = 5
	S_PlayerRegret uint16 = 6
	S_PlayerTie    uint16 = 7
	S_Message      uint16 = 8
)

func Empty(*idtcp.Request) error {
	return nil
}

var Endpoints = []func(*idtcp.Request) error{
	S_Version:      Empty,
	S_FatalError:   Empty,
	S_JoinRoom:     Empty,
	S_UserInfo:     Empty,
	S_LeaveRoom:    Empty,
	S_PlayerStone:  Empty,
	S_PlayerRegret: Empty,
	S_PlayerTie:    Empty,
	S_Message:      ReceiveMessage,
}

func ReceiveMessage(req *idtcp.Request) error {

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
