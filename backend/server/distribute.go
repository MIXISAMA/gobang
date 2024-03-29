package server

import (
	"errors"
	"fmt"
	"log"

	"github.com/MIXISAMA/gobang/backend/game"
	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwroom"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
	"github.com/MIXISAMA/gobang/backend/utils"
)

const (
	S_FatalError   uint16 = 0
	S_JoinRoom     uint16 = 1
	S_UserInfo     uint16 = 2
	S_LeaveRoom    uint16 = 3
	S_Ready        uint16 = 4
	S_PlayerStone  uint16 = 5
	S_PlayerRegret uint16 = 6
	S_ReplyRegret  uint16 = 7
	S_PlayerTie    uint16 = 8
	S_ReplyTie     uint16 = 9
	S_GiveUp       uint16 = 10
	S_Message      uint16 = 11
)

func Empty(*idtcp.Request) error {
	return nil
}

var Endpoints = []func(*idtcp.Request) error{
	S_FatalError: Empty,
	// we do nothing here, as user will be processed by mdwuser
	S_JoinRoom: Empty,
	S_UserInfo: ReceiveUserInfo,
	// we do nothing here, as user will be processed by mdwuser when disconnecting
	S_LeaveRoom:    Empty,
	S_Ready:        ReceiveReady,
	S_PlayerStone:  ReceivePlayerStone,
	S_PlayerRegret: ReceivePlayerRegret,
	S_ReplyRegret:  Empty,
	S_PlayerTie:    Empty,
	S_ReplyTie:     Empty,
	S_GiveUp:       Empty,
	S_Message:      ReceiveMessage,
}

var (
	ErrGameNotStart = errors.New("game not start")
)

func ReceiveUserInfo(req *idtcp.Request) error {
	room := req.Payloads[mdwroom.Key].(*mdwroom.Payload).Room
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

type IGameStart struct {
	BlackPlayerName string `len_bytes:"1"`
}

func ReceiveReady(req *idtcp.Request) error {
	user := req.Payloads[mdwuser.Key].(*mdwuser.Payload).User
	room := req.Payloads[mdwroom.Key].(*mdwroom.Payload).Room

	color, ready, err := room.Ready(user)
	if err != nil {
		return err
	}
	if ready {
		i := IGameStart{room.BlackPlayer.(*mdwuser.User).Username}
		data, err := utils.Marshal(i)
		if err != nil {
			return err
		}

		broadcastMessage(room, C_GameStart, data)
		room.Room.Start()
	} else {
		broadcastMessage(room, C_PlayerReady, []byte{color})
	}
	return nil
}

func ReceiveLeaveRoom(req *idtcp.Request) error {
	var (
		room = req.Payloads[mdwroom.Key].(*mdwroom.Payload).Room
		user = req.Payloads[mdwuser.Key].(*mdwuser.Payload).User
	)
	if color := room.PlayerColor(user); color != game.SPACE && room.IsPlaying {
		return SendGameOver(req.Conn, color^0x01)
	}
	return nil
}

func ReceivePlayerStone(req *idtcp.Request) error {
	room := req.Payloads[mdwroom.Key].(*mdwroom.Payload).Room
	user := req.Payloads[mdwuser.Key].(*mdwuser.Payload).User

	s := utils.MakeSerializer(req.Data)
	move, err := s.ReadUint8()
	if err != nil {
		return err
	}
	coor, err := s.ReadByte()
	if err != nil {
		return err
	}

	if !room.Room.IsPlaying {
		return ErrGameNotStart
	}

	who := room.Chess.WhoseTurn()
	if user == room.BlackPlayer && who == game.BLACK {
		return errors.New("not black's turn")
	}
	if user == room.WhitePlayer && who == game.WHITE {
		return errors.New("not white's turn")
	}
	if user != room.BlackPlayer && user != room.WhitePlayer {
		return fmt.Errorf("user %v is not player, can't stone", user.Username)
	}

	err = room.Chess.Stone(int(move), coor)
	if err != nil {
		return nil
	}

	win := room.Chess.DidYouWin()

	for i := room.Users.Front(); i != nil; i = i.Next() {
		user := i.Value.(*mdwuser.User)
		log.Printf("sending to user %v", user.Username)
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
	var (
		room = req.Payloads[mdwroom.Key].(*mdwroom.Payload).Room
		user = req.Payloads[mdwuser.Key].(*mdwuser.Payload).User
	)
	err := room.Regret(user)
	if err != nil {
		return err
	}
	for i := room.Users.Front(); i != nil; i = i.Next() {
		user := i.Value.(*mdwuser.User)
		err = sendByte(user.Conn, C_PlayerRegret, room.RegretColor)
		if err != nil {
			user.Conn.Close()
			continue
		}
		err = SendMessage(user.Conn, mdwuser.ServerUsername, "regret")
		if err != nil {
			user.Conn.Close()
			continue
		}
	}
	return nil
}

func ReceiveReplyRegret(req *idtcp.Request) error {
	var (
		room = req.Payloads[mdwroom.Key].(*mdwroom.Payload).Room
		user = req.Payloads[mdwuser.Key].(*mdwuser.Payload).User
	)

	s := utils.MakeSerializer(req.Data)
	agree, err := s.ReadBoolean()
	if err != nil {
		return err
	}

	err = room.ReplyRegret(user, agree)
	if err != nil {
		return err
	}

	opp, err := room.Opponent(user)
	if err != nil {
		return err
	}
	sendBoolean(opp.(*mdwuser.User).Conn, C_AgreeRegret, agree)

	for i := room.Users.Front(); i != nil; i = i.Next() {
		user := i.Value.(*mdwuser.User)
		if agree {
			err = SendMessage(user.Conn, mdwuser.ServerUsername, "agree")
		} else {
			err = SendMessage(user.Conn, mdwuser.ServerUsername, "disagree")
		}
		if err != nil {
			user.Conn.Close()
		}
	}
	return nil
}

func ReceiveMessage(req *idtcp.Request) error {
	var (
		room   = req.Payloads[mdwroom.Key].(*mdwroom.Payload).Room
		sender = req.Payloads[mdwuser.Key].(*mdwuser.Payload).User
	)

	m := new(utils.C_Message)
	err := utils.Unmarshal(req.Data, m)
	if err != nil {
		return err
	}

	for i := room.Users.Front(); i != nil; i = i.Next() {
		user := i.Value.(*mdwuser.User)
		err := SendMessage(user.Conn, sender.Username, m.Message)
		if err != nil {
			continue
		}
	}
	return nil

}
