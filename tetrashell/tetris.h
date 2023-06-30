#ifndef TETRIS_H
#define TETRIS_H
// We'll use this file to contain commonly-accessed struct definitions from
// tetris.c.
#include <curses.h>

// The width and height of the area where the blocks go, in blocks rather than
// characters.
#define BLOCKS_WIDE (10)
#define BLOCKS_TALL (20)

// This is the y position a piece spawns at when entering the board.
#define PIECE_START_Y (-1)

// This struct keeps track of the tetris display windows from ncurses.
typedef struct {
  // The top-level curses "window", containing all of the other windows.
  WINDOW *top_window;
  // The "game" area, where the pieces are dropped and moved around.
  // Should be 20 characters wide (for 10 blocks) and 20 character tall (for
  // 10 blocks), plus one char of padding around the border; a total of 22x22
  // characters.
  WINDOW *game;
  // The window showing only the player's current score.
  WINDOW *score;
  // The window showing the number of lines the player has completed.
  WINDOW *line_count;
  // The window showing a preview of the next piece.
  WINDOW *next_piece;
  // The status message to be displayed. strlen(status_message) must be 0 if no
  // message should be displayed.
  char status_message[40];
  // The time at which the current status message was first displayed. Used so
  // we can automatically clear the status message after a certain amount of
  // time has elapsed.
  double status_start_time;
} TetrisDisplay;

// This holds everything we need to know about the current state of an ongoing
// game.
typedef struct {
  // Holds the state of the entire board. If a cell is empty, it must contain a
  // space character. It is invalid for a cell to contain a nonprintable
  // character or whitespace other than ' '. In any case except for ' ', the
  // cell is occupied, and this must contain the character to draw to the
  // screen when drawing the cell. Note that cells are drawn as two characters
  // wide, so each character in this array will be printed twice when drawing
  // the board. This starts from the TOP row in the window that's displayed.
  char board[BLOCKS_WIDE * BLOCKS_TALL];
  // The ID of the next piece that will be generated; the index into the
  // tetris_pieces array.
  short next_piece;
  // The x and y location of the current piece being dropped into the board, in
  // a cell coordinate rather than a window character.  Note that these
  // coordinates refer to the *bottom left* of the falling piece on the board!
  int location_x;
  int location_y;
  // The piece that is currently "falling". Once again, it's an index into the
  // tetris_pieces array.
  short current_piece;
  // The player's current score.
  unsigned score;
  // The number of lines the player has completed.
  unsigned lines;
} TetrisGameState;

// We statically define each piece as a 16-byte strings here. Note that when
// the game is being rendered, the widths of these will be doubled. This
// contains all pieces along with their rotations. There are 19 of them. Note
// that the pieces in this array are "upside down", with the bottom row first.
static const char *tetris_pieces[] = {
  // 0:
  "===="
  "    "
  "    "
  "    ",
  // 1:
  "=   "
  "=   "
  "=   "
  "=   ",
  // 2:
  "HH  "
  "HH  "
  "    "
  "    ",
  // 3:
  "N   "
  "NN  "
  " N  "
  "    ",
  // 4:
  " NN "
  "NN  "
  "    "
  "    ",
  // 5:
  " Z  "
  "ZZ  "
  "Z   "
  "    ",
  // 6:
  "ZZ  "
  " ZZ "
  "    "
  "    ",
  // 7:
  " #  "
  "### "
  "    "
  "    ",
  // 8:
  "#   "
  "##  "
  "#   "
  "    ",
  // 9:
  "### "
  " #  "
  "    "
  "    ",
  // 10:
  " #  "
  "##  "
  " #  "
  "    ",
  // 11:
  "@   "
  "@   "
  "@@  "
  "    ",
  // 12:
  "@@@ "
  "@   "
  "    "
  "    ",
  // 13:
  "@@  "
  " @  "
  " @  "
  "    ",
  // 14:
  "  @ "
  "@@@ "
  "    "
  "    ",
  // 15:
  " O  "
  " O  "
  "OO  "
  "    ",
  // 16:
  "O   "
  "OOO "
  "    "
  "    ",
  // 17:
  "OO  "
  "O   "
  "O   "
  "    ",
  // 18:
  "OOO "
  "  O "
  "    "
  "    ",
};

// This is how we look up rotations. If the current piece is at index i in the
// tetris_pieces array, then piece_rotations[i] gives the index of its next
// rotation in the tetris_pieces array.
static char piece_rotations[] = {1, 0, 2, 4, 3, 6, 5, 8, 9, 10, 7, 12, 13, 14,
  11, 16, 17, 18, 15};

#endif  // TETRIS_H

