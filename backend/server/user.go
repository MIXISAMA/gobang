package server

import (
	"errors"

	"github.com/MIXISAMA/gobang/backend/idtcp"
)

type User struct {
	Anonymous bool
	Name      string
	Conn      *idtcp.Conn
}

func NewAnonymousUser(conn *idtcp.Conn) *User {
	return &User{
		Anonymous: true,
		Name:      "Anonymous User",
		Conn:      conn,
	}
}

func (user *User) Rename(name string) error {

	// name = strings.Trim(name, " ")
	if len(name) == 0 {
		return errors.New("name can not be blank")
	}
	if len(name) > 64 {
		return errors.New("name is too long")
	}

	user.Name = name
	user.Anonymous = false

	return nil
}
