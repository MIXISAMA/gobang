package utils

import (
	"bytes"
	"encoding/binary"
	"errors"
	"fmt"
	"reflect"
)

// For now, serializer can only serialize centain basic types
// refer to design doc for more info

// string and byte types must have a `len_bytes` or `array_len_bytes` struct tag
// e.g.
// B []byte `len_bytes:"2"`
// C [][]byte `len_bytes:"2" array_len_bytes:"1"`

// fields to be serialized must be exported
// since package reflect can only change exported fields' values

type Char uint8

type len_type string

const (
	array_len_bytes len_type = "array_len_bytes"
	len_bytes       len_type = "len_bytes"
)

// Marshal can accept both an object or a pointer
func Marshal(obj any) ([]byte, error) {
	if reflect.ValueOf(obj).Kind() == reflect.Ptr {
		obj = reflect.ValueOf(obj).Elem().Interface()
	}

	t := reflect.TypeOf(obj)
	v := reflect.ValueOf(obj)
	buf := new(bytes.Buffer)
	fields := reflect.VisibleFields(t)
	for _, field := range fields {
		kind := field.Type.Kind()
		value := v.FieldByIndex(field.Index)
		var err error
		switch kind {
		// includes char, uint8
		case reflect.Uint8:
			err = buf.WriteByte(uint8(value.Uint()))
		case reflect.Uint16:
			err = binary.Write(buf, binary.LittleEndian, uint16(value.Uint()))
		case reflect.Uint32:
			err = binary.Write(buf, binary.LittleEndian, uint32(value.Uint()))
		case reflect.Uint64:
			err = binary.Write(buf, binary.LittleEndian, value.Uint())
		case reflect.Bool:
			if value.Bool() {
				err = buf.WriteByte(0xFF)
			} else {
				err = buf.WriteByte(0x00)
			}
		case reflect.String:
			err = writeString(&field, &value, buf)

		case reflect.Slice:
			switch field.Type.Elem().Kind() {
			case reflect.Uint8:
				// []byte
				err = writeBytes(&field, &value, buf)
			case reflect.String:
				// []string
				len_arr := value.Len()
				err = writeLen(&field, &value, buf, "array_len_bytes", len_arr)
				if err != nil {
					return nil, err
				}
				for i := 0; i < len_arr; i++ {
					v := value.Index(i)
					err = writeString(&field, &v, buf)
					if err != nil {
						break
					}
				}

			case reflect.Slice:
				// [][]byte
				// FIXME: very confusing...
				if field.Type.Elem().Elem().Kind() != reflect.Uint8 {
					return nil, errors.New("unhandled type")
				}

				len_arr := value.Len()
				err = writeLen(&field, &value, buf, "array_len_bytes", len_arr)
				if err != nil {
					return nil, err
				}
				for i := 0; i < len_arr; i++ {
					v := value.Index(i)
					err = writeBytes(&field, &v, buf)
					if err != nil {
						break
					}
				}
			default:
				return nil, errors.New("unhandled type")
			}

		default:
			err = errors.New("unhandled type")
		}

		if err != nil {
			return nil, err
		}
	}

	return buf.Bytes(), nil
}

func writeString(field *reflect.StructField, value *reflect.Value, buf *bytes.Buffer) error {
	err := writeLen(field, value, buf, "len_bytes", len(value.String()))
	if err != nil {
		return err
	}

	_, err = buf.Write([]byte(value.String()))
	return err
}

func writeBytes(field *reflect.StructField, value *reflect.Value, buf *bytes.Buffer) error {
	err := writeLen(field, value, buf, "len_bytes", len(value.Bytes()))
	if err != nil {
		return err
	}

	_, err = buf.Write(value.Bytes())
	return err
}

func writeLen(field *reflect.StructField, value *reflect.Value, buf *bytes.Buffer, tag len_type, length int) (err error) {
	len_v := length
	len_bytes_str := field.Tag.Get(string(tag))
	if len_bytes_str != "1" && len_bytes_str != "2" {
		err = fmt.Errorf("invalid `%v` struct tag", tag)
	} else {
		if len_bytes_str == "1" {
			if len_v > 255 || len_v < 0 {
				err = fmt.Errorf("string with %v 1 must have length <= 255", tag)
			} else {
				err = buf.WriteByte(uint8(len_v))
			}
		} else {
			if len_v > 65535 || len_v < 0 {
				err = fmt.Errorf("string with %v 1 must have length <= 65535", tag)
			} else {
				err = binary.Write(buf, binary.LittleEndian, uint16(len_v))
			}
		}
	}
	return err
}

// Unmarshal only accepts a pointer to an object
// because it modifies data in place
func Unmarshal(b []byte, obj any) error {
	if reflect.ValueOf(obj).Kind() != reflect.Ptr {
		return errors.New("cannot unmarshal to a non-pointer type")
	}

	t := reflect.TypeOf(obj).Elem()
	v := reflect.ValueOf(obj).Elem()
	buf := bytes.NewBuffer(b)
	fields := reflect.VisibleFields(t)
	for _, field := range fields {
		kind := field.Type.Kind()
		value := v.FieldByIndex(field.Index)
		var err error
		switch kind {
		// includes char, uint8
		case reflect.Uint8:
			var data uint8
			data, err = buf.ReadByte()
			value.SetUint(uint64(data))
		case reflect.Uint16:
			var data uint16
			err = binary.Read(buf, binary.LittleEndian, &data)
			value.SetUint(uint64(data))
		case reflect.Uint32:
			var data uint32
			err = binary.Read(buf, binary.LittleEndian, &data)
			value.SetUint(uint64(data))
		case reflect.Uint64:
			var data uint64
			err = binary.Read(buf, binary.LittleEndian, &data)
			value.SetUint(uint64(data))
		case reflect.Bool:
			var data byte
			data, err = buf.ReadByte()
			if err != nil {
				return err
			}
			if data == 0xFF {
				value.SetBool(true)
			} else if data == 0x00 {
				value.SetBool(false)
			} else {
				err = errors.New("unexpected byte")
			}
		case reflect.String:
			err = readString(&field, &value, buf)

		case reflect.Slice:
			switch field.Type.Elem().Kind() {
			case reflect.Uint8:
				// []byte
				err = readBytes(&field, &value, buf)
			case reflect.String:
				// []string
				var length int
				length, err = readLen(&field, buf, array_len_bytes)
				if err != nil {
					return err
				}
				for i := 0; i < length; i++ {
					v := reflect.ValueOf(new(string)).Elem()
					err = readString(&field, &v, buf)
					if err != nil {
						break
					}
					value.Set(reflect.Append(value, v))
				}
			case reflect.Slice:
				// [][]byte
				// FIXME: very confusing
				if field.Type.Elem().Elem().Kind() != reflect.Uint8 {
					return errors.New("unhandled type")
				}

				var length int
				length, err = readLen(&field, buf, array_len_bytes)
				if err != nil {
					return err
				}
				for i := 0; i < length; i++ {
					v := reflect.ValueOf(new([]byte)).Elem()
					fmt.Println(v.CanAddr())
					err = readBytes(&field, &v, buf)
					if err != nil {
						break
					}
					value.Set(reflect.Append(value, v))
				}
			default:
				err = errors.New("unhandled type")
			}

		default:
			err = errors.New("unhandled type")
		}

		if err != nil {
			return err
		}
	}

	return nil
}

func readString(field *reflect.StructField, value *reflect.Value, buf *bytes.Buffer) error {
	data, err := readData(field, value, buf)
	if err != nil {
		return err
	}

	value.SetString(string(data))
	return nil
}

func readBytes(field *reflect.StructField, value *reflect.Value, buf *bytes.Buffer) error {
	data, err := readData(field, value, buf)
	if err != nil {
		return err
	}

	value.SetBytes(data)
	return nil
}

func readLen(field *reflect.StructField, buf *bytes.Buffer, tag len_type) (length int, err error) {
	len_bytes_str := field.Tag.Get(string(tag))
	if len_bytes_str != "1" && len_bytes_str != "2" {
		err = fmt.Errorf("invalid `%v` struct tag", tag)
	} else {
		if len_bytes_str == "1" {
			var data uint8
			data, err = buf.ReadByte()
			length = int(uint8(data))
		} else {
			var data uint16
			err = binary.Read(buf, binary.LittleEndian, &data)
			length = int(data)
		}
	}
	if err != nil {
		return 0, err
	}
	return length, nil
}

func readData(field *reflect.StructField, value *reflect.Value, buf *bytes.Buffer) (data []byte, err error) {
	length, err := readLen(field, buf, "len_bytes")
	if err != nil {
		return nil, err
	}

	data = make([]byte, length)
	n, err := buf.Read(data)
	if err != nil {
		return nil, err
	}
	if n != length {
		return nil, errors.New("not enough bytes to read")
	}

	return data, nil
}

// APIs below are deprecated

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
