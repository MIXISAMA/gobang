package main

import (
	"flag"
	"fmt"
	"net"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/server"
)

var serverAddress = flag.String("address", "127.0.0.1:9759", "server address")

func main() {

	var err error
	var serverAddr *net.TCPAddr
	var tcpListener *net.TCPListener

	flag.Parse()
	server.OnReady()

	serverAddr, err = net.ResolveTCPAddr("tcp", *serverAddress)
	if err != nil {
		panic(err.Error())
	}

	tcpListener, err = net.ListenTCP("tcp", serverAddr)
	if err != nil {
		panic(err.Error())
	}
	defer tcpListener.Close()

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
