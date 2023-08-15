package idtcp

type Middleware interface {
	ProcessDistribute(*DistributeContext) error
	ProcessConnect(*ConnectContext) (*Conn, error)
	ProcessDisconnect(*DisconnectContext)
	SetMdwKey(int)
	NewPayload() interface{}
}

type DistributeContext struct {
	Request
	distribute func(*Request) error
	mdwList    []func(*DistributeContext) error
	mdwIdx     int
}

type ConnectContext struct {
	Payloads []interface{}
	connect  func() (*Conn, error)
	mdwList  []func(*ConnectContext) (*Conn, error)
	mdwIdx   int
}

type DisconnectContext struct {
	Payloads   []interface{}
	disconnect func(*Conn)
	mdwList    []func(*DisconnectContext)
	mdwIdx     int
	Conn       *Conn
}

func (ctx *DistributeContext) Next() error {
	ctx.mdwIdx++
	if ctx.mdwIdx < len(ctx.mdwList) {
		return ctx.mdwList[ctx.mdwIdx](ctx)
	}
	return ctx.distribute(&ctx.Request)
}

func (ctx *ConnectContext) Next() (*Conn, error) {
	ctx.mdwIdx++
	if ctx.mdwIdx < len(ctx.mdwList) {
		return ctx.mdwList[ctx.mdwIdx](ctx)
	}
	return ctx.connect()
}

func (ctx *DisconnectContext) Next() {
	ctx.mdwIdx++
	if ctx.mdwIdx < len(ctx.mdwList) {
		ctx.mdwList[ctx.mdwIdx](ctx)
		return
	}
	ctx.disconnect(ctx.Conn)
}

func NewDistributeContext(
	distribute func(*Request) error,
	mdwList []func(*DistributeContext) error,
	payloads []interface{},
	conn *Conn,
	instruction uint16,
	data []byte,
) *DistributeContext {
	return &DistributeContext{
		Request: Request{
			Instruction: instruction,
			Data:        data,
			Conn:        conn,
			Payloads:    payloads,
		},
		mdwIdx:     -1,
		distribute: distribute,
		mdwList:    mdwList,
	}
}

func NewConnectContext(
	connect func() (*Conn, error),
	mdwList []func(*ConnectContext) (*Conn, error),
	payloads []interface{},
) *ConnectContext {
	return &ConnectContext{
		Payloads: payloads,
		connect:  connect,
		mdwList:  mdwList,
		mdwIdx:   -1,
	}
}

func NewDisconnectContext(
	disconnect func(*Conn),
	mdwList []func(*DisconnectContext),
	payloads []interface{},
	conn *Conn,
) *DisconnectContext {
	return &DisconnectContext{
		Payloads:   payloads,
		disconnect: disconnect,
		mdwList:    mdwList,
		mdwIdx:     -1,
		Conn:       conn,
	}
}
