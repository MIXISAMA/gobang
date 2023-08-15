package idtcp

import (
	"fmt"
	"log"
	"net"
	"sync"
)

type Request struct {
	Instruction uint16
	Data        []byte
	Conn        *Conn
	Payloads    []interface{}
}

type Server struct {
	listener       *net.TCPListener
	distributeList []func(request *Request) error

	middlewareList    []Middleware
	mdwDistributeList []func(*DistributeContext) error
	mdwConnectList    []func(*ConnectContext) (*Conn, error)
	mdwDisconnectList []func(*DisconnectContext)

	connectionSet sync.Map
	listenerMutex sync.Mutex
	connWaitGroup sync.WaitGroup
	pipelineMutex sync.Mutex
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

	var mdwNum = len(middlewareList)

	var server = Server{
		listener:       tcpListener,
		distributeList: distributeList,

		middlewareList:    middlewareList,
		mdwDistributeList: make([]func(*DistributeContext) error, mdwNum),
		mdwConnectList:    make([]func(*ConnectContext) (*Conn, error), mdwNum),
		mdwDisconnectList: make([]func(*DisconnectContext), mdwNum),
	}

	for i := 0; i < mdwNum; i++ {
		middlewareList[i].SetMdwKey(i)
		server.mdwDistributeList[i] = middlewareList[i].ProcessDistribute
		server.mdwConnectList[i] = middlewareList[i].ProcessConnect
		server.mdwDisconnectList[i] = middlewareList[i].ProcessDisconnect
	}

	return &server
}

func (server *Server) Run() {
	defer server.connWaitGroup.Wait()

	log.Println("Server Started")

	for {
		payloads := make([]interface{}, len(server.middlewareList))
		for i := 0; i < len(server.middlewareList); i++ {
			payloads[i] = server.middlewareList[i].NewPayload()
		}
		server.listenerMutex.Lock()
		conn, err := NewConnectContext(
			server.connect,
			server.mdwConnectList,
			payloads,
		).Next()
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
		go server.requestPipe(conn, payloads)
		server.listenerMutex.Unlock()
	}

}

func (server *Server) requestPipe(conn *Conn, payloads []interface{}) {
	defer func() {
		server.connectionSet.Delete(conn)
		server.connWaitGroup.Done()
	}()

	for {
		var instruction uint16
		var data []byte
		_, err := conn.Read(&instruction, &data)
		log.Printf("received instruction %v", instruction)
		if err != nil {
			log.Printf("%-21s TCP ERR: %s", conn.RemoteAddr().String(), err.Error())
			break
		}

		server.pipelineMutex.Lock()
		err = NewDistributeContext(
			server.distribute,
			server.mdwDistributeList,
			payloads,
			conn,
			instruction,
			data,
		).Next()
		server.pipelineMutex.Unlock()
		if err != nil {
			log.Printf("%-21s TCP ERR: %s", conn.RemoteAddr().String(), err.Error())
			break
		}
	}

	server.pipelineMutex.Lock()
	NewDisconnectContext(
		server.disconnect,
		server.mdwDisconnectList,
		payloads,
		conn,
	).Next()
	server.pipelineMutex.Unlock()
}

func (server *Server) CloseAll() {
	server.listener.Close()

	server.listenerMutex.Lock()
	server.connectionSet.Range(func(conn, _ interface{}) bool {
		conn.(*Conn).Close()
		return true
	})
	server.listenerMutex.Unlock()
}

func (server *Server) connect() (*Conn, error) {
	tcpConn, err := server.listener.AcceptTCP()
	conn := &Conn{TCPConn: tcpConn}
	if err != nil {
		return conn, err
	}
	log.Printf("%-21s TCP Connected\n", conn.RemoteAddr().String())
	return conn, nil
}

func (server *Server) disconnect(conn *Conn) {
	conn.Close()
	log.Printf("%-21s TCP Disconnected\n", conn.RemoteAddr().String())
}

func (server *Server) distribute(req *Request) error {
	if int(req.Instruction) >= len(server.distributeList) {
		return fmt.Errorf("wrong instruction: %d", req.Instruction)
	}

	err := server.distributeList[req.Instruction](req)
	if err != nil {
		log.Println(err)
	}
	return err
}
