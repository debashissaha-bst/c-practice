#include <stdio.h>
#include <stdbool.h>

#define SIZE 9
#define BOX  3
#define EMPTY 0

typedef int Board[SIZE][SIZE];

static void print_board(const Board board) {
    for (int r = 0; r < SIZE; r++) {
        if (r % BOX == 0 && r != 0) printf("------+-------+------\n");
        for (int c = 0; c < SIZE; c++) {
            if (c % BOX == 0 && c != 0) printf("| ");
            printf("%d ", board[r][c]);
        }
        printf("\n");
    }
}

static bool in_row(const Board board, int row, int num) {
    for (int c = 0; c < SIZE; c++) {
        if (board[row][c] == num) return true;
    }
    return false;
}

static bool in_col(const Board board, int col, int num) {
    for (int r = 0; r < SIZE; r++) {
        if (board[r][col] == num) return true;
    }
    return false;
}

static bool in_box(const Board board, int box_start_row, int box_start_col, int num) {
    for (int r = 0; r < BOX; r++) {
        for (int c = 0; c < BOX; c++) {
            if (board[box_start_row + r][box_start_col + c] == num) return true;
        }
    }
    return false;
}

static bool is_safe(const Board board, int row, int col, int num) {
    return !in_row(board, row, num) &&
           !in_col(board, col, num) &&
           !in_box(board, row - row % BOX, col - col % BOX, num);
}

static bool find_empty_cell(const Board board, int *row, int *col) {
    for (*row = 0; *row < SIZE; (*row)++) {
        for (*col = 0; *col < SIZE; (*col)++) {
            if (board[*row][*col] == EMPTY) return true;
        }
    }
    return false;
}


static bool solve_sudoku(Board board) {
    int row, col;
    if (!find_empty_cell(board, &row, &col)) {
        return true; 
    }

    for (int num = 1; num <= 9; num++) {
        if (is_safe(board, row, col, num)) {
            board[row][col] = num;

            if (solve_sudoku(board)) {
                return true;
            }

            board[row][col] = EMPTY; 
        }
    }
    return false; 
}

int main(void) {
    Board board = {
        {5, 3, 0,  0, 7, 0,  0, 0, 0},
        {6, 0, 0,  1, 9, 5,  0, 0, 0},
        {0, 9, 8,  0, 0, 0,  0, 6, 0},

        {8, 0, 0,  0, 6, 0,  0, 0, 3},
        {4, 0, 0,  8, 0, 3,  0, 0, 1},
        {7, 0, 0,  0, 2, 0,  0, 0, 6},

        {0, 6, 0,  0, 0, 0,  2, 8, 0},
        {0, 0, 0,  4, 1, 9,  0, 0, 5},
        {0, 0, 0,  0, 8, 0,  0, 7, 9}
    };

    printf("Initial Sudoku puzzle:\n");
    print_board(board);

    if (solve_sudoku(board)) {
        printf("\nSolved puzzle:\n");
        print_board(board);
    } else {
        printf("\nNo solution exists for this puzzle.\n");
    }

    return 0;
}
