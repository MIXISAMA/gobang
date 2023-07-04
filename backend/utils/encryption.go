package utils

import (
	"crypto/md5"
	"crypto/rand"
	"encoding/base64"
	"encoding/hex"
)

func UpdatePassword(password string, salt string) string {
	b := []byte(password)
	s := []byte(salt)
	h := md5.New()
	h.Write(s)
	h.Write(b)
	var res []byte
	res = h.Sum(nil)
	for i := 0; i < 100; i++ {
		h.Reset()
		h.Write(res)
		res = h.Sum(nil)
	}
	return hex.EncodeToString(res)
}

func RandomString() string {
	randomBytes := make([]byte, 16)

	_, err := rand.Read(randomBytes)
	if err != nil {
		panic(err)
	}
	return base64.URLEncoding.EncodeToString(randomBytes)
}
