package idtcp

import (
	"encoding/binary"
	"errors"
	"net"
)

type Conn struct {
	TCPConn *net.TCPConn
	buffer  []byte
	tmpbuf  []byte
}

func NewConn(tcpConn *net.TCPConn) *Conn {
	idtcpConn := new(Conn)
	idtcpConn.TCPConn = tcpConn
	idtcpConn.tmpbuf = make([]byte, 65536)
	return idtcpConn
}

func (conn *Conn) Write(instruction uint16, data []byte) (int, error) {

	length := len(data) + 4
	if length >= 65536 {
		return 0, errors.New("payload is out of size (65531)")
	}

	bufferLength := make([]byte, 2)
	bufferInstruction := make([]byte, 2)
	binary.LittleEndian.PutUint16(bufferLength, uint16(length))
	binary.LittleEndian.PutUint16(bufferInstruction, instruction)

	payload := append(bufferLength, bufferInstruction...)
	payload = append(payload, data...)

	return conn.TCPConn.Write(payload)
}

func (conn *Conn) Read(instruction *uint16, data *[]byte) (int, error) {

	for {

		bufferLength := len(conn.buffer)

		if bufferLength >= 4 {
			len := binary.LittleEndian.Uint16(conn.buffer[0:2])
			ins := binary.LittleEndian.Uint16(conn.buffer[2:4])

			if bufferLength > int(len) {
				*data = conn.buffer[4:bufferLength]
				*instruction = ins
				return bufferLength, nil
			}
		}

		bufLen, err := conn.TCPConn.Read(conn.tmpbuf)
		if err != nil {
			return bufLen, err
		}
		conn.buffer = append(conn.buffer, conn.tmpbuf[:bufLen]...)

	}

}

type Message struct {
	Connect     *Conn
	Instruction uint16
	Data        []byte
}

func (conn *Conn) ReadMessage() (*Message, error) {
	msg := new(Message)
	_, err := conn.Read(&msg.Instruction, &msg.Data)
	if err != nil {
		return nil, err
	}
	msg.Connect = conn
	return msg, nil
}
