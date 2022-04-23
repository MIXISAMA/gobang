package game

import (
	"github.com/MIXISAMA/gobang/backend/server"
)

func SerializerUint16String(data []byte) (v1 uint16, v2 string, err error) {

	s := server.MakeSerializer(data)

	v1, err = s.ReadUint16()
	if err != nil {
		return
	}

	v2, err = s.ReadString()
	return

}
