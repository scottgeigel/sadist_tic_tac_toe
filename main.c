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

/*
  The game structure (AKA the only variable)
  [      8 bits    ]  [3 bits]  [   2 bits  ]  [   1 bit   ]  [18 bits]
  <character buffer>  <unused>  <player swap>  <Player Turn>  < Board >
*/
static uint32_t the_game;

/*
  The character buffer is used to read from the console. It is conveniently the
  first byte in the structure for ease of access.
*/
#define GET_GAME_CHARBUF() ((the_game & 0xFF000000) >> 24)
#define SET_GAME_CHARBUF(new_val) (\
  the_game = \
  ( \
    (the_game & 0x00FFFFFF) | (((uint32_t) new_val) << 24) \
  ) \
)

/*
  Unused is for padding
*/
#define __UNUSED_BITS__ 0x00E00000

/*
  Player swap is used in the game evaluation function. It is used to compare
  whether the 3 in a row match or not.
*/
//  0   0   1     8   0     0   0     0
//0000 0000 0001 1000 0000 0000 0000 0000
#define GET_GAME_PLAYERSWAP() ((the_game & 0x00180000) >> 19)
//  F   F   E     7   F     F   F     F
//1111 1111 1110 0111 1111 1111 1111 1111
#define SET_GAME_PLAYERSWAP(new_val) (\
  the_game = (\
    (the_game & 0xFFE7FFFF) | (new_val << 19) \
  ) \
)

/*
  Player turn gets flipped every turn except for the game-ending turn. It is also
  used to determine which player won the game... assuming someone won.

  The actual game board will look like this:
    bit 17 and 16 --> xx xx xx <--bit 13 and 12
    bit 11 and 10 --> xx xx xx <--bit 7 and 6
    bit 5 and 4   --> xx xx xx <--bit 1 and 0
*/

//  0   0       0   3       F   F       F   F
//0000 0000   0000 0011   1111 1111   1111 1111
#define MASK_BOARD_MASK 0x0003FF
#define MASK_BOARD() (the_game & MASK_BOARD_MASK)

/*
  Pursuant to the bit map of the board, this macro will index the board as so
    8 7 6
    5 4 3
    2 1 0

  all other values are not available
*/
#define GET_BOARD_TILE(num) (MASK_BOARD() & (MASK_SQUARE_MASK << (num * 2))) >> (num * 2)
// void game_eval() {
//   //check the square in the middle
//
//   if (MASK_BOARD() &)
// }

int main () {
  the_game = 0;
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

  return EXIT_SUCCESS;
}
