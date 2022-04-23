package game

// import (
// 	"fmt"

// 	"github.com/MIXISAMA/gobang/backend/game"
// 	"github.com/MIXISAMA/gobang/backend/idtcp"
// )

// const (
// 	C_PlayerJoinRoom        uint16 = 0x0000
// 	C_PlayerJoinRoomSuccess uint16 = 0x0000
// 	C_PlayerLeaveRoom       uint16 = 0x0001
// 	C_PlayerBlack           uint16 = 0x0002
// 	C_PlayerReady           uint16 = 0x0001
// 	C_PlayerStone           uint16 = 0x0000
// 	C_PlayerRegretRequest   uint16 = 0x0003
// 	C_PlayerAdmitDefeat     uint16 = 0x0000

// 	C_OnlookerAll             uint16 = 0x0000
// 	C_OnlookerJoinRoom        uint16 = 0x0002
// 	C_OnlookerJoinRoomSuccess uint16 = 0x0000
// 	C_OnlookerLeaveRoom       uint16 = 0x0000

// 	C_GameRoom  uint16 = 0x0001
// 	C_GameStart uint16 = 0x0005
// 	C_GameEnd   uint16 = 0x0006
// )

// func (s *Serializer) writePlayer(player *game.Player) error {
// 	err := s.WriteString(player.Name)
// 	if err != nil {
// 		return err
// 	}
// 	s.WriteBoolean(player.Ready)
// 	s.WriteByte(player.Color)
// 	return nil
// }

// func (room *Room) sendRoom(conn *idtcp.Conn) error {
// 	/*
// 		string:      room name
// 		boolean:     is playing
// 		15*15 byte:  chess board
// 		[]byte:      record
// 		boolean:     exist player 1
// 			string:  player name
// 			boolean: ready
// 			byte:	 color
// 		boolean:     exist player 2
// 			...
// 	*/
// 	var s Serializer
// 	err := s.WriteString(room.Name)
// 	if err != nil {
// 		return err
// 	}
// 	s.WriteBoolean(room.IsPlaying)
// 	for i := 0; i < 15; i++ {
// 		for j := 0; j < 15; j++ {
// 			s.WriteByte(room.ChessBoard[i][j])
// 		}
// 	}
// 	err = s.WriteUint16_Int(len(room.Record))
// 	if err != nil {
// 		return err
// 	}
// 	for i := range room.Record {
// 		s.WriteByte(room.Record[i])
// 	}

// 	for i := range room.Players {
// 		player := room.Players[i]
// 		if player == nil {
// 			s.WriteBoolean(false)
// 			continue
// 		}
// 		s.WriteBoolean(true)
// 		err = s.writePlayer(player)
// 		if err != nil {
// 			return err
// 		}
// 	}
// 	_, err = conn.Write(C_GameRoom, s.Raw)
// 	return err
// }

// func (room *Room) sendOnlookers(conn *idtcp.Conn) error {
// 	/*
// 		[]string:    names of onlookers
// 	*/
// 	var s Serializer
// 	err := s.WriteUint16_Int(len(room.Onlookers))
// 	if err != nil {
// 		return err
// 	}
// 	for i := range room.Onlookers {
// 		err = s.WriteString(room.Onlookers[i])
// 		if err != nil {
// 			return err
// 		}
// 	}

// 	_, err = conn.Write(C_OnlookerAll, s.Raw)
// 	return err
// }

// func (room *Room) sendAll(conn *idtcp.Conn) (err error) {
// 	err = room.sendRoom(conn)
// 	if err != nil {
// 		return
// 	}
// 	err = room.sendOnlookers(conn)
// 	return
// }

// func (room *Room) sendJoinPlayerToOthers(player *game.Player) error {
// 	/*
// 		string:  player name
// 		boolean: ready
// 		byte:	 color
// 	*/

// 	var s Serializer

// 	err := s.writePlayer(player)
// 	if err != nil {
// 		return err
// 	}

// 	playerConn := room.ConnMap[player.Name]
// 	for _, conn := range room.ConnMap {
// 		if conn == playerConn {
// 			continue
// 		}
// 		_, err = conn.Write(C_PlayerJoinRoom, s.Raw)
// 		if err != nil {
// 			fmt.Println(err.Error())
// 			continue
// 		}
// 	}
// 	return nil
// }

// func (room *Room) sendNameToOther(name string, instruction uint16) error {
// 	var s Serializer

// 	err := s.WriteString(name)
// 	if err != nil {
// 		return err
// 	}

// 	selfConn := room.ConnMap[name]
// 	for _, conn := range room.ConnMap {
// 		if conn == selfConn {
// 			continue
// 		}
// 		_, err = conn.Write(instruction, s.Raw)
// 		if err != nil {
// 			fmt.Println(err.Error())
// 			continue
// 		}
// 	}
// 	return nil
// }

// func (room *Room) sendJoinOnlookerToOthers(name string) error {
// 	return room.sendNameToOther(name, C_OnlookerJoinRoom)
// }

// func (room *Room) sendLeavePlayerToOthers(name string) error {
// 	return room.sendNameToOther(name, C_PlayerLeaveRoom)
// }

// func (room *Room) sendLeaveOnlookerToOthers(name string) error {
// 	return room.sendNameToOther(name, C_OnlookerLeaveRoom)
// }

// func sendPlayerLeaveSuccess(conn *idtcp.Conn) error {
// 	_, err := conn.Write(C_PlayerLeaveRoom, nil)
// 	return err
// }

// func sendOnlookerLeaveSuccess(conn *idtcp.Conn) error {
// 	_, err := conn.Write(C_OnlookerLeaveRoom, nil)
// 	return err
// }
