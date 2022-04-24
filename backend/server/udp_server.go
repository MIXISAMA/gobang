package server

import (
	"log"
	"net"
)

type UdpMessage struct {
	Data []byte
	Conn *net.UDPConn
	Addr *net.UDPAddr
}

type UdpServer struct {
	Conn *net.UDPConn
	Pipe func(*UdpMessage) error
}

func NewUdpServer(address string, pipe func(*UdpMessage) error) *UdpServer {
	serverUdpAddr, err := net.ResolveUDPAddr("udp", address)
	if err != nil {
		panic(err.Error())
	}

	udpConn, err := net.ListenUDP("udp", serverUdpAddr)
	if err != nil {
		panic(err.Error())
	}

	return &UdpServer{
		Conn: udpConn,
		Pipe: pipe,
	}
}

func (server *UdpServer) Run() {
	defer server.Conn.Close()
	buffer := make([]byte, 65536)
	for {
		n, remoteAddr, err := server.Conn.ReadFromUDP(buffer)
		if err != nil {
			log.Println(err.Error())
			continue
		}
		msg := UdpMessage{
			Data: buffer[:n],
			Conn: server.Conn,
			Addr: remoteAddr,
		}
		log.Println("Received UDP package")
		go func() {
			err := server.Pipe(&msg)
			if err != nil {
				log.Println("UDP err:", err.Error())
			}
		}()
	}
}
