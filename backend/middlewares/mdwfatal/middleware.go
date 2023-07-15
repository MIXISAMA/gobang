package mdwfatal

import (
	"container/list"
	"fmt"
	"log"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/utils"
)

type Middleware struct {
	s_FatalError uint16
	c_FatalError uint16
}

func NewMiddleware(
	s_FatalError uint16,
	c_FatalError uint16,
) *Middleware {
	return &Middleware{
		s_FatalError: s_FatalError,
		c_FatalError: c_FatalError,
	}
}

var Key = new(idtcp.MiddlewareKey)

type Execution struct {
	conn    *idtcp.Conn
	errorId *int32
	message string
}

func NewExecution(
	conn *idtcp.Conn,
	errorId *int32,
	message string,
) *Execution {
	return &Execution{
		conn:    conn,
		errorId: errorId,
		message: message,
	}
}

func (execution *Execution) Error() string {
	return fmt.Sprintf("Fatal ID %d: %s", execution.errorId, execution.message)
}

type Payload struct {
	Executions *list.List
}

func (middleware *Middleware) ProcessConnect(
	payloads idtcp.PayloadMap,
	processConnect func(idtcp.PayloadMap) (*idtcp.Conn, error),
) (*idtcp.Conn, error) {
	payload := &Payload{Executions: list.New()}
	payloads[Key] = payload
	conn, err := processConnect(payloads)
	switch e := err.(type) {
	case *Execution:
		middleware.processExecution(e)
	}
	middleware.processExecutions(payload.Executions)
	return conn, err
}

func (middleware *Middleware) ProcessDisconnect(
	conn *idtcp.Conn,
	payloads idtcp.PayloadMap,
	processDisconnect func(*idtcp.Conn, idtcp.PayloadMap),
) {

}

func (middleware *Middleware) ProcessDistribute(
	request *idtcp.Request,
	processDistribute func(*idtcp.Request) error,
) error {
	if request.Instruction == middleware.s_FatalError {
		receiveFatalError(request.Conn, request.Data)
	}
	err := processDistribute(request)
	switch e := err.(type) {
	case *Execution:
		middleware.processExecution(e)
	}
	middleware.processExecutions(request.Payloads[Key].(Payload).Executions)
	return err
}

func (middleware *Middleware) processExecution(e *Execution) {
	defer e.conn.Close()
	var s utils.Serializer
	err := s.WriteString8(e.message)
	if err != nil {
		log.Print(err.Error())
		return
	}
	e.conn.Write(middleware.c_FatalError, s.Raw)
}

func (middleware *Middleware) processExecutions(executions *list.List) {
	for i := executions.Front(); i != executions.Back(); i = i.Next() {
		execution := i.Value.(*Execution)
		middleware.processExecution(execution)
	}
}

func receiveFatalError(conn *idtcp.Conn, data []byte) {
	defer conn.Close()
	s := utils.MakeSerializer(data)
	errorId, err := s.ReadInt32()
	if err != nil {
		return
	}
	message, err := s.ReadString8()
	if err != nil {
		return
	}
	log.Println(NewExecution(conn, &errorId, message).Error())
}
