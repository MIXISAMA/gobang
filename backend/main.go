package main

import (
	"flag"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	usermiddleware "github.com/MIXISAMA/gobang/backend/middlewares"
	"github.com/MIXISAMA/gobang/backend/server"
	"github.com/MIXISAMA/gobang/backend/udp"
)

func main() {

	var configPath string
	flag.StringVar(&configPath, "config", "./config.yaml", "config file path")
	flag.Parse()

	conf, err := ReadConfig(configPath)
	if err != nil {
		panic(err.Error())
	}

	var udpServer = udp.NewServer(conf.Server, server.UdpPipe)
	go udpServer.Run()

	userMiddleware, err := usermiddleware.New(2, 2, conf.DatabasePath, conf.Uuid)
	if err != nil {
		panic(err.Error())
	}

	idtcp.NewServer(
		conf.Server,
		server.Endpoints,
		[]idtcp.Middleware{userMiddleware},
	).Run()

}
