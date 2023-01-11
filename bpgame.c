/*
Author: Harith Patel UIC Fall 2021

Description: This program creates a command line pop it game.

*/

// file:  bpgame.c


/** #include statements... **/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "bpgame.h"

/** TYPEDEFS / STRUCTs HERE ***/
struct bpgame {
   int score;
   char board[MAX_ROWS][MAX_COLS];
   int rows;
   int cols;
   struct stack * moves;
   // YOU DECIDE WHAT TO PUT IN HERE TO CAPTURE THE STATE OF
   //   A "RUN" OF THE GAME.....
};

struct stack {
   int top;
   int cap;
   BPGame * board_states;
};


/*** IMPLEMENTATION OF bp_XXXX FUNCTIONS HERE  ****/

int indexOf(char color, char array[], int size) {
   /* base case */
   int found = -1;

   for(int i = 0; i < 4; i++) {
      /* check if char is at index i in array */
      if(color == array[i]) {
         /* stop loop if char is found in array */
         found = i;
         break;
      } 
   }
   return found;
}

void resize(BPGame *b) {
   BPGame *temp = b->moves->board_states;
   /* double the memory for the array */
   b->moves->board_states = (BPGame*) malloc(sizeof(BPGame) * b->moves->cap * 2);
   
   /* add instances of bpgame struct into new array */
   for(int i = 0; i < b->moves->cap; i++) {
      b->moves->board_states[i] = temp[i];
   }

   free(temp);
   b->moves->cap = b->moves->cap * 2;
}

void push(BPGame *b) {
   int rows = b->rows;
   int cols = b->cols;

   /* check if the stack is full to resize it */
   if (b->moves->top == b->moves->cap) 
      resize(b); 

   /* adds current score and board into array */
   b->moves->board_states[b->moves->top].score = b->score;
   for(int r = 0; r < rows; r++) {
      for(int c = 0; c < cols; c++) {
         b->moves->board_states[b->moves->top].board[r][c] = b->board[r][c];
      }
   }
   b->moves->top = b->moves->top + 1;
}


void init_stack(BPGame *b) {
   /* allocate memory for stack */
   b->moves = (struct stack *) malloc(sizeof(struct stack));
   b->moves->top = 0;
   b->moves->cap = 10;
   /* allocate memory to store bpgame struct in stack */
   b->moves->board_states = (BPGame*) malloc(sizeof(BPGame) * b->moves->cap);
}


void pop(BPGame *b) {
   int rows = b->rows;
   int cols = b->cols;
   b->moves->top = b->moves->top - 1;
   /* set current score to last score in stack */
   b->score = b->moves->board_states[b->moves->top].score;
   /* set board to last board in stack */
   for(int r = 0; r < rows; r++) {
      for(int c = 0; c < cols; c++) {
         b->board[r][c] = b->moves->board_states[b->moves->top].board[r][c];
      }
   }
}


BPGame * bp_create(int nrows, int ncols) {
   srand(time(NULL));
   char colors[4] = {Red, Green, Yellow, Blue};
   int r , c;

   /* check if the rows and cols are in range */
   if(nrows < 0 || ncols < 0 || nrows > MAX_ROWS || ncols > MAX_COLS) {
      fprintf(stderr, "rows or cols out of range");
      return NULL;
   }

   /* create a bpgame struct */
   BPGame *b = (BPGame *) malloc(sizeof(BPGame));

   b->score = 0;
   b->rows = nrows;
   b->cols =  ncols;
   init_stack(b);

   /* add random baloon colors into board */
   for(r = 0; r < nrows; r++) {
      for(c = 0; c < ncols; c++) {
         b->board[r][c] = colors[rand() % 4];
      }
   }
   return b;
}

BPGame * bp_create_from_mtx(char mtx[][MAX_COLS], int nrows, int ncols) {
   char colors[] = {Red, Green, Yellow, Blue};
   int r , c;

   /* check if the rows and cols are in range */
   if(nrows < 0 || ncols < 0 || nrows > MAX_ROWS || ncols > MAX_COLS) {
      fprintf(stderr, "rows or cols out of range");
      return NULL;
   }
   
   BPGame *b = (BPGame *) malloc(sizeof(BPGame));

   b->score = 0;
   b->rows = nrows;
   b->cols =  ncols;
   init_stack(b);

   for(r = 0; r < nrows; r++) {
      for(c = 0; c < ncols; c++) {
         /* check if color is a valid color */
         if(indexOf(mtx[r][c], colors, 4) == -1) {
            fprintf(stderr, "%c is not a valid color", mtx[r][c]);
            return NULL;
         }
         /* populate board with color */
         b->board[r][c] = mtx[r][c];
      }
   }
   return b;
}

void bp_destroy(BPGame *b) {
   int rows = b->rows;
   int r;

   /* free memory allocated to the arrays and structs */
   free(b->moves->board_states);
   free(b->moves);
   free(b);
}

void bp_display(BPGame *b) {
   int rows = b->rows;
   int cols = b->cols;
   int r , c;
   if(rows < 0 || cols < 0) return;

   printf("\n   ");
   /* print column numbers above each column in the board */
   for(int c = 0; c < cols; c++)
      printf("%d", c / 10);

   printf("\n   ");
   for(int c = 0; c < cols; c++)
      printf("%d", c % 10);

   printf("\n  +");
   for(c = 0; c < cols; c++) 
      printf("-");
   printf("+\n");

   for(r = 0; r < rows; r++) {
      /* print row numbers next to each row in the board */
      if(r < 10)
         printf("%d |", r);
      else
         printf("%d|", r);

      /* print the board */
      for(c = 0; c < cols; c++) 
         printf("%c", b->board[r][c]);

      printf("|\n");
   }

   printf("  +");
   for(c = 0; c < cols; c++)
      printf("-");
   printf("+\n");
}

int find_clusters(BPGame * b, int r, int c, char baloon) {
   int popped = 0;
   int friends;
   /* check if the checked position is in range */
   if(c - 1 > -1)
      /* check if the position is contains the same baloon color pop it if it does */
      if(b->board[r][c-1] == baloon){
         b->board[r][c-1] = None;
         popped++;
         friends = find_clusters(b, r, c - 1, baloon);
         popped = popped + friends;
      }
   
   /* check if the checked position is in range */
   if(r - 1 > -1)
      /* check if the position is contains the same baloon color pop it if it does */
      if(b->board[r-1][c] == baloon){
         b->board[r-1][c] = None;
         popped++;
         friends = find_clusters(b, r-1, c, baloon);
         popped = popped + friends;
      }
   
   /* check if the checked position is in range */
   if(r + 1 < b->rows)
      /* check if the position is contains the same baloon color pop it if it does */
      if(b->board[r+1][c] == baloon) {
         b->board[r+1][c] = None;
         popped++;
         friends = find_clusters(b, r + 1, c, baloon);
         popped = popped + friends;
      }
   
   /* check if the checked position is in range */
   if(c + 1 < b->cols)
      /* check if the position is contains the same baloon color pop it if it does */
      if(b->board[r][c+1] == baloon) {
         b->board[r][c+1] = None;
         popped++;
         friends = find_clusters(b, r, c + 1, baloon);
         popped = popped + friends;
      }

   return popped;
}

int bp_pop(BPGame * b, int r, int c) {
   if (r >= b->rows || r < 0 || c < 0 || c >= b->cols) return 0;
   if(b->board[r][c] == None) return 0;
   char baloon = b->board[r][c];
   int passed = 0;
   /* if it is a vlid pop push current board into stack */
   if (passed == 0 && c - 1 > -1) {
      if(b->board[r][c-1] == baloon) {
         push(b);
         passed = 1;
      }
   } 

   if (passed == 0 && r - 1 > -1) {
      if(b->board[r-1][c] == baloon) {
         push(b);
         passed = 1;
      }
   } 

   if (passed == 0 && r + 1 < b->rows) {
      if (b->board[r+1][c] == baloon) {
         push(b);
         passed = 1;
      }
   } 

   if (passed == 0 && c + 1 < b->cols) {
      if(b->board[r][c+1] == baloon) {
         push(b);
         passed = 1;
      }
   }
   
   
   int total_popped = find_clusters(b, r, c, baloon);

   if(total_popped > 0) {
      b->board[r][c] = None;
      /* change the score if baloons are popped */
      b->score = b->score + total_popped * (total_popped-1);
   }
   return total_popped;
}


int bp_is_compact(BPGame * b) {
   int rows = b->rows;
   int cols = b->cols;
   int air;
   
   for(int c = 0; c < cols; c++) {
      air = 0;
      for(int r = 0; r < rows; r++) {
         /* check if current positon is air */
         if(b->board[r][c] == None) {
            air = 1;
         }
         /* check if there is baloons below any air in column */
         if(air == 1 && b->board[r][c] != None) {
            return 0;
         }
      }
   }
   return 1;
}

void bp_float_one_step(BPGame * b) {  
   int rows = b->rows;
   int cols = b->cols;
   for(int c = 0; c < cols; c++) {
      for(int r = 0; r < rows - 1; r++) {
         if(b->board[r][c] != None) 
            continue;
         
         int below_pos = r + 1;
         /* move air to the bottom of the column */
         while (below_pos < rows) {
            /* move al baloons up one position */
            b->board[below_pos - 1][c] = b->board[below_pos][c];
            below_pos++;
         }
         b->board[below_pos - 1][c] = None;
         r = rows;
        }
   }
   return;
}

int bp_get_balloon(BPGame * b, int r, int c) {
   /* check if r and c are in range */
   if(c >= b->cols || r > b->rows || c < 0 || r < 0) return -1;
   return b->board[r][c]; 
}

int bp_can_pop(BPGame * b) {
   int rows = b->rows;
   int cols = b->cols;
   for(int r = 0; r < rows; r++) {
      for(int c = 0; c < cols; c++) {
         /* check if current position isnt a baloon */
         if(b->board[r][c] == None)
            continue;
         
         /* check if baloon next to it is the same color */
         if(r + 1 < rows)
               if(b->board[r][c] == b->board[r+1][c])
                  return 1;
         
         /* check if baloon below current baloon is the same color */
         if(c + 1 < rows)
            if(b->board[r][c] == b->board[r][c+1])
               return 1;
      }
   }
   return 0;
}

int bp_score(BPGame * b) {
   /* return the current score */
   return b->score;
}


int bp_undo(BPGame * b) {
   /* check if any move have been made */
   if(b->moves->top <= 0) return 0;
   /* replace current board and score with last score and board in stacl */
   pop(b);
   return 1;
}

void bp_display_STD(BPGame *b) {
   int rows = b->rows;
   int cols = b->cols;
   /* display the board */
   for(int r = 0; r < rows; r++) {
      for(int c = 0; c < cols; c++) {
         printf("%c", b->board[r][c]);
      }
      printf("\n");
   }
}