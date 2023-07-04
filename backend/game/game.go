package game

type Game struct {
	Rooms []Room
}

func (game Game) MakeRoom(n int) {
	game.Rooms = make([]Room, n)
	for i := range game.Rooms {
		game.Rooms[i].init()
	}
}
