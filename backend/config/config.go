package config

import (
	"io/ioutil"

	"gopkg.in/yaml.v2"
)

type ConfigRoom struct {
	Name     string
	MaxUsers int
}

type Config struct {
	Server string
	Rooms  []ConfigRoom `yaml:",flow"`
}

func ReadConfig(path string) (conf *Config, err error) {

	file, err := ioutil.ReadFile(path)
	if err != nil {
		return
	}
	conf = new(Config)
	err = yaml.Unmarshal(file, conf)
	return

}
