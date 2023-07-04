package main

import (
	"io/ioutil"

	"github.com/google/uuid"
	"gopkg.in/yaml.v2"
)

type ConfigRoom struct {
	Name     string
	MaxUsers int
}

type Config struct {
	Server       string
	Name         string
	Uuid         string
	DatabasePath string
	RoomsNames   []string
}

func ReadConfig(path string) (*Config, error) {

	file, err := ioutil.ReadFile(path)
	if err != nil {
		return nil, err
	}

	conf := new(Config)
	err = yaml.Unmarshal(file, conf)
	if err != nil {
		return nil, err
	}

	if conf.Uuid == "" {
		conf.Uuid = uuid.New().String()

		yamlBytes, err := yaml.Marshal(&conf)
		if err != nil {
			return conf, err
		}
		err = ioutil.WriteFile(path, yamlBytes, 0644)
		if err != nil {
			return conf, err
		}
	}

	return conf, nil

}
