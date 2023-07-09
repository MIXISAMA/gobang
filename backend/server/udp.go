package server

import (
	"fmt"

	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
	"github.com/MIXISAMA/gobang/backend/udp"
	"github.com/MIXISAMA/gobang/backend/utils"
)

func (server *Server) UdpPipe(msg *udp.Message) error {

	sr := utils.MakeSerializer(msg.Data)

	version, err := sr.ReadString8()
	if err != nil {
		return err
	}

	if version != Version {
		return fmt.Errorf("inconsistent version %s", version)
	}

	var sw utils.Serializer

	sw.WriteString8(Version)
	sw.WriteString8(server.name)
	sw.WriteUint8_Int(len(server.room.Rooms))
	for i := range server.room.Rooms {
		room := server.room.Rooms[i]
		sw.WriteString8(room.Name)
		sw.WriteBoolean(room.IsPlaying)
		sw.WriteString8(room.BlackPlayer.(*mdwuser.User).GetUsername(""))
		sw.WriteString8(room.WhitePlayer.(*mdwuser.User).GetUsername(""))
		sw.WriteUint8_Int(room.Users.Len())
		sw.WriteUint8_Int(room.MaxUsers)
	}

	_, err = msg.Conn.WriteToUDP(sw.Raw, msg.Addr)
	return err

}
