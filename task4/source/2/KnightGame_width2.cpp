#include <iostream>
#include <memory>
#include <deque>
#include <array>
#include <cmath>

#define BOARD_SIZE 4
#define MAX_DEPTH 30

std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cell_start = {{
    {10, 1, 12, 4},
    {5, 13, 7, 9},
    {15, 3, 2, 6},
    {11, 14, 8, 16}
}};

std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cell_goal = {{
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12},
    {13, 14, 15, 16}
}};

char move[8][2] = {{1, 2}, {2, 1}, {-2, 1}, {1, -2}, {-1, 2}, {2, -1}, {-1, -2}, {-2, -1}};

class BOARD {
public:
    std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cell;
    int depth;
    int i_1; //position of 1;
    int j_1; //position of 1;
    std::shared_ptr<BOARD> parent_board;

    BOARD() : depth(0), i_1(0), j_1(0), parent_board(nullptr) {}
    BOARD(std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cells, int depth, std::shared_ptr<BOARD> parent_board):depth(depth), parent_board(parent_board) {
        cell = cells;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (cell[i][j] == 1) {
                    i_1 = i;
                    j_1 = j;
                }
            }
        }
    }
    BOARD(std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cells, int depth, int i_1, int j_1, std::shared_ptr<BOARD> parent_board):depth(depth) , i_1(i_1), j_1(j_1), parent_board(parent_board) {
        cell = cells;
    }

    bool operator==(const BOARD& other) const {
        return cell == other.cell;
    }
};

std::ostream& operator <<(std::ostream& out, const BOARD& board) {
    int max_digit = int(log10(BOARD_SIZE*BOARD_SIZE))+1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0;  j < BOARD_SIZE; j++) {
            int num = (int)board.cell[i][j];
            int num_digit = int(log10(num))+1;
            for (int p = 0; p < max_digit-num_digit; p++) {
                out << " ";
            }
            out << num;
            if (j != BOARD_SIZE-1) {
                out << "|";
            } else {
                out << "\n";
                if (i != BOARD_SIZE-1) {
                    for (int k = 0; k < BOARD_SIZE; k++) {
                        for (int p = 0; p < max_digit; p++) {
                            out << "-";
                        }
                        if (k != BOARD_SIZE-1) {
                            out << "+";
                        } else {
                            out << "\n";
                        }
                    }
                }
            }
        }
    }
    out << "\n";
    return out;
}

int main() {
    std::deque<BOARD> open;
    std::deque<BOARD> closed;

    int num_child = 0;

    //step1
    open.push_front(BOARD(cell_start, 0, nullptr));
    std::cout << open.front();
    int step = 2;
    BOARD cur_board;

    while (true) {
        //step2
        if (step == 2) {
            if (open.empty()) {
                std::cout << "Answer did not found\n";
                return 0;
            } else {
                step = 3;
            }
        }

        //step3
        if (step == 3) {
            cur_board = open.front();
            open.pop_front();
            if (cur_board.depth > MAX_DEPTH) {
                std::cout << "Answer did not found\n";
                return 0;
            }
            closed.push_back(cur_board);
            step = 4;
        }

        //step4
        if (step == 4) {
            for (int i = 0; i < 8; i++) {
                char delta_i = move[i][0];
                char delta_j = move[i][1];
                int cur_i1 = cur_board.i_1;
                int cur_j1 = cur_board.j_1;
                int next_i1 = cur_i1 + delta_i;
                int next_j1 = cur_j1 + delta_j;

                if (0 <= next_i1 && next_i1 < BOARD_SIZE && 0 <= next_j1 && next_j1 < BOARD_SIZE) {
                    auto next_cell = cur_board.cell;
                    next_cell[next_i1][next_j1] = cur_board.cell[cur_i1][cur_j1];
                    next_cell[cur_i1][cur_j1] = cur_board.cell[next_i1][next_j1];

                    //check if next_cell is in open or closed
                    bool is_included = false;
                    for (BOARD& board : open) {
                        if (board.cell == next_cell) {
                            is_included = true;
                            break;
                        }
                    }
                    for (BOARD& board : closed) {
                        if (board.cell == next_cell) {
                            is_included = true;
                            break;
                        }
                    }


                    //if next_cell = goal
                    if (next_cell == cell_goal) {
                        BOARD parent_board = cur_board;
                        cur_board = BOARD(next_cell, cur_board.depth + 1, next_i1, next_j1, std::make_shared<BOARD>(parent_board));
                        num_child ++;
                        std::cout << "depth: " << cur_board.depth << "\n";
                        std::cout <<  cur_board;
                        std::cout << "**** Answer found... ****\n";
                        step = 0;
                        goto end_loop;
                    }


                    //if not next_cell = goal and not in open or closed
                    if (!is_included) {
                        BOARD parent_board = cur_board;
                        BOARD new_board = BOARD(next_cell, cur_board.depth + 1, next_i1, next_j1, std::make_shared<BOARD>(parent_board));
                        open.push_back(new_board);
                        num_child ++;
                        std::cout << "depth: " << new_board.depth << "\n";
                        std::cout << new_board;
                    }
                }
            }
            step = 2;
        }
    }
    end_loop:
    std::deque<BOARD> answer;
    while(cur_board.parent_board != nullptr) {
        answer.push_front(cur_board);
        cur_board = *cur_board.parent_board;
    }
    answer.push_front(cur_board);
    
    for (BOARD board: answer) {
        std::cout << board;
    }
    std::cout << "Number of children : " << num_child << "\n";
    std::cout << "length of solution : " << answer.size() << "\n";
    return 0;
}