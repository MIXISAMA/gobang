package udp

import (
	"log"
	"net"
)

type Message struct {
	Data []byte
	Conn *net.UDPConn
	Addr *net.UDPAddr
}

type Server struct {
	Conn *net.UDPConn
	Pipe func(*Message) error
}

func NewServer(address string, pipe func(*Message) error) *Server {
	serverUdpAddr, err := net.ResolveUDPAddr("udp", address)
	if err != nil {
		panic(err.Error())
	}

	udpConn, err := net.ListenUDP("udp", serverUdpAddr)
	if err != nil {
		panic(err.Error())
	}

	return &Server{
		Conn: udpConn,
		Pipe: pipe,
	}
}

func (server *Server) Run() {
	defer server.Conn.Close()
	buffer := make([]byte, 65536)
	for {
		n, remoteAddr, err := server.Conn.ReadFromUDP(buffer)
		if err != nil {
			log.Println(err.Error())
			continue
		}
		msg := Message{
			Data: buffer[:n],
			Conn: server.Conn,
			Addr: remoteAddr,
		}
		go func() {
			err := server.Pipe(&msg)
			if err != nil {
				log.Println("UDP err:", err.Error())
			}
		}()
	}
}
