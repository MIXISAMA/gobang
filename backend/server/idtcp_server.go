package server

import (
	"log"
	"net"

	"github.com/MIXISAMA/gobang/backend/idtcp"
)

type IdtcpMessage struct {
	Instruction uint16
	Data        []byte
	User        *User
}

type IdtcpServer struct {
	Listener   *net.TCPListener
	Distribute []func(*IdtcpMessage) error
	Disconnect func(*User)
}

func NewIdtcpServer(
	address string,
	distribute []func(*IdtcpMessage) error,
	disconnect func(*User),
) *IdtcpServer {

	serverTcpAddr, err := net.ResolveTCPAddr("tcp", address)
	if err != nil {
		panic(err.Error())
	}

	tcpListener, err := net.ListenTCP("tcp", serverTcpAddr)
	if err != nil {
		panic(err.Error())
	}

	return &IdtcpServer{
		Listener:   tcpListener,
		Distribute: distribute,
		Disconnect: disconnect,
	}
}

func (server *IdtcpServer) Run() {
	defer server.Listener.Close()
	for {
		tcpConn, err := server.Listener.AcceptTCP()
		if err != nil {
			log.Println(err.Error())
			return
		}
		go server.ConnPipe(idtcp.NewConn(tcpConn))
	}
}

func (server *IdtcpServer) ConnPipe(conn *idtcp.Conn) {

	user := NewAnonymousUser(conn)

	remoteAddress := conn.TCPConn.RemoteAddr().String()
	log.Println("A client connected : " + remoteAddress)
	defer func() {
		log.Println("disconnected :" + remoteAddress)
		server.Disconnect(user)
		conn.TCPConn.Close()
	}()

	for {

		msg := IdtcpMessage{User: user}

		_, err := conn.Read(&msg.Instruction, &msg.Data)
		if err != nil {
			log.Println(err.Error())
			return
		}

		if int(msg.Instruction) >= len(server.Distribute) {
			log.Printf("Instruction %d wrong", msg.Instruction)
			return
		}

		log.Printf("Received User[%s]'s Instruction: %d", user.Name, msg.Instruction)
		err = server.Distribute[msg.Instruction](&msg)
		if err != nil {
			log.Println(err.Error())
			continue
		}

	}
}
