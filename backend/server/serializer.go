package server

import (
	"encoding/binary"
	"errors"
)

type Serializer struct {
	Raw    []byte
	cursor int
}

func MakeSerializer(Raw []byte) *Serializer {
	s := new(Serializer)
	s.Raw = Raw
	s.cursor = 0
	return s
}

func (s *Serializer) ReadUint16() (uint16, error) {
	var data = s.Raw[s.cursor : s.cursor+2]
	if len(data) != 2 {
		return 0, errors.New("GetUint16 error")
	}
	s.cursor += 2
	return binary.LittleEndian.Uint16(data), nil
}

func (s *Serializer) ReadString() (string, error) {
	length_u16, err := s.ReadUint16()
	if err != nil {
		return "", err
	}
	var length = int(length_u16)
	var strRaw = s.Raw[s.cursor : s.cursor+length]
	if len(strRaw) != length {
		return "", errors.New("GetString error")
	}
	s.cursor += length
	return string(strRaw), nil
}

func (s *Serializer) WriteByte(val byte) {
	s.Raw = append(s.Raw, val)
}

func (s *Serializer) WriteUint16(val uint16) {
	var data = make([]byte, 2)
	binary.LittleEndian.PutUint16(data, val)
	s.Raw = append(s.Raw, data...)
}

func (s *Serializer) WriteUint16_Int(val int) error {
	if val >= 65536 || val < 0 {
		return errors.New("val >= 65536 || val < 0")
	}
	s.WriteUint16(uint16(val))
	return nil
}

func (s *Serializer) WriteString(str string) error {
	length := len(str)
	err := s.WriteUint16_Int(length)
	if err != nil {
		return err
	}
	s.Raw = append(s.Raw, []byte(str)...)
	return nil
}

func (s *Serializer) WriteBoolean(val bool) {
	if val {
		s.WriteByte(0xFF)
	} else {
		s.WriteByte(0x00)
	}
}
