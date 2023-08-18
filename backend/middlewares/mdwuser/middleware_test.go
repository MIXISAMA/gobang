package mdwuser

import (
	"net"
	"os"
	"testing"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/utils"
	"github.com/stretchr/testify/assert"
)

const (
	S_JoinRoom    uint16 = 1
	C_YouJoinRoom uint16 = 2
)

func TestHasher(t *testing.T) {
	password := []byte("foo")
	hashed_password := hasher_encode(password, []byte(utils.RandomString(16)))
	assert.True(t, hasher_verify(password, hashed_password))
	assert.False(t, hasher_verify([]byte("bar"), hashed_password))
}

func newMiddleware(t *testing.T) *Middleware {
	databasePath := "test.db"
	NewDatabase(databasePath)
	t.Cleanup(func() { os.Remove("test.db") })

	userMiddleware, _ := NewMiddleware(
		S_JoinRoom,
		C_YouJoinRoom,
		databasePath,
		"cee330f8-303b-11ee-be56-000000000000",
	)
	return userMiddleware
}

func newTestClient() *idtcp.Conn {

	serverTcpAddr, _ := net.ResolveTCPAddr("tcp", ":28080")
	ln, _ := net.ListenTCP("tcp", serverTcpAddr)
	go func() {
		conn, _ := ln.AcceptTCP()
		buffer := make([]byte, 1024)
		for {
			_, err := conn.Read(buffer)
			if err != nil {
				break
			}
		}
		conn.Close()
		ln.Close()
	}()

	client, _ := net.Dial("tcp", "localhost:28080")
	conn := idtcp.Conn{TCPConn: client.(*net.TCPConn)}

	return &conn
}

func TestAuthentication(t *testing.T) {
	assert := assert.New(t)

	conn := newTestClient()
	defer conn.Close()

	m := newMiddleware(t)
	m.SetMdwKey(0)

	i_join_room := InstructionJoinRoom{
		RoomID:   0,
		Username: "foo",
		Password: []byte("bar"),
		Role:     'P',
	}
	data, err := utils.Marshal(i_join_room)
	assert.Nil(err)

	ctx := idtcp.NewDistributeContext(
		func(*idtcp.Request) error { return nil },
		[]func(*idtcp.DistributeContext) error{m.ProcessDistribute},
		[]interface{}{m.NewPayload()},
		conn,
		S_JoinRoom,
		data,
	)
	ctx.Next()

	assert.NotNil(ctx.Payloads[Key].(*Payload).User)
	assert.Equal("foo", ctx.Payloads[Key].(*Payload).User.Username)
}
