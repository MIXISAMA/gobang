package utils

import (
	"bytes"
	"encoding/binary"
	"errors"
	"fmt"
	"testing"

	"github.com/stretchr/testify/assert"
)

type Foo struct {
	a uint8
	b string `len_bytes:"1"`
	c []byte `len_bytes:"2"`
	d char
	e bool
	f byte
}

func newFoo() (Foo, []byte) {
	foo := Foo{
		a: 123,
		b: "xxxx",
		c: []byte("xxxxbbbbbbb"),
		d: 'x',
		e: true,
		f: byte(0xAA),
	}

	buf := new(bytes.Buffer)
	buf.WriteByte(foo.a)
	buf.WriteByte(uint8(len(foo.b)))
	buf.Write([]byte(foo.b))
	fmt.Println(len(foo.c))
	binary.Write(buf, binary.LittleEndian, uint16(len(foo.c)))
	buf.Write(foo.c)
	buf.WriteByte(byte(foo.d))
	buf.WriteByte(0xFF)
	buf.WriteByte(foo.f)

	return foo, buf.Bytes()
}

func TestMarshal(t *testing.T) {
	assert := assert.New(t)

	foo, buf2 := newFoo()
	buf, err := Marshal(foo)
	assert.Nil(err)

	fmt.Println(buf)

	assert.Equal(buf2, buf)
}

func TestMarshalInvalidStruct(t *testing.T) {
	type Invalid1 struct {
		xx []byte
	}
	i1 := Invalid1{xx: []byte("xx")}
	_, err := Marshal(i1)
	assert.Equal(t, errors.New("invalid `len_bytes` struct tag"), err)

	type Invalid2 struct {
		aa Invalid1
	}
	i2 := Invalid2{aa: i1}
	_, err = Marshal(i2)
	assert.Equal(t, errors.New("unhandled type"), err)
}
