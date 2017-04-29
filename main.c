#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/*
  Tic Tac Toe has 9 square. The squares can be either X, O, or Not Set.
  If we represent each square with 2 bits, we can fit the entire board into
  18 bits of data. Where each 2-bit structure is:
    00 => Not set
    01 => X
    10 => O
    11 => Invalid
*/
#define MASK_SQUARE_X 0x01
#define MASK_SQUARE_O 0x02
#define MASK_SQUARE_EMPTY 0x00
#define MASK_SQUARE_MASK 0x03

/*
  We need to keep track of the player's turn. This can be done entirely with
  1 bit.
*/
#define MASK_TURN_X 0x00
#define MASK_TURN_O 0x01

union {
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

//  0   0     0   4     0   0     0   0
//0000 0000 0000 0100 0000 0000 0000 0000
void display_prompt() {
  printf("It is player ");
  if (the_game.player_turn) {
    putchar('O');
  } else {
    putchar('X');
  }
  printf("'s turn\n");
}

void next_turn() {
  the_game.player_turn ^= 1;
}


//  0   0       0   3       F   F       F   F
//0000 0000   0000 0011   1111 1111   1111 1111
#define MASK_BOARD_MASK 0x0003FF
#define MASK_BOARD() (the_game & MASK_BOARD_MASK)

/*
  The board is logically laid as this grid
    8 7 6
    5 4 3
    2 1 0
*/
void game_eval() {
  //xx 11 00 00
  //   11 00 00
  //   11 00 00
  #define VERTICAL_MASK 0x30C30
  //xx 01 00 00
  //   01 00 00
  //   01 00 00
  #define VERTICAL_X_WIN 0x10410
  //xx 10 00 00
  //   10 00 00
  //   10 00 00
  #define VERTICAL_O_WIN 0x20820

  //xx 01 11 11
  //   00 00 00
  //   00 00 00
  #define HORIZONTAL_MASK 0x1F000
  //xx 01 00 00
  //   00 01 00
  //   00 00 01
  #define DIAG_TL2BR 0x10101

  //xx 00 00 01
  //   00 01 00
  //   01 00 00
  #define DIAG_BL2TR 0x01110

  if ((the_game.board & VERTICAL_MASK) == (VERTICAL_X_WIN << (the_game.player_turn))) {
    the_game.win_flag = 1;
  }
  // if (the_game.player_turn == 0) {
  //   if ((the_game.board & VERTICAL_MASK) == VERTICAL_X_WIN) {
  //     the_game.win_flag = 1;
  //   }
  // } else {
  //   if ((the_game.board & VERTICAL_MASK) == VERTICAL_O_WIN) {
  //     the_game.win_flag = 1;
  //   }
  // }
  // ////////vertical pass////////
  // //check left most
  // if ((the_game.board & TRANSPOSE_MASK(VERTICAL_MASK, the_game.player_turn)) == TRANSPOSE_MASK(VERTICAL_MASK, the_game.player_turn)) {
  //   the_game.win_flag = 1;
  //   return;
  // }
  // //check middle
  // if ((the_game.board & TRANSPOSE_MASK(VERTICAL_MASK >> 2, the_game.player_turn)) == TRANSPOSE_MASK(VERTICAL_MASK >> 2, the_game.player_turn)) {
  //   the_game.win_flag = 1;
  //   return;
  // }
  // //check right
  // if ((the_game.board & TRANSPOSE_MASK(VERTICAL_MASK >> 4, the_game.player_turn)) == TRANSPOSE_MASK(VERTICAL_MASK >> 4, the_game.player_turn)) {
  //   the_game.win_flag = 1;
  //   return;
  // }
  // if ((the_game.board & TRANSPOSE_MASK(VERTICAL_MASK, the_game.player_turn)) == TRANSPOSE_MASK(VERTICAL_MASK, the_game.player_turn)) {
  //   the_game.win_flag = 1;
  //   return;
  // }
  // ////////horizontal pass////////
  // //check top
  // if ((the_game.board & TRANSPOSE_MASK(HORIZONTAL_MASK, the_game.player_turn)) == TRANSPOSE_MASK(HORIZONTAL_MASK, the_game.player_turn)) {
  //   the_game.win_flag = 1;
  //   return;
  // }
  // //check middle
  // if ((the_game.board & TRANSPOSE_MASK(HORIZONTAL_MASK >> 6, the_game.player_turn)) == TRANSPOSE_MASK(HORIZONTAL_MASK >> 6, the_game.player_turn)) {
  //   the_game.win_flag = 1;
  //   return;
  // }
  // //check bottom
  // if ((the_game.board & TRANSPOSE_MASK(HORIZONTAL_MASK >> 12, the_game.player_turn)) == TRANSPOSE_MASK(HORIZONTAL_MASK >> 12, the_game.player_turn)) {
  //   the_game.win_flag = 1;
  //   return;
  // }
  // ////////diagonal pass////////
  // //top left to bottom right
  // if ((the_game.board & TRANSPOSE_MASK(DIAG_TL2BR, the_game.player_turn)) == TRANSPOSE_MASK(DIAG_TL2BR, the_game.player_turn)) {
  //   the_game.win_flag = 1;
  //   return;
  // }
  // //bottom left to top right
  // if ((the_game.board & TRANSPOSE_MASK(DIAG_BL2TR, the_game.player_turn)) == TRANSPOSE_MASK(DIAG_BL2TR, the_game.player_turn)) {
  //   the_game.win_flag = 1;
  //   return;
  // }
}

void game_turn() {
try_again:
  the_game.char_buffer = getchar();
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
      the_game.char_buffer -= '0';
      printf("trying %d %#X %#X\n", the_game.char_buffer, 0x03 << (the_game.char_buffer*2), ((the_game.player_turn)? 0x02: 0x01) << (the_game.char_buffer*2));
      if ((the_game.board & (0x03 << (the_game.char_buffer*2))) == 0) {
        the_game.board |= ((the_game.player_turn)? 0x02: 0x01) << (the_game.char_buffer*2);
      } else {
        printf("The square <%d> has alrady been claimed\n", the_game.char_buffer);
        goto try_again;
      }
      break;
    case '\n':
    case '\r':
      /* skip white space character */
      goto try_again;
      break;
    case 0xFF:
      printf("\nUser terminated game\n");
      exit(EXIT_FAILURE);
    default:
      printf("Invalid character <%c>\n", the_game.char_buffer);
      goto try_again;
      break;
  }
}

//TODO: cleanup comments
void display_board() {
  //xx11 0000 0000 0000 0000 => 8
  //xx00 1100 0000 0000 0000 => 7
  //xx00 0011 0000 0000 0000 => 6
  //
  //xx00 0000 1100 0000 0000 => 5
  //xx00 0000 0011 0000 0000 => 4
  //xx00 0000 0000 1100 0000 => 3
  //
  //xx00 0000 0000 0011 0000 => 2
  //xx00 0000 0000 0000 1100 => 1
  //xx00 0000 0000 0000 0011 => 0
  /*
    top left square
    0x3   0     0    0   0
    xx11 0000 0000 0000 0000
  */
  printf("DEBUG %#X => %#X\n", the_game, the_game.board);
  if ((the_game.board & 0x30000) == 0x20000) { //player O
    putchar('O');
  } else if ((the_game.board & 0x30000) == 0x10000) { //player x
    putchar('X');
  } else { //empty
    putchar('8');
  }

  /*
    top middle square
    0x0    C    0    0    0
    xx00 1100 0000 0000 0000
  */
  if ((the_game.board & 0x0C000) == 0x08000) { //player O
    putchar('O');
  } else if ((the_game.board & 0x0C000) == 0x04000) { //player x
    putchar('X');
  } else { //empty
    putchar('7');
  }

  /*
    top right square
  */
  //0    3    0    0    0
  //xx00 0011 0000 0000 0000
  if ((the_game.board & 0x03000)== 0x02000) { //player O
    putchar('O');
  } else if ((the_game.board & 0x03000) == 0x01000) { //player x
    putchar('X');
  } else { //empty
    putchar('6');
  }

  /*****
    Next line
  *****/
  putchar('\n');

  /*
    Middle Left square
  */
  //0     0    C    0    0
  //xx00 0000 1100 0000 0000
  if ((the_game.board & 0x00C00) == 0x00800) { //player O
    putchar('O');
  } else if ((the_game.board & 0x00C00) == 0x00400) { //player x
    putchar('X');
  } else { //empty
    putchar('5');
  }

  /*
    Middle middle square
  */
  //0    0     3    0    0
  //xx00 0000 0011 0000 0000
  if ((the_game.board & 0x00300)== 0x00200) { //player O
    putchar('O');
  } else if ((the_game.board & 0x00300) == 0x00100) { //player x
    putchar('X');
  } else { //empty
    putchar('4');
  }

  /*
    Middle right square
    xx00 0000 0000 1100 0000
  */
  if ((the_game.board & 0x000C0) == 0x00080) { //player O
    putchar('O');
  } else if ((the_game.board & 0x000C0) == 0x00040) { //player x
    putchar('X');
  } else { //empty
    putchar('3');
  }

  /*****
    Next line
  *****/
  putchar('\n');

  /*
    Bottom Left square
    xx00 0000 0000 0011 0000
  */
  if ((the_game.board & 0x00030) == 0x00020) { //player O
    putchar('O');
  } else if ((the_game.board & 0x00030) == 0x00010) { //player x
    putchar('X');
  } else { //empty
    putchar('2');
  }

  /*
    Bottom Middle square
    xx00 0000 0000 0000 1100
  */
  if ((the_game.board & 0x0000C) == 0x00008) { //player O
    putchar('O');
  } else if ((the_game.board & 0x0000C) == 0x00004) { //player x
    putchar('X');
  } else { //empty
    putchar('1');
  }

  /*
    bottom right square
    xx00 0000 0000 0000 0011
  */
  if ((the_game.board & 0x00003) == 0x00002) { //player O
    putchar('O');
  } else if ((the_game.board & 0x00003) == 0x00001) { //player x
    putchar('X');
  } else { //empty
    putchar('0');
  }
  putchar('\n');
}

int main () {
  the_game.val = 0;
loop:
  display_board();
  display_prompt();
  game_turn();
  game_eval();
  if (the_game.win_flag) {
    printf("Player %c won!!!\n", (the_game.player_turn)? 'O' : 'X');
    goto end_of_game;
  } else if (the_game.turn_count >= 9) {
    printf("DRAW!!!");
    goto end_of_game;
  } else {
    the_game.turn_count++;
    the_game.player_turn ^= 1;
    goto loop;
  }
end_of_game:
  printf("\n\nGAME OVER\n\n");
#ifndef TEST_MODE
#else
  printf("Testing functionality of get buf and set buf\n");
  printf("\t<0x%8.8X> Char Buf is now <0x%2.2X>\n", the_game, GET_GAME_CHARBUF());
  printf("\tType a character > ");
  SET_GAME_CHARBUF(getchar());
  printf("\t<0x%8.8X> Char Buf is now <0x%2.2X>\n", the_game, GET_GAME_CHARBUF());

  printf("\n\n");

  printf("Testing functionality of player swap\n");
  printf("\t<0x%8.8X> Player Swap is now <0x%2.2X>\n", the_game, GET_GAME_PLAYERSWAP());
  printf("\tSetting player to X\n");
  SET_GAME_PLAYERSWAP(MASK_SQUARE_X);
  printf("\t<0x%8.8X> Player Swap is now <0x%2.2X>\n\n", the_game, GET_GAME_PLAYERSWAP());
  printf("\tSetting player to O\n");
  SET_GAME_PLAYERSWAP(MASK_SQUARE_O);
  printf("\t<0x%8.8X> Player Swap is now <0x%2.2X>\n", the_game, GET_GAME_PLAYERSWAP());

  printf("\n\n");

  printf("Testing Player Turn\n");
  printf("\t<0x%8.8X>", the_game); display_prompt(); printf("\n");
  next_turn();
  printf("\t<0x%8.8X>", the_game); display_prompt(); printf("\n");
  next_turn();
  printf("\t<0x%8.8X>", the_game); display_prompt(); printf("\n");
#endif
  return EXIT_SUCCESS;
}
