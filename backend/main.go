package main

import (
	"flag"

	"github.com/MIXISAMA/gobang/backend/config"
	"github.com/MIXISAMA/gobang/backend/idtcp"
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

	userMiddleware, err := mdwuser.NewMiddleware(
		server.C_PublicKey,
		server.S_JoinRoom,
		server.C_YouJoinRoom,
		conf.DatabasePath,
		conf.Uuid,
	)
	if err != nil {
		panic(err.Error())
	}

	rooms := make([]*mdwroom.Room, len(conf.Rooms))
	for i := range rooms {
		rooms[i] = mdwroom.NewRoom(conf.Rooms[i].Name, conf.Rooms[i].MaxUser)
	}
	roomMiddleware := mdwroom.NewMiddleware(rooms)

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
			userMiddleware,
			roomMiddleware,
		},
	).Run()

}
