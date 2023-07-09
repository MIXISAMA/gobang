package server

import (
	"log"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwroom"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
	"github.com/MIXISAMA/gobang/backend/utils"
)

const (
	S_Version      uint16 = 0
	S_FatalError   uint16 = 1
	S_JoinRoom     uint16 = 2
	S_UserInfo     uint16 = 3
	S_LeaveRoom    uint16 = 4
	S_PlayerStone  uint16 = 5
	S_PlayerRegret uint16 = 6
	S_AgreeRegret  uint16 = 7
	S_PlayerTie    uint16 = 8
	S_AgreeTie     uint16 = 9
	S_GiveUp       uint16 = 10
	S_Message      uint16 = 11
)

func Empty(*idtcp.Request) error {
	return nil
}

var Endpoints = []func(*idtcp.Request) error{
	S_Version:      Empty,
	S_FatalError:   Empty,
	S_JoinRoom:     ReceiveJoinRoom,
	S_UserInfo:     ReceiveUserInfo,
	S_LeaveRoom:    Empty,
	S_PlayerStone:  ReceivePlayerStone,
	S_PlayerRegret: ReceivePlayerRegret,
	S_AgreeRegret:  Empty,
	S_PlayerTie:    Empty,
	S_AgreeTie:     Empty,
	S_GiveUp:       Empty,
	S_Message:      ReceiveMessage,
}

func ReceiveJoinRoom(req *idtcp.Request) error {

	room := req.DistPayloads[mdwroom.Key].(*mdwroom.DistributePayload).Room

	return SendYouJoinRoom(req.Conn, true, room)
}

func ReceiveUserInfo(req *idtcp.Request) error {
	room := req.DistPayloads[mdwroom.Key].(*mdwroom.DistributePayload).Room
	s := utils.MakeSerializer(req.Data)
	username, err := s.ReadString8()
	if err != nil {
		return err
	}
	user := room.FindUser(username)
	if user == nil {
		return nil
	}
	return SendUserInfo(req.Conn, user)
}

func ReceiveLeaveRoom(req *idtcp.Request) error {
	return sendVoid(req.Conn, C_UserLeaveRoom)
}

func ReceivePlayerStone(req *idtcp.Request) error {
	room := req.DistPayloads[mdwroom.Key].(*mdwroom.DistributePayload).Room

	s := utils.MakeSerializer(req.Data)
	move, err := s.ReadUint8()
	if err != nil {
		return err
	}
	coor, err := s.ReadByte()
	if err != nil {
		return err
	}

	who := room.Chess.WhoseTurn()

	err = room.Chess.Stone(int(move), coor)
	if err != nil {
		return nil
	}

	win := room.Chess.DidYouWin()

	for i := room.Users.Front(); i != room.Users.Back(); i = i.Next() {
		user := i.Value.(*mdwuser.User)
		err = sendByte(user.Conn, C_Stone, coor)
		if win {
			err = sendByte(user.Conn, C_GameOver, who)
		}
		if err != nil {
			log.Println(err.Error())
		}
	}
	return nil
}

func ReceivePlayerRegret(req *idtcp.Request) error {
	return sendVoid(req.Conn, C_PlayerRegret)
}

func ReceiveAgreeRegret(req *idtcp.Request) error {
	var (
		room = req.DistPayloads[mdwroom.Key].(*mdwroom.DistributePayload).Room
		user = req.ConnPayloads[mdwuser.Key].(*mdwuser.ConnectionPayload).User
	)

	s := utils.MakeSerializer(req.Data)
	agree, err := s.ReadBoolean()
	if err != nil {
		return err
	}

	opp, err := room.Opponent(user)
	if err != nil {
		return err
	}
	sendBoolean(opp.(*mdwuser.User).Conn, C_AgreeRegret, agree)

	for i := room.Users.Front(); i != room.Users.Back(); i = i.Next() {
		user := i.Value.(*mdwuser.User)
		if agree {
			err = SendMessage(user.Conn, mdwuser.ServerUsername, "agree")
		} else {
			err = SendMessage(user.Conn, mdwuser.ServerUsername, "disagree")
		}
		if err != nil {
			// todo
			continue
		}
	}
	return nil
}

func ReceiveMessage(req *idtcp.Request) error {
	var (
		room   = req.DistPayloads[mdwroom.Key].(*mdwroom.DistributePayload).Room
		sender = req.DistPayloads[mdwuser.Key].(*mdwuser.ConnectionPayload).User
	)

	s := utils.MakeSerializer(req.Data)
	text, err := s.ReadString16()
	if err != nil {
		return err
	}

	for i := room.Users.Front(); i != room.Users.Back(); i = i.Next() {
		user := i.Value.(*mdwuser.User)
		err := SendMessage(user.Conn, sender.Username, text)
		if err != nil {
			continue
		}
	}
	return nil

}
