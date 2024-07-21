#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  char* edge = "####################";
  char* empty = "#                  #";
  game_state_t* state = (game_state_t*) malloc(sizeof(game_state_t));
  state->board = malloc(19*sizeof(char*));
  //state->board[19] = '\0';
  state->num_rows = 18;
  for (int i = 1; i < 17; i++) {
    state->board[i] = malloc(21*sizeof(char));
    strcpy(state->board[i], empty);
  }
  for (int i = 0; i < 2; i++) {
    state->board[17*i] = malloc(21*sizeof(char));
    strcpy(state->board[17 * i], edge);
  }
    snake_t *snake = (snake_t*) malloc(sizeof(snake_t));
    snake->tail_row = 2;
    snake->tail_col = 2;
    snake->head_row = 2;
    snake->head_col = 4;
    snake->live = true;
    //state->snakes = malloc(sizeof(snake_t));
    state->snakes = snake;
    state->num_snakes = 1;
    set_board_at(state, 2, 2, 'd');
    set_board_at(state, 2, 3, '>');
    set_board_at(state, 2, 4, 'D');
    set_board_at(state, 2, 9, '*');
    //free(snake);
  return state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i++) {
      free((*state).board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i++) {
      fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  return (c == 'w' || c == 'a' || c == 's' || c == 'd');
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  return (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x');
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  return is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>';
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  if (c == '^') {
    return 'w';
  }
  else if (c == '<') {
    return 'a';
  }
  else if (c == 'v') {
    return 's';
  }
  else if (c == '>') {
    return 'd';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  if (c == 'W') {
    return '^';
  }
  else if (c == 'A') {
    return '<';
  }
  else if (c == 'S') {
    return 'v';
  }
  else if (c == 'D') {
    return '>';
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  }
  else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  }
  else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake = state->snakes[snum];
  unsigned int row = snake.head_row;
  unsigned int col = snake.head_col;
  char head = get_board_at(state, row, col);
  row = get_next_row(row, head); // update col and row to be the next square that 
                                 // the snake is about to move into
  col = get_next_col(col, head);
  return get_board_at(state, row, col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  snake_t* snake = &state->snakes[snum];
  char head = get_board_at(state, snake->head_row, snake->head_col);
  state->board[snake->head_row][snake->head_col] = head_to_body(head);
  snake->head_row = get_next_row(snake->head_row, head); // update row and col
  snake->head_col = get_next_col(snake->head_col, head);
  state->board[snake->head_row][snake->head_col] = head;
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a taiil character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  snake_t* snake = &state->snakes[snum];
  char tail = get_board_at(state, snake->tail_row, snake->tail_col);
  state->board[snake->tail_row][snake->tail_col] = ' ';
  snake->tail_row = get_next_row(snake->tail_row, tail); // updates row and col to the 
                                                       // new tail location
  snake->tail_col = get_next_col(snake->tail_col, tail);
  char replace = body_to_tail(state->board[snake->tail_row][snake->tail_col]);
  state->board[snake->tail_row][snake->tail_col] = replace;
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  for (unsigned int i = 0; i < state->num_snakes; i++) {
    if (next_square(state, i) == '#' || (is_snake(next_square(state, i)))) { // the stuff after || checks if the snake is about to hit the body (but not head or tail) of another snake
        snake_t* snake = &state->snakes[i];
        snake->live = false;
        state->board[snake->head_row][snake->head_col] = 'x';
        /* char* tail = &state->board[snake->tail_row][snake->tail_col];
        unsigned int row = snake->tail_row;
        unsigned int col = snake->tail_col;
        while (*tail != 'x') {
            row = get_next_row(row, *tail); // next row/col
            col = get_next_col(col, *tail);
            *tail = ' ';
            tail = &state->board[row][col];
        } */
    } else if (next_square(state, i) == '*') {
        update_head(state, i);
        add_food(state);
    } /*else if (is_tail(next_square(state, i))) {
        tail
    } need to do smth abt whe next_square is tail because it will simply override tail first*/ else {
        update_tail(state, i);
        update_head(state, i);
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(FILE* fp) {
  // TODO: Implement this function.
  unsigned int row = 0;
  unsigned int col = 0;
  game_state_t* state = calloc(1, sizeof(game_state_t));
  state->board = calloc(1, sizeof(char*));
  state->board[0] = calloc(2, sizeof(char));
  char c;

  while(!feof(fp)) {
      c = (char) fgetc(fp);
      // state->board[row] = realloc(state->board[row], sizeof(char) * (col + 1));
      
      if (c != '\n') {
          state->board[row][col] = c;
          col++;
          state->board[row] = realloc(state->board[row], sizeof(char)*(col + 1));
      } else {
          state->board[row][col] = '\0';
          row++;
          state->board = realloc(state->board, sizeof(char*) * (row + 1));
          state->board[row] = calloc(2, sizeof(char));
          col = 0;
      }
  }
  state->board[row][col] = '\0';
  state->snakes = NULL;
  state->num_rows = row;
  state->num_snakes = 0;
  return state;
}


  /*
  game_state_t* state = (game_state_t*) malloc(sizeof(game_state_t));
  state->board = malloc(sizeof(char*));
  state->board[row] = malloc(sizeof(char));

  while(!feof(fp) && row != 200000 && col != 20000) {

      c = (char) fgetc(fp);

      if (c == '\n' && col != 20000) {
          state->board[row][col] = '\0';
          row++;
          state->board = realloc(state->board, (row) * sizeof(char*));
          state->board[row] = malloc(sizeof(char));
          col = 0;
      } else {
          state->board[row][col] = c;
          col++;
          state->board[row] = realloc(state->board[row], (col) * sizeof(char));
      }
  }
  state->board[row][col] = '\0';
  state->snakes = NULL;
  state->num_rows = row;
  state->num_snakes = 0;
  return state;
  */

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned row = state->snakes[snum].tail_row;
  unsigned col = state->snakes[snum].tail_col;

  char curr = get_board_at(state, row, col);
  while (!is_head(curr)) {
      row = get_next_row(row, curr);
      col = get_next_col(col, curr);
      curr = get_board_at(state, row, col);
  }
  state->snakes[snum].head_row = row;
  state->snakes[snum].head_col = col;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  //snake_t** temp = malloc(sizeof(snake_t*));
  state->snakes = malloc(sizeof(snake_t));
  char c;
  char c2;
  unsigned int snum = 0;
  snake_t* high_snake;
  for (unsigned int i = 0; i < state->num_rows; i++) {
      for (unsigned int j = 0; j < strlen(state->board[i]); j++) {
        c = get_board_at(state, i, j);
        if (is_tail(c)) {
            high_snake = &state->snakes[snum];
            state->snakes[snum].tail_row = i;
            state->snakes[snum].tail_col = j;
            find_head(state, snum);
            c2 = get_board_at(state, state->snakes[snum].head_row, state->snakes[snum].tail_row);
            if (c2 == 'x') {
                state->snakes[snum].live = false;
            }
            snum++;
            state->snakes = realloc(state->snakes, (snum + 1) * sizeof(snake_t));
        }
      }
  }
  state->num_snakes = snum;
  return state;
 }
