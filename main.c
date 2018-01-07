/*
  I've made this game as sort of a joke.
  The rules are as follows:
    make a fully functional 2-player tic tac toe game with a user friendly interface
    use only a single 32 bit number
    no other variables
    no return values
    no parameters
    user made functions are ok, but may not call subsequent user made functions
      i.e. stack level never goes further than 1 past main
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

static void display_prompt(void);
static void display_board(void);
static void game_eval(void);
static void game_turn(void);

/*
  Tic Tac Toe has 9 square. The squares can be either X, O, or Not Set.
  If we represent each square with 2 bits, we can fit the entire board into
  18 bits of data. Where each 2-bit structure is:
    00 => Not set
    01 => X
    10 => O
    11 => Invalid

  We need to keep track of the player's turn. This can be done entirely with
  1 bit.
*/
static union {
  struct {
    /*
      The character buffer is used to read from the console.
    */
    uint32_t char_buffer : 8;
    /*
      The Turn counter will be used to detect draws... if there is no winner,
      and the value is 9, a draw has occurred
    */
    uint32_t turn_count : 4;
    /*
      The actual game board will look like this:
        bit 17 and 16 --> xx xx xx <--bit 13 and 12
        bit 11 and 10 --> xx xx xx <--bit 7 and 6
        bit 5 and 4   --> xx xx xx <--bit 1 and 0
    */
    uint32_t board : 18;
    /*
      Player turn gets flipped every turn except for the game-ending turn. It is also
      used to determine which player won the game... assuming someone won.
    */
    uint32_t player_turn : 1;
    uint32_t win_flag : 1;
  };
  uint32_t val;
} the_game;

static void display_board(void) {
  /*
    top left square
    0x3   0     0    0   0
    xx11 0000 0000 0000 0000
  */
  if ((the_game.board & 0x30000) == 0x20000) { //player O
    (void)putchar('O');
  } else if ((the_game.board & 0x30000) == 0x10000) { //player x
    (void)putchar('X');
  } else { //empty
    (void)putchar('8');
  }

  /*
    top middle square
    0x0    C    0    0    0
    xx00 1100 0000 0000 0000
  */
  if ((the_game.board & 0x0C000) == 0x08000) { //player O
    (void)putchar('O');
  } else if ((the_game.board & 0x0C000) == 0x04000) { //player x
    (void)putchar('X');
  } else { //empty
    (void)putchar('7');
  }

  /*
    top right square
  */
  //0    3    0    0    0
  //xx00 0011 0000 0000 0000
  if ((the_game.board & 0x03000)== 0x02000) { //player O
    (void)putchar('O');
  } else if ((the_game.board & 0x03000) == 0x01000) { //player x
    (void)putchar('X');
  } else { //empty
    (void)putchar('6');
  }

  /*****
    Next line
  *****/
  (void)putchar('\n');

  /*
    Middle Left square
  */
  //0     0    C    0    0
  //xx00 0000 1100 0000 0000
  if ((the_game.board & 0x00C00) == 0x00800) { //player O
    (void)putchar('O');
  } else if ((the_game.board & 0x00C00) == 0x00400) { //player x
    (void)putchar('X');
  } else { //empty
    (void)putchar('5');
  }

  /*
    Middle middle square
  */
  //0    0     3    0    0
  //xx00 0000 0011 0000 0000
  if ((the_game.board & 0x00300)== 0x00200) { //player O
    (void)putchar('O');
  } else if ((the_game.board & 0x00300) == 0x00100) { //player x
    (void)putchar('X');
  } else { //empty
    (void)putchar('4');
  }

  /*
    Middle right square
    xx00 0000 0000 1100 0000
  */
  if ((the_game.board & 0x000C0) == 0x00080) { //player O
    (void)putchar('O');
  } else if ((the_game.board & 0x000C0) == 0x00040) { //player x
    (void)putchar('X');
  } else { //empty
    (void)putchar('3');
  }

  /*****
    Next line
  *****/
  (void)putchar('\n');

  /*
    Bottom Left square
    xx00 0000 0000 0011 0000
  */
  if ((the_game.board & 0x00030) == 0x00020) { //player O
    (void)putchar('O');
  } else if ((the_game.board & 0x00030) == 0x00010) { //player x
    (void)putchar('X');
  } else { //empty
    (void)putchar('2');
  }

  /*
    Bottom Middle square
    xx00 0000 0000 0000 1100
  */
  if ((the_game.board & 0x0000C) == 0x00008) { //player O
    (void)putchar('O');
  } else if ((the_game.board & 0x0000C) == 0x00004) { //player x
    (void)putchar('X');
  } else { //empty
    (void)putchar('1');
  }

  /*
    bottom right square
    xx00 0000 0000 0000 0011
  */
  if ((the_game.board & 0x00003) == 0x00002) { //player O
    (void)putchar('O');
  } else if ((the_game.board & 0x00003) == 0x00001) { //player x
    (void)putchar('X');
  } else { //empty
    (void)putchar('0');
  }
  (void)putchar('\n');
}

static void display_prompt() {
  printf("It is player ");
  if (the_game.player_turn == 1) {
    (void)putchar('O');
  } else {
    (void)putchar('X');
  }
  printf("'s turn\n");

}

/*
  The board is logically laid as this grid
    8 7 6
    5 4 3
    2 1 0
*/
static void game_eval() {
  //xx 01 00 00
  //   01 00 00
  //   01 00 00
  #define VERTICAL_MASK ((uint32_t)0x10410)
  //xx 01 01 01
  //   00 00 00
  //   00 00 00
  #define HORIZONTAL_MASK ((uint32_t)0x15000)
  //xx 01 00 00
  //   00 01 00
  //   00 00 01
  #define DIAG_TL2BR ((uint32_t)0x10101)
  //xx 00 00 01
  //   00 01 00
  //   01 00 00
  #define DIAG_BL2TR ((uint32_t)0x01110)

  #define TRANSPOSE_MASK(mask, turn) ((mask) << turn)
  ////////vertical pass////////
  //check left most
  if ((the_game.board & TRANSPOSE_MASK(VERTICAL_MASK, the_game.player_turn)) == TRANSPOSE_MASK(VERTICAL_MASK, the_game.player_turn)) {
    the_game.win_flag = 1;
    return;
  }
  //check middle
  if ((the_game.board & TRANSPOSE_MASK(VERTICAL_MASK >> 2u, the_game.player_turn)) == TRANSPOSE_MASK(VERTICAL_MASK >> 2u, the_game.player_turn)) {
    the_game.win_flag = 1;
    return;
  }
  //check right
  if ((the_game.board & TRANSPOSE_MASK(VERTICAL_MASK >> 4u, the_game.player_turn)) == TRANSPOSE_MASK(VERTICAL_MASK >> 4u, the_game.player_turn)) {
    the_game.win_flag = 1;
    return;
  }
  if ((the_game.board & TRANSPOSE_MASK(VERTICAL_MASK, the_game.player_turn)) == TRANSPOSE_MASK(VERTICAL_MASK, the_game.player_turn)) {
    the_game.win_flag = 1;
    return;
  }
  ////////horizontal pass////////
  //check top
  if ((the_game.board & TRANSPOSE_MASK(HORIZONTAL_MASK, the_game.player_turn)) == TRANSPOSE_MASK(HORIZONTAL_MASK, the_game.player_turn)) {
    the_game.win_flag = 1;
    return;
  }
  //check middle
  if ((the_game.board & TRANSPOSE_MASK(HORIZONTAL_MASK >> 6, the_game.player_turn)) == TRANSPOSE_MASK(HORIZONTAL_MASK >> 6, the_game.player_turn)) {
    the_game.win_flag = 1;
    return;
  }
  //check bottom
  if ((the_game.board & TRANSPOSE_MASK(HORIZONTAL_MASK >> 12, the_game.player_turn)) == TRANSPOSE_MASK(HORIZONTAL_MASK >> 12, the_game.player_turn)) {
    the_game.win_flag = 1;
    return;
  }
  ////////diagonal pass////////
  //top left to bottom right
  if ((the_game.board & TRANSPOSE_MASK(DIAG_TL2BR, the_game.player_turn)) == TRANSPOSE_MASK(DIAG_TL2BR, the_game.player_turn)) {
    the_game.win_flag = 1;
    return;
  }
  //bottom left to top right
  if ((the_game.board & TRANSPOSE_MASK(DIAG_BL2TR, the_game.player_turn)) == TRANSPOSE_MASK(DIAG_BL2TR, the_game.player_turn)) {
    the_game.win_flag = 1;
    return;
  }
}

static void game_turn() {
try_again:
  the_game.char_buffer = (uint32_t) getchar();
  switch (the_game.char_buffer) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
      the_game.char_buffer -= (uint32_t)'0';
      if ((the_game.board & (0x03 << (the_game.char_buffer*2))) == 0) {
        the_game.board |= ((the_game.player_turn == 1)? 0x02u: 0x01u) << (the_game.char_buffer*2);
      } else {
        printf("The square <%d> has alrady been claimed\n", (int)the_game.char_buffer);
        goto try_again;
      }
      break;
    case '\n':
    case '\r':
      /* skip white space character */
      goto try_again;
    case 0xFF:
      printf("\nUser terminated game\n");
      exit(EXIT_FAILURE);
    default:
      printf("Invalid character <%c>\n", (char) the_game.char_buffer);
      goto try_again;
  }
}

int main () {
  the_game.val = 0;
loop:
  display_board();
  display_prompt();
  game_turn();
  game_eval();
  if (the_game.win_flag == 1) {
    printf("Player %c won!!!\n", (the_game.player_turn == 1)? 'O' : 'X');
    goto end_of_game;
  } else if (the_game.turn_count >= 8) {
    printf("DRAW!!!\n");
    goto end_of_game;
  } else {
    the_game.turn_count++;
    the_game.player_turn ^= 1;
    goto loop;
  }
end_of_game:
  display_board();
  printf("\n\nGAME OVER\n\n");
  return EXIT_SUCCESS;
}
