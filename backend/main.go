package main

import (
	"flag"

	"github.com/MIXISAMA/gobang/backend/config"
	"github.com/MIXISAMA/gobang/backend/game"
	"github.com/MIXISAMA/gobang/backend/server"
)

func main() {

	var configPath string
	flag.StringVar(&configPath, "config", "./config.yaml", "config file path")
	flag.Parse()

	conf, err := config.ReadConfig(configPath)
	if err != nil {
		panic(err.Error())
	}

	game.LoadRoomListFromConfig(conf)

	var udpServer = server.NewUdpServer(conf.Server, game.UdpPipe)
	go udpServer.Run()

	var idtcpServer = server.NewIdtcpServer(conf.Server, game.Endpoints)
	idtcpServer.Run()

}
