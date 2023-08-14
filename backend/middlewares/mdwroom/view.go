package mdwroom

import (
	"errors"
	"log"

	"github.com/MIXISAMA/gobang/backend/game"
	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
	"github.com/MIXISAMA/gobang/backend/utils"
)

func (middleware *Middleware) receiveJoinRoom(
	conn *idtcp.Conn,
	data []byte,
	payloads []interface{},
) error {

	i_join_room := new(mdwuser.InstructionJoinRoom)
	err := utils.Unmarshal(data, i_join_room)
	if err != nil {
		return err
	}

	if int(i_join_room.RoomID) >= len(middleware.Rooms) {
		return errors.New("wrong room id")
	}
	room := middleware.Rooms[i_join_room.RoomID]

	user := payloads[mdwuser.Key].(*mdwuser.Payload).User

	if i_join_room.Role == 'P' {
		err = room.joinAsPlayer(user)
	} else if i_join_room.Role == 'O' {
		err = room.joinAsOnlooker(user)
	} else {
		return errors.New("wrong role")
	}
	if err != nil {
		return err
	}

	for i := room.Users.Front(); i != nil; i = i.Next() {
		otherUser := i.Value.(*mdwuser.User)
		if otherUser == user {
			continue
		}
		_ = middleware.sendOtherJoinRoom(otherUser.Conn, otherUser.Username, i_join_room.Role)
		// TODO
	}

	err = middleware.sendYouJoinRoom(conn, true, room)
	if err != nil {
		return err
	}

	payloads[Key].(*Payload).Room = room
	return nil
}

func (middleware *Middleware) sendYouJoinRoom(
	conn *idtcp.Conn,
	isSuccess bool,
	room *Room,
) error {
	if !isSuccess {
		var s utils.Serializer
		s.WriteBoolean(false)
		_, err := conn.Write(middleware.c_YouJoinRoom, s.Raw)
		return err
	}

	blackPlayer, _ := room.BlackPlayer.(*mdwuser.User)
	whitePlayer, _ := room.WhitePlayer.(*mdwuser.User)

	var onlookers []string
	for i := room.Users.Front(); i != nil; i = i.Next() {
		user := i.Value.(*mdwuser.User)
		if user == room.BlackPlayer || user == room.WhitePlayer {
			continue
		}
		onlookers = append(onlookers, user.Username)
	}

	c_you_join_room := InstructionClientYouJoinRoom{
		IsSuccess:     isSuccess,
		RoomName:      room.Name,
		MaxUsers:      uint8(room.MaxUsers),
		BlackUsername: blackPlayer.GetUsername(""),
		WhileUsername: whitePlayer.GetUsername(""),
		Onlookers:     onlookers,
		ReadyPlayer:   room.ReadyColor,
		IsPlaying:     room.IsPlaying,
		RegretPlayer:  room.RegretColor,
		TiePlayer:     room.TieColor,
		Records:       room.Chess.GetRecords(),
	}

	log.Printf("sending instruction youjoinroom: %+v", c_you_join_room)
	data, err := utils.Marshal(c_you_join_room)
	if err != nil {
		return err
	}

	_, err = conn.Write(middleware.c_YouJoinRoom, data)
	return err
}

func (middleware *Middleware) sendOtherJoinRoom(conn *idtcp.Conn, username string, role utils.Char) error {
	type OtherJoinRoom struct {
		Username string `len_bytes:"1"`
		Role     utils.Char
	}
	data, err := utils.Marshal(OtherJoinRoom{Username: username, Role: role})
	if err != nil {
		return err
	}
	_, err = conn.Write(middleware.c_OtherJoinRoom, data)
	return err
}

func (middleware *Middleware) receiveUserLeaveRoom(payloads []interface{}) error {
	room := payloads[Key].(*Payload).Room
	user := payloads[mdwuser.Key].(*mdwuser.Payload).User
	if color := room.PlayerColor(user); color != game.SPACE {

		err := room.leave(user)
		if err != nil {
			return err
		}
	}
	for i := room.Users.Front(); i != nil; i = i.Next() {
		user := i.Value.(*mdwuser.User)
		_, err := user.Conn.Write(middleware.c_UserLeaveRoom, make([]byte, 0))
		if err != nil {
			user.Conn.Close()
		}
	}
	return nil
}
