package idtcp

import (
	"encoding/binary"
	"errors"
	"log"
	"net"
)

type Conn struct {
	*net.TCPConn
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
	log.Printf("%-21s TCP Sent     [%2d] Length = %d", conn.RemoteAddr().String(), instruction, length)
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

	package_bytes := make([]byte, 2)
	err := conn.ReadN(package_bytes)
	if err != nil {
		return -1, err
	}
	length := int(binary.LittleEndian.Uint16(package_bytes))

	buffer := make([]byte, length-2)
	err = conn.ReadN(buffer)
	if err != nil {
		return -1, err
	}

	*instruction = binary.LittleEndian.Uint16(buffer[:2])
	*data = buffer[2:]

	log.Printf("%-21s TCP Received [%2d] Length = %d", conn.RemoteAddr().String(), *instruction, length)

	return length, nil
}
