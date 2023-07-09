package mdwuser

import (
	"time"

	"github.com/MIXISAMA/gobang/backend/idtcp"
)

const ServerUsername = "Server"

type User struct {
	Username     string
	NumOfWins    uint32
	NumOfTies    uint32
	NumOfLosses  uint32
	NumOfMatches uint32
	GameDuration uint64
	Conn         *idtcp.Conn
	gameStamp    time.Time
}

func NewUser(username string, conn *idtcp.Conn) *User {
	return &User{
		Username:  username,
		Conn:      conn,
		gameStamp: time.Now(),
	}
}

func (user *User) GetUsername(defaultUsername string) string {
	if user == nil {
		return defaultUsername
	}
	return user.Username
}
