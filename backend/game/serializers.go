package game

import (
	"errors"

	"github.com/MIXISAMA/gobang/backend/server"
)

func DecodeString(data []byte) (string, error) {

	s := server.MakeSerializer(data)
	return s.ReadString()

}

func DecodeJoinRoom(data []byte) (*Room, string, bool, error) {

	s := server.MakeSerializer(data)

	roomId, err := s.ReadUint16()
	if err != nil {
		return nil, "", false, errors.New("wrong room id")
	}
	room := Gd.Rooms[roomId]

	name, err := s.ReadString()
	if err != nil {
		return nil, "", false, err
	}

	is_player, err := s.ReadBoolean()
	if err != nil {
		return nil, "", false, err
	}

	return room, name, is_player, err

}

func EncodeAllRoomInformation(room *Room) ([]byte, error) {

	var s server.Serializer

	err := s.WriteString(room.Name)
	if err != nil {
		return nil, err
	}

	err = s.WriteUint16_Int(room.MaxUsers)
	if err != nil {
		return nil, err
	}

	err = s.WriteUint16_Int(len(room.Users))
	if err != nil {
		return nil, err
	}

	for i := range room.Users {
		err = s.WriteString(room.Users[i].Name)
		if err != nil {
			return nil, err
		}
	}

	character := append(room.Players[:], room.Repentant, room.Ready)
	for i := range character {
		if character[i] == nil {
			s.WriteBoolean(false)
			continue
		}
		s.WriteBoolean(true)
		err = s.WriteString(character[i].Name)
		if err != nil {
			return nil, err
		}
	}

	s.WriteBoolean(room.IsPlaying)

	for i := range room.Chess.Board {
		for j := range room.Chess.Board[i] {
			s.WriteByte(room.Chess.Board[i][j])
		}
	}

	err = s.WriteUint16_Int(len(room.Chess.Record))
	if err != nil {
		return nil, err
	}

	for i := range room.Chess.Record {
		s.WriteByte(room.Chess.Record[i])
	}

	return s.Raw, nil
}

func EncodeUserInRoom(room *Room, user *server.User) ([]byte, error) {

	var s server.Serializer

	err := s.WriteString(user.Name)
	if err != nil {
		return nil, err
	}

	color, err := room.LocatePlayer(user)
	if err != nil {
		s.WriteBoolean(false)
	} else {
		s.WriteBoolean(true)
		s.WriteByte(byte(color))
	}

	return s.Raw, nil
}

func EncodeString(str string) ([]byte, error) {

	var s server.Serializer
	err := s.WriteString(str)
	return s.Raw, err

}

func EncodeStringString(str1 string, str2 string) ([]byte, error) {

	var s server.Serializer
	err := s.WriteString(str1)
	if err != nil {
		return nil, err
	}
	err = s.WriteString(str2)
	if err != nil {
		return nil, err
	}
	return s.Raw, nil

}

func EncodeRooms() ([]byte, error) {

	var s server.Serializer

	err := s.WriteUint16_Int(len(Gd.Rooms))
	if err != nil {
		return nil, err
	}

	for i := range Gd.Rooms {

		room := Gd.Rooms[i]

		err = s.WriteUint16_Int(i)
		if err != nil {
			return nil, err
		}

		err = s.WriteString(room.Name)
		if err != nil {
			return nil, err
		}

		s.WriteBoolean(room.IsPlaying)

		for j := range room.Players {
			player := room.Players[j]
			if player != nil {
				s.WriteBoolean(true)
				err = s.WriteString(player.Name)
				if err != nil {
					return nil, err
				}
			} else {
				s.WriteBoolean(false)
			}
		}

		err = s.WriteUint16_Int(len(room.Users))
		if err != nil {
			return nil, err
		}

		err = s.WriteUint16_Int(room.MaxUsers)
		if err != nil {
			return nil, err
		}

	}

	return s.Raw, nil

}
