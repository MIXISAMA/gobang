package idtcp

import (
	"fmt"
	"log"
	"net"
	"os"
	"sync"
)

type Middleware interface {
	ProcessConnection(ConnectionPayloadMap, func(ConnectionPayloadMap) (*Conn, error)) (*Conn, error)
	ProcessDistribution(*Request, func(*Request) error) error
}

type MiddlewareKey struct{}

type MiddlewarePayload interface{}
type ConnectionPayloadMap map[*MiddlewareKey]MiddlewarePayload
type DistributePayloadMap map[*MiddlewareKey]MiddlewarePayload

type Request struct {
	Instruction  uint16
	Data         []byte
	Conn         *Conn
	ConnPayloads ConnectionPayloadMap
	DistPayloads DistributePayloadMap
}

type Server struct {
	listener       *net.TCPListener
	distributeList []func(request *Request) error
	middlewareList []Middleware
	connectPipe    func(connPayload ConnectionPayloadMap) (*Conn, error)
	distributePipe func(request *Request) error
	connectionSet  sync.Map
	listenerMutex  sync.Mutex
	connWaitGroup  sync.WaitGroup
	quitChannel    chan os.Signal
}

func NewServer(
	address string,
	distributeList []func(request *Request) error,
	middlewareList []Middleware,
) *Server {

	serverTcpAddr, err := net.ResolveTCPAddr("tcp", address)
	if err != nil {
		panic(err.Error())
	}

	tcpListener, err := net.ListenTCP("tcp", serverTcpAddr)
	if err != nil {
		panic(err.Error())
	}

	var server = Server{
		listener:       tcpListener,
		distributeList: distributeList,
		middlewareList: middlewareList,
		quitChannel:    make(chan os.Signal, 1),
	}

	server.connectPipe = func(connPayloads ConnectionPayloadMap) (*Conn, error) {

		tcpConn, err := server.listener.AcceptTCP()
		conn := &Conn{TCPConn: tcpConn}
		if err != nil {
			return conn, err
		}

		remoteAddress := tcpConn.RemoteAddr().String()
		log.Println("A client connected : " + remoteAddress)

		return conn, nil
	}

	for i := len(middlewareList) - 1; i >= 0; i-- {
		server.connectPipe = func(connPayload ConnectionPayloadMap) (*Conn, error) {
			return middlewareList[i].ProcessConnection(connPayload, server.connectPipe)
		}
	}

	server.distributePipe = func(req *Request) error {
		if int(req.Instruction) >= len(server.distributeList) {
			return fmt.Errorf("wrong instruction: %d", req.Instruction)
		}

		log.Printf("Received Instruction: %d\n", req.Instruction)
		err := server.distributeList[req.Instruction](req)
		return err
	}

	for i := len(middlewareList) - 1; i >= 0; i-- {
		server.distributePipe = func(req *Request) error {
			return middlewareList[i].ProcessDistribution(req, server.distributePipe)
		}
	}

	return &server
}

func (server *Server) Run() {
	defer server.connWaitGroup.Wait()

	server.connWaitGroup.Add(1)
	go server.listenInterrupt()

	for {
		connPayloads := make(ConnectionPayloadMap)
		server.listenerMutex.Lock()
		conn, err := server.connectPipe(connPayloads)
		if err != nil {
			log.Println(err.Error())
			if err == net.ErrClosed {
				server.listenerMutex.Unlock()
				return
			}
			server.listenerMutex.Unlock()
			continue
		}
		server.connectionSet.Store(conn, struct{}{})
		server.connWaitGroup.Add(1)
		go server.requestPipe(conn, connPayloads)
		server.listenerMutex.Unlock()
	}

}

func (server *Server) requestPipe(conn *Conn, connPayloads ConnectionPayloadMap) {
	defer func() {
		conn.Close()
		server.connectionSet.Delete(conn)
		server.connWaitGroup.Done()
	}()

	for {
		var req = Request{
			Conn:         conn,
			ConnPayloads: connPayloads,
			DistPayloads: make(DistributePayloadMap),
		}
		_, err := conn.Read(&req.Instruction, &req.Data)
		if err != nil {
			log.Println(err.Error())
			return
		}

		server.distributePipe(&req)
	}
}

func (server *Server) listenInterrupt() {
	defer server.connWaitGroup.Done()
	<-server.quitChannel
	log.Println("quit")

	server.listener.Close()

	server.listenerMutex.Lock()
	server.connectionSet.Range(func(conn, _ interface{}) bool {
		conn.(*Conn).Close()
		return true
	})
	server.listenerMutex.Unlock()

}
