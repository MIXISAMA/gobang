package mdwuser

import (
	"bytes"
	"database/sql"
	"errors"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	_ "github.com/mattn/go-sqlite3"
)

var (
	ErrUserNotFound  = errors.New("username is not found")
	ErrWrongPassword = errors.New("password is wrong")
)

type Database struct {
	path string
}

func NewDatabase(path string) (*Database, error) {

	db, err := sql.Open("sqlite3", path)
	if err != nil {
		return nil, err
	}
	defer db.Close()

	_, err = db.Exec("PRAGMA locking_mode = EXCLUSIVE;")
	if err != nil {
		return nil, err
	}

	_, err = db.Exec(`
		CREATE TABLE IF NOT EXISTS user (
			username TEXT PRIMARY KEY,
			password BLOB,
			wins     INTEGER DEFAULT 0,
			ties     INTEGER DEFAULT 0,
			losses   INTEGER DEFAULT 0,
			matches  INTEGER DEFAULT 0,
			duration INTEGER DEFAULT 0
		);`)
	if err != nil {
		return nil, err
	}

	return &Database{path: path}, nil
}

func (database *Database) authorization(username string, password []byte) error {
	db, err := sql.Open("sqlite3", database.path)
	if err != nil {
		return err
	}
	defer db.Close()

	stmt, err := db.Prepare(`
		SELECT password
		FROM user
		WHERE username = ?`)
	if err != nil {
		return err
	}
	defer stmt.Close()

	row := stmt.QueryRow(username)

	var dbPassword []byte
	err = row.Scan(&dbPassword)
	if err != nil {
		return ErrUserNotFound

	}
	if !bytes.Equal(password, dbPassword) {
		return ErrWrongPassword
	}

	return nil
}

func (database *Database) register(username string, password []byte) error {
	db, err := sql.Open("sqlite3", database.path)
	if err != nil {
		return err
	}
	defer db.Close()

	stmt2, err := db.Prepare(`
		INSERT INTO user (username, password)
		VALUES (?, ?)`)
	if err != nil {
		return err
	}
	defer stmt2.Close()
	_, err = stmt2.Exec(username, password)
	if err != nil {
		return err
	}
	return nil
}

func (database *Database) getUser(conn *idtcp.Conn, username string) (*User, error) {
	db, err := sql.Open("sqlite3", database.path)
	if err != nil {
		return nil, err
	}
	defer db.Close()

	stmt, err := db.Prepare(`
		SELECT wins, ties, losses, matches, duration
		FROM user
		WHERE username = ?`)
	if err != nil {
		return nil, err
	}
	defer stmt.Close()

	row := stmt.QueryRow(username)

	user := NewUser(username, conn)
	err = row.Scan(
		&user.NumOfWins,
		&user.NumOfTies,
		&user.NumOfLosses,
		&user.NumOfMatches,
		&user.GameDuration,
	)
	if err != nil {
		return nil, err
	}

	return user, nil
}
