package idtcp

type Middleware interface {
	ProcessDistribute(*DistributeContext) error
	ProcessConnect(*ConnectContext) (*Conn, error)
	ProcessDisconnect(*DisconnectContext)
	SetMdwKey(int)
	NewPayload() interface{}
}

type middlewareContext struct {
	Payloads []interface{}
	server   *Server
	mdwIdx   int
}

type DistributeContext struct {
	middlewareContext
	Conn        *Conn
	Instruction uint16
	Data        []byte
}

type ConnectContext struct {
	middlewareContext
}

type DisconnectContext struct {
	middlewareContext
	Conn *Conn
}

func (ctx *DistributeContext) Next() error {
	ctx.mdwIdx++
	if ctx.mdwIdx < len(ctx.server.middlewareList) {
		return ctx.server.middlewareList[ctx.mdwIdx].ProcessDistribute(ctx)
	}
	return ctx.server.distribute(&Request{
		Instruction: ctx.Instruction,
		Data:        ctx.Data,
		Conn:        ctx.Conn,
		Payloads:    ctx.Payloads,
	})
}

func (ctx *ConnectContext) Next() (*Conn, error) {
	ctx.mdwIdx++
	if ctx.mdwIdx < len(ctx.server.middlewareList) {
		return ctx.server.middlewareList[ctx.mdwIdx].ProcessConnect(ctx)
	}
	return ctx.server.connect()
}

func (ctx *DisconnectContext) Next() {
	ctx.mdwIdx++
	if ctx.mdwIdx < len(ctx.server.middlewareList) {
		ctx.server.middlewareList[ctx.mdwIdx].ProcessDisconnect(ctx)
		return
	}
	ctx.server.disconnect(ctx.Conn)
}

func newDistributeContext(
	server *Server,
	payloads []interface{},
	conn *Conn,
	instruction uint16,
	data []byte,
) *DistributeContext {
	return &DistributeContext{
		middlewareContext: middlewareContext{
			Payloads: payloads,
			server:   server,
			mdwIdx:   -1,
		},
		Conn:        conn,
		Instruction: instruction,
		Data:        data,
	}
}

func newConnectContext(
	server *Server,
	payloads []interface{},
) *ConnectContext {
	return &ConnectContext{
		middlewareContext: middlewareContext{
			Payloads: payloads,
			server:   server,
			mdwIdx:   -1,
		},
	}
}

func newDisconnectContext(
	server *Server,
	payloads []interface{},
	conn *Conn,
) *DisconnectContext {
	return &DisconnectContext{
		middlewareContext: middlewareContext{
			Payloads: payloads,
			server:   server,
			mdwIdx:   -1,
		},
		Conn: conn,
	}
}
