package main

import (
	"flag"

	"github.com/MIXISAMA/gobang/backend/config"
	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwfatal"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwroom"
	"github.com/MIXISAMA/gobang/backend/middlewares/mdwuser"
	"github.com/MIXISAMA/gobang/backend/server"
	"github.com/MIXISAMA/gobang/backend/udp"
)

func main() {

	var configPath string
	flag.StringVar(&configPath, "config", "./config.yaml", "config file path")
	flag.Parse()

	conf, err := config.ReadConfig(configPath)
	if err != nil {
		panic(err.Error())
	}

	fatalMiddleware := mdwfatal.NewMiddleware(
		server.S_FatalError,
		server.C_FatalError,
	)

	userMiddleware, err := mdwuser.NewMiddleware(
		server.S_JoinRoom,
		server.C_YouJoinRoom,
		conf.DatabasePath,
		conf.Uuid,
	)
	if err != nil {
		panic(err.Error())
	}

	rooms := make([]*mdwroom.ConfigRoom, len(conf.Rooms))
	for i := range rooms {
		rooms[i] = &mdwroom.ConfigRoom{
			Name:     conf.Rooms[i].Name,
			MaxUsers: conf.Rooms[i].MaxUser,
		}
	}
	roomMiddleware := mdwroom.NewMiddleware(
		server.S_JoinRoom,
		server.S_LeaveRoom,
		server.C_YouJoinRoom,
		server.C_OtherJoinRoom,
		server.C_UserLeaveRoom,
		server.C_GameOver,
		rooms,
	)

	serv := server.NewServer(
		conf.ServerName,
		roomMiddleware,
	)

	var udpServer = udp.NewServer(conf.Address, serv.UdpPipe)
	go udpServer.Run()

	idtcp.NewServer(
		conf.Address,
		server.Endpoints,
		[]idtcp.Middleware{
			fatalMiddleware,
			userMiddleware,
			roomMiddleware,
		},
	).Run()

}
