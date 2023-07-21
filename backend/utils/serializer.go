package utils

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

func (s *Serializer) ReadByte() (byte, error) {
	if s.cursor >= len(s.Raw) {
		return 0x00, errors.New("ReadByte error")
	}
	b := s.Raw[s.cursor]
	s.cursor++
	return b, nil
}

func (s *Serializer) ReadBoolean() (bool, error) {
	b, err := s.ReadByte()
	if err != nil {
		return false, err
	}
	return b != 0x00, nil
}

func (s *Serializer) ReadUint8() (uint8, error) {
	return s.ReadByte()
}

func (s *Serializer) ReadUint16() (uint16, error) {
	var data = s.Raw[s.cursor : s.cursor+2]
	if len(data) != 2 {
		return 0, errors.New("ReadUint16 error")
	}
	s.cursor += 2
	return binary.LittleEndian.Uint16(data), nil
}

func (s *Serializer) ReadUint32() (uint32, error) {
	var data = s.Raw[s.cursor : s.cursor+4]
	if len(data) != 4 {
		return 0, errors.New("ReadInt32 error")
	}
	s.cursor += 4
	return binary.LittleEndian.Uint32(data), nil
}

func (s *Serializer) ReadInt32() (int32, error) {
	n, err := s.ReadUint32()
	return int32(n), err
}

func (s *Serializer) ReadBytes8() ([]byte, error) {
	length_u8, err := s.ReadUint8()
	if err != nil {
		return nil, nil
	}
	var length = int(length_u8)
	var bytesRaw = s.Raw[s.cursor : s.cursor+length]
	if len(bytesRaw) != length {
		return nil, errors.New("ReadBytes8 error")
	}
	s.cursor += length
	return bytesRaw, nil
}

func (s *Serializer) ReadBytes16() ([]byte, error) {
	length_u16, err := s.ReadUint16()
	if err != nil {
		return nil, nil
	}
	var length = int(length_u16)
	var bytesRaw = s.Raw[s.cursor : s.cursor+length]
	if len(bytesRaw) != length {
		return nil, errors.New("ReadBytes16 error")
	}
	s.cursor += length
	return bytesRaw, nil
}

func (s *Serializer) ReadString8() (string, error) {
	bytesRaw, err := s.ReadBytes8()
	if err != nil {
		return "", err
	}
	return string(bytesRaw), nil
}

func (s *Serializer) ReadString16() (string, error) {
	bytesRaw, err := s.ReadBytes16()
	if err != nil {
		return "", err
	}
	return string(bytesRaw), nil
}

func (s *Serializer) WriteByte(val byte) error {
	s.Raw = append(s.Raw, val)
	return nil
}

func (s *Serializer) WriteUint8_Int(val int) error {
	if val >= 256 || val < 0 {
		return errors.New("val >= 256 || val < 0")
	}
	s.WriteByte(uint8(val))
	return nil
}

func (s *Serializer) WriteBoolean(val bool) {
	if val {
		s.WriteByte(0xFF)
	} else {
		s.WriteByte(0x00)
	}
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

func (s *Serializer) WriteUint32(val uint32) {
	var data = make([]byte, 4)
	binary.LittleEndian.PutUint32(data, val)
	s.Raw = append(s.Raw, data...)
}

func (s *Serializer) WriteUint64(val uint64) {
	var data = make([]byte, 8)
	binary.LittleEndian.PutUint64(data, val)
	s.Raw = append(s.Raw, data...)
}

func (s *Serializer) WriteInt32(val int32) {
	s.WriteUint32(uint32(val))
}

func (s *Serializer) WriteBytes8(val []byte) error {
	err := s.WriteUint8_Int(len(val))
	if err != nil {
		return err
	}
	s.Raw = append(s.Raw, val...)
	return nil
}

func (s *Serializer) WriteString8(str string) error {
	return s.WriteBytes8([]byte(str))
}

func (s *Serializer) WriteBytes16(val []byte) error {
	err := s.WriteUint16_Int(len(val))
	if err != nil {
		return err
	}
	s.Raw = append(s.Raw, val...)
	return nil
}

func (s *Serializer) WriteString16(str string) error {
	return s.WriteBytes16([]byte(str))
}
