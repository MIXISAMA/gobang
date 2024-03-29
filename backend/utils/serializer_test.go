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
	A uint8
	B string `len_bytes:"1"`
	C []byte `len_bytes:"2"`
	D Char
	E bool
	F byte
	G [][]byte `len_bytes:"1" array_len_bytes:"1"`
	H []string `len_bytes:"1" array_len_bytes:"2"`
}

func newFoo() (Foo, []byte) {
	foo := Foo{
		A: 123,
		B: "bbbb",
		C: []byte("ccccccccccccc"),
		D: 'x',
		E: true,
		F: byte(0xAA),
		G: [][]byte{[]byte("g1g1g1g1g1"), []byte("g2g2g2g2")},
		H: []string{"h1h1h1h1", "h2h2h2h2h2"},
	}

	buf := new(bytes.Buffer)
	buf.WriteByte(foo.A)
	buf.WriteByte(uint8(len(foo.B)))
	buf.Write([]byte(foo.B))
	// fmt.Println(len(foo.c))
	binary.Write(buf, binary.LittleEndian, uint16(len(foo.C)))
	buf.Write(foo.C)
	buf.WriteByte(byte(foo.D))
	buf.WriteByte(0xFF)
	buf.WriteByte(foo.F)

	// G
	buf.WriteByte(uint8(len(foo.G)))
	buf.WriteByte(uint8(len(foo.G[0])))
	buf.Write(foo.G[0])
	buf.WriteByte(uint8(len(foo.G[1])))
	buf.Write(foo.G[1])

	// H
	binary.Write(buf, binary.LittleEndian, uint16(len(foo.H)))
	buf.WriteByte(uint8(len(foo.H[0])))
	buf.Write([]byte(foo.H[0]))
	buf.WriteByte(uint8(len(foo.H[1])))
	buf.Write([]byte(foo.H[1]))

	return foo, buf.Bytes()
}

func TestMarshal(t *testing.T) {
	assert := assert.New(t)

	foo, buf := newFoo()
	buf2, err := Marshal(foo)
	assert.Nil(err)
	buf3, err := Marshal(&foo)
	assert.Nil(err)

	fmt.Println(buf)

	assert.Equal(buf, buf2)
	assert.Equal(buf, buf3)
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

	i3, _ := newFoo()
	i3.B = RandomString(300)
	_, err = Marshal(i3)
	assert.Equal(t, errors.New("string with len_bytes 1 must have length <= 255"), err)
}

func TestUnmarshal(t *testing.T) {
	assert := assert.New(t)

	foo, buf := newFoo()
	var foo2 Foo
	err := Unmarshal(buf, &foo2)
	assert.Nil(err)
	assert.Equal(foo, foo2)
}

func TestUnmarshalOnlyAcceptPtr(t *testing.T) {
	_, buf := newFoo()
	var foo2 Foo
	err := Unmarshal(buf, foo2)
	assert.NotNil(t, err)
}

func BenchmarkMarshal(b *testing.B) {
	foo, _ := newFoo()
	for n := 0; n < b.N; n++ {
		Marshal(foo)
	}
}

func BenchmarkRawWrite(b *testing.B) {
	for n := 0; n < b.N; n++ {
		newFoo()
	}
}
