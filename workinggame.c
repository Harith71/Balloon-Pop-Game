

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
   char** board;
   int rows;
   int cols;
   char*** board_states;
   int *scores;
   int top;
   int size;
   // YOU DECIDE WHAT TO PUT IN HERE TO CAPTURE THE STATE OF
   //   A "RUN" OF THE GAME.....
};






/*** IMPLEMENTATION OF bp_XXXX FUNCTIONS HERE  ****/

int indexOf(char color, char array[], int size) {
   int found = -1;

   for(int i = 0; i < 4; i++) {
      if(color == array[i]) {
         found = i;
         break;
      } 
   }
   return found;
}

BPGame * bp_create(int nrows, int ncols) {
   //srand(time(NULL)); commented out for testing pop function add back after
   char colors[4] = {Red, Green, Yellow, Blue};
   int r , c;

   if(nrows < 0 || ncols < 0 || nrows > MAX_ROWS || ncols > MAX_COLS) {
      fprintf(stderr, "rows or cols out of range");
      return NULL;
   }

   BPGame *b = (BPGame *) malloc(sizeof(BPGame));

   b->score = 0;
   b->top = 0;
   b->size = 5;
   b->rows = nrows;
   b->cols =  ncols;
   b->board = (char**) malloc(sizeof(char*) * nrows);

   for(r = 0; r < nrows; r++){
      b->board[r] = (char*) malloc(sizeof(char) * ncols);
   }

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

   if(nrows < 0 || ncols < 0 || nrows > MAX_ROWS || ncols > MAX_COLS) {
      fprintf(stderr, "rows or cols out of range");
      return NULL;
   }
   
   BPGame *b = (BPGame *) malloc(sizeof(BPGame));

   b->score = 0;
   b->top = 0;
   b->rows = nrows;
   b->cols =  ncols;
   b->board = (char**) malloc(sizeof(char*) * nrows);

   for(r = 0; r < nrows; r++){
      b->board[r] = (char*) malloc(sizeof(char) * ncols);
   }
   
   for(r = 0; r < nrows; r++) {
      for(c = 0; c < ncols; c++) {
         if(indexOf(mtx[r][c], colors, 4) == -1) {
            fprintf(stderr, "%c is not a valid color", mtx[r][c]);
            return NULL;
         }
         b->board[r][c] = mtx[r][c];
      }
   }
   return b;
}

void bp_destroy(BPGame *b) {
   int rows = b->rows;
   for(int r = 0; r < rows; r++) {
      free(b->board[r]);
   }
   free(b->board);
   free(b);
}

void bp_display(BPGame *b) {
   int rows = b->rows;
   int cols = b->cols;
   int r , c;
   if(rows < 0 || cols < 0) return;

   printf("\n   ");
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
      if(r < 10)
         printf("%d |", r);
      else
         printf("%d|", r);

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
   if(c - 1 > -1)
      if(b->board[r][c-1] == baloon){
         b->board[r][c-1] = None;
         popped++;
         friends = find_clusters(b, r, c - 1, baloon);
         popped = popped + friends;
      }
   
   if(r - 1 > -1)
      if(b->board[r-1][c] == baloon){
         b->board[r-1][c] = None;
         popped++;
         friends = find_clusters(b, r-1, c, baloon);
         popped = popped + friends;
      }
   

   if(r + 1 < b->rows)
      if(b->board[r+1][c] == baloon) {
         b->board[r+1][c] = None;
         popped++;
         friends = find_clusters(b, r + 1, c, baloon);
         popped = popped + friends;
      }
   
   if(c + 1 < b->cols)
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
   
   b->board_states[b->top] = b->board;
   char baloon = b->board[r][c];
   int total_popped = find_clusters(b, r, c, baloon);

   if(total_popped > 0) {
      b->board[r][c] = None;
      b->score = b->score + total_popped * (total_popped-1);
   }
   return total_popped;
}

// check if this works.
int bp_is_compact(BPGame * b) {
   int rows = b->rows;
   int cols = b->cols;
   int air;
   
   for(int c = 0; c < cols; c++) {
      air = 0;
      for(int r = 0; r < rows; r++) {
         if(b->board[r][c] == None) {
            air = 1;
         }
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

         if(b->board[r+1][c] == None) 
            continue;
            // better names for var pos
         int below_pos = r + 1;
         while (below_pos < rows && b->board[below_pos][c] != None) {
            b->board[below_pos - 1][c] = b->board[below_pos][c];
            below_pos++;
         }
         b->board[below_pos - 1][c] = None;
         r = rows;
        }
   }
   return;
}

int bp_can_pop(BPGame * b) {
   int rows = b->rows;
   int cols = b->cols;
   for(int r = 0; r < rows; r++) {
      for(int c = 0; c < cols; c++) {
         if(b->board[r][c] != None)
            continue;

         if(r + 1 < rows)
               if(b->board[r][c] == b->board[r+1][c])
                  return 1;
         
         if(c + 1 < rows)
            if(b->board[r][c] == b->board[r][c+1])
               return 1;
      }
   }
   return 0;
}

int bp_score(BPGame * b) {
   return b->score;
}

int bp_undo(BPGame * b) {
   if(b->top == 0) return 0;
   return 1;
}

/*
void sleep_ms(int milliseconds) // cross-platform sleep function
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = milliseconds * 1000000;
    nanosleep(&ts, NULL);
}

void animate(BPGame * board) {

  do {
    sleep_ms(300);
    printf("\nCompact: %d\n", bp_is_compact(board));
    bp_float_one_step(board);
    //clear();
    bp_display(board);
  }
  while(!bp_is_compact(board));
}


int main() {
   //char mtx[5][MAX_COLS] = {{'^','^','o','^','^',}, {'^','^','^','^','^',},{'^','^','^','^','^',},{'^','^','^','^','^',},
   //{'^','^','^','^','^',}};
   BPGame *b = bp_create(15, 15);
   //BPGame *b = bp_create_from_mtx(mtx, 5, 5);
   bp_display(b);
   //printf("\nCompact: %d", bp_is_compact(b));
   printf("\nCompact: %d", bp_is_compact(b));
   printf("\nPopped: %d", bp_pop(b, 3, 5));
   printf("Score: %d", b->score);
   
   bp_display(b);
   printf("\nCompact: %d", bp_is_compact(b));
   printf("------- moving");
   animate(b);
   

   bp_destroy(b);
} 
*/
