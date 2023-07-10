package mdwroom

import (
	"errors"

	"github.com/MIXISAMA/gobang/backend/game"
	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
	"github.com/MIXISAMA/gobang/backend/utils"
)

func (middleware *Middleware) receiveJoinRoom(req *idtcp.Request) error {

	s := utils.MakeSerializer(req.Data)

	roomIdU8, err := s.ReadUint8()
	if err != nil {
		return err
	}
	roomId := int(roomIdU8)
	if roomId >= len(middleware.Rooms) {
		return errors.New("wrong room id")
	}
	room := middleware.Rooms[roomId]

	_, err = s.ReadBytes8()
	if err != nil {
		return err
	}

	user := req.ConnPayloads[mdwuser.Key].(*mdwuser.ConnPayload).User

	role, err := s.ReadByte()
	if err != nil {
		return err
	}
	if role == 'P' {
		err = room.joinAsPlayer(user)
	} else if role == 'O' {
		err = room.joinAsOnlooker(user)
	} else {
		return errors.New("wrong role")
	}
	if err != nil {
		return err
	}

	for i := room.Users.Front(); i != room.Users.Back(); i = i.Next() {
		otherUser := i.Value.(*mdwuser.User)
		if otherUser == user {
			continue
		}
		_ = middleware.sendOtherJoinRoom(otherUser.Conn, otherUser.Username)
		// TODO
	}

	err = middleware.sendYouJoinRoom(req.Conn, true, room)
	if err != nil {
		return err
	}

	req.ConnPayloads[Key].(*ConnPayload).Room = room
	return nil
}

func (middleware *Middleware) sendYouJoinRoom(
	conn *idtcp.Conn,
	isSuccess bool,
	room *Room,
) error {

	var s utils.Serializer

	s.WriteBoolean(isSuccess)

	if !isSuccess {
		_, err := conn.Write(middleware.c_YouJoinRoom, s.Raw)
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

	_, err := conn.Write(middleware.c_YouJoinRoom, s.Raw)
	return err
}

func (middleware *Middleware) sendOtherJoinRoom(conn *idtcp.Conn, username string) error {
	var s utils.Serializer
	s.WriteString8(username)
	_, err := conn.Write(middleware.c_OtherJoinRoom, s.Raw)
	return err
}

func (middleware *Middleware) receiveUserLeaveRoom(req *idtcp.Request) error {
	room := req.ConnPayloads[Key].(*ConnPayload).Room
	user := req.ConnPayloads[mdwuser.Key].(*mdwuser.ConnPayload).User
	if color := room.PlayerColor(user); color != game.SPACE {
		if room.IsPlaying {
			middleware.sendGameOver(req.Conn, color^0x01)

		}
		err := room.leaveAsPlayer(user)
		if err != nil {
			return err
		}
	}
	for i := room.Users.Front(); i != room.Users.Back(); i = i.Next() {
		user := i.Value.(*mdwuser.User)
		_, err := user.Conn.Write(middleware.c_UserLeaveRoom, make([]byte, 0))
		if err != nil {
			user.Conn.Close()
		}
	}
	return nil
}

func (middleware *Middleware) sendGameOver(conn *idtcp.Conn, color byte) error {
	var s utils.Serializer
	s.WriteByte(color)
	_, err := conn.Write(middleware.c_GameOver, s.Raw)
	return err
}
