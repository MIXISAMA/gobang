package main

import (
	"flag"
	"fmt"
	"net"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/server"
)

func main() {

	var serverAddress string

	flag.StringVar(&serverAddress, "address", "127.0.0.1:9759", "server address")
	flag.Var(&server.RoomList, "roomname", "gobang room name")

	flag.Parse()

	serverTcpAddr, err := net.ResolveTCPAddr("tcp", serverAddress)
	if err != nil {
		panic(err.Error())
	}

	serverUdpAddr, err := net.ResolveUDPAddr("udp", serverAddress)
	if err != nil {
		panic(err.Error())
	}

	tcpListener, err := net.ListenTCP("tcp", serverTcpAddr)
	if err != nil {
		panic(err.Error())
	}
	defer tcpListener.Close()

	udpConn, err := net.ListenUDP("udp", serverUdpAddr)
	if err != nil {
		panic(err.Error())
	}
	defer udpConn.Close()

	go udpPipe(udpConn)

	for {

		tcpConn, err := tcpListener.AcceptTCP()
		if err != nil {
			continue
		}

		go idtcpPipe(idtcp.NewConn(tcpConn))

	}

}

func idtcpPipe(conn *idtcp.Conn) {

	remoteAddress := conn.TCPConn.RemoteAddr().String()
	fmt.Println("A client connected : " + remoteAddress)
	defer func() {
		fmt.Println("disconnected :" + remoteAddress)
		conn.TCPConn.Close()
	}()

	for {

		msg, err := conn.ReadMessage()

		if err != nil {
			fmt.Println(err.Error())
			return
		}

		go messagePipe(msg)

	}
}

func messagePipe(msg *idtcp.Message) {

	err := server.Distribute(msg)

	if err != nil {
		fmt.Println(err.Error())
	}

}

func udpPipe(conn *net.UDPConn) {

	buffer := make([]byte, 65536)

	for {

		n, remoteAddr, err := conn.ReadFromUDP(buffer)

		data := buffer[:n]

		if err != nil {
			fmt.Println(err.Error())
			continue
		}

		go server.SendAllRoom(data, conn, remoteAddr)

	}

}
