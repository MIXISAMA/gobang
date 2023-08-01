package mdwuser

import (
	"database/sql"
	"errors"
	"fmt"
	"strings"

	"crypto/sha256"
	"crypto/subtle"
	b64 "encoding/base64"

	"github.com/MIXISAMA/gobang/backend/idtcp"
	"github.com/MIXISAMA/gobang/backend/utils"
	_ "github.com/mattn/go-sqlite3"
	"golang.org/x/crypto/pbkdf2"
)

var (
	ErrUserNotFound  = errors.New("username is not found")
	ErrWrongPassword = errors.New("password is wrong")
)

func hasher_encode(password, salt []byte) string {
	iterations := 720000 // derived from https://github.com/django/django/blob/22b0b73c7732ba67db4e69fd9fa75aad84c8e5c4/django/contrib/auth/hashers.py#L316

	hash := pbkdf2.Key(password, salt, iterations, 32, sha256.New)
	hash_str := b64.StdEncoding.EncodeToString(hash)
	return fmt.Sprintf("pbkdf2$%v$%v$%v", iterations, string(salt), hash_str)
}

func hasher_verify(password []byte, encoded string) bool {
	splited := strings.Split(encoded, "$")
	salt := splited[2]
	encoded_2 := hasher_encode(password, []byte(salt))
	if subtle.ConstantTimeCompare([]byte(encoded), []byte(encoded_2)) == 0 {
		return false
	} else {
		return true
	}
}

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
			password TEXT,
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

func (database *Database) authenticate(username string, password []byte) error {
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

	var dbPassword string
	err = row.Scan(&dbPassword)
	if err != nil {
		return ErrUserNotFound

	}
	if !hasher_verify(password, dbPassword) {
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
	hashed_password := hasher_encode(password, []byte(utils.RandomString(16)))
	_, err = stmt2.Exec(username, hashed_password)
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
