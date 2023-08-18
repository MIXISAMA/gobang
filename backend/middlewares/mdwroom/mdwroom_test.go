package mdwroom_test

import (
	"os"
	"testing"
	"time"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwroom"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
	"github.com/MIXISAMA/gobang/backend/testutils"
	"github.com/MIXISAMA/gobang/backend/utils"
	"github.com/stretchr/testify/assert"
)

const (
	s_JoinRoom      uint16 = 1
	s_LeaveRoom     uint16 = 2
	c_YouJoinRoom   uint16 = 3
	c_OtherJoinRoom uint16 = 4
	c_UserLeaveRoom uint16 = 5
	c_GameOver      uint16 = 6
)

var configRooms = []*mdwroom.ConfigRoom{
	{Name: "test room 1", MaxUsers: 10},
	{Name: "test room 2", MaxUsers: 20},
	{Name: "test room 3", MaxUsers: 0},
	{Name: "test room 4", MaxUsers: 300},
}

func newUserMiddleware(t *testing.T) *mdwuser.Middleware {
	databasePath := "test.db"
	mdwuser.NewDatabase(databasePath)
	t.Cleanup(func() { os.Remove("test.db") })

	userMiddleware, _ := mdwuser.NewMiddleware(
		s_JoinRoom,
		c_YouJoinRoom,
		databasePath,
		"cee330f8-303b-11ee-be56-000000000000",
	)
	return userMiddleware
}

func newRoomMiddleware() *mdwroom.Middleware {
	return mdwroom.NewMiddleware(
		s_JoinRoom,
		s_LeaveRoom,
		c_YouJoinRoom,
		c_OtherJoinRoom,
		c_UserLeaveRoom,
		c_GameOver,
		configRooms,
	)
}

type p_YouJoinRoomSuccess struct {
	Success   bool
	RoomName  string `len_bytes:"1"`
	MaxUsers  uint8
	Black     string   `len_bytes:"1"`
	White     string   `len_bytes:"1"`
	Onlookers []string `len_bytes:"1" array_len_bytes:"1"`
	Ready     byte
	Playing   bool
	Regret    byte
	Tie       byte
	Records   []byte `len_bytes:"1"`
}

type p_UserLeaveRoom struct {
	Username string `len_bytes:"1"`
}

type testContext struct {
	t      *testing.T
	client *testutils.TestClient

	mdwList           []idtcp.Middleware
	ctxDistributeList []func(*idtcp.DistributeContext) error
	payloads          []interface{}
}

func newDistributeContext(
	ctx *testContext,
	instruction uint16,
	data []byte,
) *idtcp.DistributeContext {
	return idtcp.NewDistributeContext(
		func(*idtcp.Request) error { return nil },
		ctx.ctxDistributeList,
		ctx.payloads,
		ctx.client.Conn,
		instruction,
		data,
	)
}

func setup(t *testing.T) *testContext {
	userMdw := newUserMiddleware(t)
	roomMdw := newRoomMiddleware()
	userMdw.SetMdwKey(0)
	roomMdw.SetMdwKey(1)

	return &testContext{
		t:       t,
		client:  testutils.NewTestClient(t),
		mdwList: []idtcp.Middleware{userMdw, roomMdw},
		ctxDistributeList: []func(*idtcp.DistributeContext) error{
			userMdw.ProcessDistribute,
			roomMdw.ProcessDistribute,
		},
		payloads: []interface{}{
			userMdw.NewPayload(),
			roomMdw.NewPayload(),
		},
	}
}

func teardown(t *testing.T, ctx *testContext) {

	ctx.client.Close()
}

func joinRoomHelper(tctx *testContext) {
	assert := assert.New(tctx.t)

	i_JoinRoom := mdwuser.InstructionJoinRoom{
		RoomID:   0,
		Username: "foo",
		Password: []byte("bar"),
		Role:     'P',
	}
	d_JoinRoom, err := utils.Marshal(i_JoinRoom)
	assert.Nil(err)

	err = newDistributeContext(tctx, s_JoinRoom, d_JoinRoom).Next()
	assert.Nil(err)

	ct_YouJoinRoom, dt_YouJoinRoomSuccess := tctx.client.Read(2 * time.Second)
	assert.Equal(c_YouJoinRoom, ct_YouJoinRoom)
	var i_YouJoinRoomSuccess p_YouJoinRoomSuccess
	err = utils.Unmarshal(dt_YouJoinRoomSuccess, &i_YouJoinRoomSuccess)
	assert.Nil(err)
	assert.True(i_YouJoinRoomSuccess.Success)
}

func TestJoinAndLeaveRoom(t *testing.T) {
	assert := assert.New(t)

	tctx := setup(t)
	defer teardown(t, tctx)

	joinRoomHelper(tctx)

	err := newDistributeContext(tctx, s_LeaveRoom, []byte{}).Next()
	assert.Nil(err)

	ct_UserLeaveRoom, dt_UserLeaveRoom := tctx.client.Read(2 * time.Second)
	assert.Equal(c_UserLeaveRoom, ct_UserLeaveRoom)
	var i_UserLeaveRoom p_UserLeaveRoom
	err = utils.Unmarshal(dt_UserLeaveRoom, &i_UserLeaveRoom)
	assert.Nil(err)

}
