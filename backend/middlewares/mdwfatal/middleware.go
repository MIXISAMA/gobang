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

var Key int

type Execution struct {
	conn    *idtcp.Conn
	errorId int32
	message string
}

func NewExecution(
	conn *idtcp.Conn,
	errorId int32,
	message string,
) *Execution {
	return &Execution{
		conn:    conn,
		errorId: errorId,
		message: message,
	}
}

func (execution *Execution) Error() string {
	return fmt.Sprintf("FatalID [%d] %s", execution.errorId, execution.message)
}

type Payload struct {
	Executions *list.List
}

func (middleware *Middleware) ProcessConnect(ctx *idtcp.ConnectContext) (*idtcp.Conn, error) {
	conn, err := ctx.Next()
	if err != nil {
		log.Printf("an error occurred: %v", err)
	}
	switch e := err.(type) {
	case *Execution:
		middleware.processExecution(e)
	}
	middleware.processExecutions(ctx.Payloads[Key].(*Payload).Executions)
	return conn, err
}

func (middleware *Middleware) ProcessDisconnect(ctx *idtcp.DisconnectContext) {
	ctx.Next()
}

func (middleware *Middleware) ProcessDistribute(ctx *idtcp.DistributeContext) error {
	if ctx.Instruction == middleware.s_FatalError {
		receiveFatalError(ctx.Conn, ctx.Data)
	}
	err := ctx.Next()
	if err != nil {
		log.Printf("an error occurred: %v", err)
	}
	switch e := err.(type) {
	case *Execution:
		middleware.processExecution(e)
	}
	middleware.processExecutions(ctx.Payloads[Key].(*Payload).Executions)
	return err
}

func (middleware *Middleware) processExecution(e *Execution) {
	defer e.conn.Close()
	var s utils.Serializer
	s.WriteInt32(e.errorId)
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
	log.Println(NewExecution(conn, errorId, message).Error())
}

func (middleware *Middleware) SetMdwKey(val int) {
	Key = val
}

func (middleware *Middleware) NewPayload() interface{} {
	return &Payload{Executions: list.New()}
}
