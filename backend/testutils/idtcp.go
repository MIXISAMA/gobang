package testutils

import (
	"net"
	"sync"
	"testing"
	"time"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/stretchr/testify/assert"
)

type TestClient struct {
	Conn *idtcp.Conn
	ch   chan idMessage
	wg   *sync.WaitGroup
	t    *testing.T
}

type idMessage struct {
	instruction uint16
	data        []byte
}

func NewTestClient(t *testing.T) *TestClient {
	assert := assert.New(t)

	ln, err := net.Listen("tcp", ":0")
	assert.Nil(err)

	var wg sync.WaitGroup
	wg.Add(1)
	ch := make(chan idMessage, 1)
	go func() {
		defer wg.Done()
		defer ln.Close()
		conn, err := ln.Accept()
		assert.Nil(err)
		defer conn.Close()
		idtcpConn := idtcp.Conn{TCPConn: conn.(*net.TCPConn)}
		for {
			var msg idMessage
			_, err := idtcpConn.Read(&msg.instruction, &msg.data)
			if err != nil {
				return
			}
			ch <- msg
		}
	}()

	conn, _ := net.Dial("tcp", ln.Addr().String())
	return &TestClient{
		Conn: &idtcp.Conn{TCPConn: conn.(*net.TCPConn)},
		ch:   ch,
		wg:   &wg,
		t:    t,
	}
}

func (client *TestClient) Read(timeout time.Duration) (uint16, []byte) {
	timeoutCh := time.After(timeout)
	select {
	case msg := <-client.ch:
		return msg.instruction, msg.data
	case <-timeoutCh:
		client.t.Fatalf("timeout %ds", timeout.Milliseconds())
	}
	panic("impossible to get here")
}

func (client *TestClient) Close() {
	client.Conn.Close()
	select {
	case <-client.ch:
	default:
	}
	client.wg.Wait()
}
