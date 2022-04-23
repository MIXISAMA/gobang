package idtcp

import (
	"encoding/binary"
	"errors"
	"fmt"
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

func (conn *Conn) ReadN(buffer []byte) error {
	var req_n = len(buffer)
	var cur_n = 0
	for cur_n != req_n {
		tmp_buf := make([]byte, req_n-cur_n)
		read_n, err := conn.TCPConn.Read(tmp_buf)
		if err != nil {
			return err
		}
		for i := 0; i < read_n; i++ {
			buffer[cur_n+i] = tmp_buf[i]
		}
		cur_n += read_n
	}
	return nil
}

func (conn *Conn) Read(instruction *uint16, data *[]byte) (int, error) {

	conn.buffer = nil
	fmt.Printf("Read 1: %d \n", 2)
	package_bytes := make([]byte, 2)
	err := conn.ReadN(package_bytes)
	if err != nil {
		return -1, err
	}
	length := int(binary.LittleEndian.Uint16(package_bytes))
	fmt.Printf("Read 2: %d \n", length)
	buffer := make([]byte, length-2)
	err = conn.ReadN(buffer)
	if err != nil {
		return -1, err
	}
	fmt.Println("Done")
	*instruction = binary.LittleEndian.Uint16(buffer[:2])
	*data = buffer[2:]

	return length, nil
}
