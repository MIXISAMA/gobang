package mdwuser

import (
	"errors"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/utils"
)

func (middleware *Middleware) sendAuthenticationFailed(conn *idtcp.Conn, instruction uint16) error {
	var s utils.Serializer
	s.WriteBoolean(false)
	_, err := conn.Write(instruction, s.Raw)
	return err
}

func (middleware *Middleware) authenticate(request *idtcp.Request) (*User, error) {
	i_join_room := new(InstructionJoinRoom)
	utils.Unmarshal(request.Data, i_join_room)

	err := middleware.database.authenticate(i_join_room.Username, i_join_room.Password)
	if errors.Is(err, ErrUserNotFound) {
		err = middleware.database.register(i_join_room.Username, i_join_room.Password)
		if err != nil {
			return nil, err
		}
	} else if err != nil {
		return nil, err
	}

	return middleware.database.getUser(request.Conn, i_join_room.Username)
}
