package main

import (
	"flag"

	"github.com/MIXISAMA/gobang/backend/config"
	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/middlewares/room"
	"github.com/MIXISAMA/gobang/backend/middlewares/user"
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

	userMiddleware, err := user.NewMiddleware(2, 2, conf.DatabasePath, conf.Uuid)
	if err != nil {
		panic(err.Error())
	}

	roomMiddleware := room.NewMiddleware(conf.Rooms)

	serv := server.NewServer(
		conf.ServerName,
		userMiddleware,
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
