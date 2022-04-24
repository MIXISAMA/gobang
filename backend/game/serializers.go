package game

import (
	"errors"

	"github.com/MIXISAMA/gobang/backend/server"
)

func DecodeRoomName(data []byte) (*Room, string, error) {

	s := server.MakeSerializer(data)

	roomId, err := s.ReadUint16()
	if err != nil {
		return nil, "", errors.New("wrong room id")
	}
	room := Gd.Rooms[roomId]
	name, err := s.ReadString()
	return room, name, err

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
