package server

import (
	"fmt"
	"log"

	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
	"github.com/MIXISAMA/gobang/backend/udp"
	"github.com/MIXISAMA/gobang/backend/utils"
)

func (server *Server) UdpPipe(msg *udp.Message) error {
	log.Printf("%-21s UDP Received", msg.Addr)
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
		blackPlayer, _ := room.BlackPlayer.(*mdwuser.User)
		whitePlayer, _ := room.WhitePlayer.(*mdwuser.User)
		sw.WriteString8(room.Name)
		sw.WriteBoolean(room.IsPlaying)
		sw.WriteString8(blackPlayer.GetUsername(""))
		sw.WriteString8(whitePlayer.GetUsername(""))
		sw.WriteUint8_Int(room.Users.Len())
		sw.WriteUint8_Int(room.MaxUsers)
	}

	_, err = msg.Conn.WriteToUDP(sw.Raw, msg.Addr)
	log.Printf("%-21s UDP Sent", msg.Addr)
	return err

}
