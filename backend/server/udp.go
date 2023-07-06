package server

import (
	"fmt"

	"github.com/MIXISAMA/gobang/backend/udp"
	"github.com/MIXISAMA/gobang/backend/utils"
)

func (server *Server) UdpPipe(msg *udp.Message) error {

	s := utils.MakeSerializer(msg.Data)

	version, err := s.ReadString8()
	if err != nil {
		return err
	}

	if version != Version {
		return fmt.Errorf("inconsistent version %s", version)
	}

	data, err := encodeRooms()
	if err != nil {
		return err
	}

	_, err = msg.Conn.WriteToUDP(data, msg.Addr)
	return err

}

func encodeRooms() ([]byte, error) {

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
