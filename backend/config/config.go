package config

import (
	"io/ioutil"

	"github.com/google/uuid"
	"gopkg.in/yaml.v2"
)

type Room struct {
	Name    string
	MaxUser int
}

type Config struct {
	Address      string
	ServerName   string
	Uuid         string
	DatabasePath string
	Rooms        []Room
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
