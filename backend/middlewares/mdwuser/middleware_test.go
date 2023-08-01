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

func newRequest(instruction uint16, data []byte) (*idtcp.Request, *net.TCPConn) {
	var server *net.TCPConn
	go func() {
		serverTcpAddr, _ := net.ResolveTCPAddr("tcp", ":28080")
		ln, _ := net.ListenTCP("tcp", serverTcpAddr)
		for {
			server, _ = ln.AcceptTCP()
		}
	}()

	client, _ := net.Dial("tcp", "localhost:28080")
	conn := idtcp.Conn{TCPConn: client.(*net.TCPConn)}

	payloads := make(idtcp.PayloadMap)
	payloads[&Key] = &Payload{
		User: nil,
	}

	request := idtcp.Request{
		Instruction: instruction,
		Data:        data,
		Conn:        &conn,
		Payloads:    payloads,
	}
	return &request, server
}

func TestAuthentication(t *testing.T) {
	assert := assert.New(t)

	m := newMiddleware(t)

	i_join_room := InstructionJoinRoom{
		RoomID:   0,
		Username: "foo",
		Password: []byte("bar"),
		Role:     'P',
	}
	data, err := utils.Marshal(i_join_room)
	assert.Nil(err)
	request, _ := newRequest(S_JoinRoom, data)

	m.ProcessDistribute(request, func(*idtcp.Request) error { return nil })

	assert.NotNil(request.Payloads[&Key].(*Payload).User)
	assert.Equal("foo", request.Payloads[&Key].(*Payload).User.Username)
}
