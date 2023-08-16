package utils

type S_LeaveRoom struct {
	Username string `len_bytes:"1"`
}

type S_Message struct {
	Timestamp uint64
	Username  string `len_bytes:"1"`
	Message   string `len_bytes:"2"`
}

type C_YouJoinRoom struct {
	IsSuccess     bool
	RoomName      string `len_bytes:"1"`
	MaxUsers      uint8
	BlackUsername string   `len_bytes:"1"`
	WhileUsername string   `len_bytes:"1"`
	Onlookers     []string `len_bytes:"1" array_len_bytes:"1"`
	ReadyPlayer   byte
	IsPlaying     bool
	RegretPlayer  byte
	TiePlayer     byte
	Records       []byte `len_bytes:"1"`
}

type C_OtherJoinRoom struct {
	Username string `len_bytes:"1"`
	Role     Char
}

type C_Message struct {
	Message string `len_bytes:"2"`
}
