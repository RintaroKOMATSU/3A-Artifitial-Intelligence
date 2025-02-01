#include <iostream>
#include <memory>
#include <deque>
#include <array>
#include <queue>
#include <cmath>

#define BOARD_SIZE 4

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


char move[][2] = {{1, 2}, {2, 1}, {-2, 1}, {1, -2}, {-1, 2}, {2, -1}, {-1, -2}, {-2, -1}};

class BOARD {
public:
    std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cell;
    int depth = 0;
    int f = 0;
    int i_1 = 0; //position of 1;
    int j_1 = 0; //position of 1;
    std::shared_ptr<BOARD> parent_board = nullptr;

    BOARD(): f(depth){}
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
        cal_f();
    }
    BOARD(std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cells, int depth, int i_1, int j_1, std::shared_ptr<BOARD> parent_board):depth(depth) , i_1(i_1), j_1(j_1), parent_board(parent_board) {
        cell = cells;
        cal_f();
    }

    void cal_f() {
        f = depth;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (cell[i][j] != cell_goal[i][j]) {
                    f ++;
                }
            }
        }
    }

    void cal_p() {
        f = depth;
        for (int k = 1; k < BOARD_SIZE*BOARD_SIZE+1; k++) {
            int cell_ki, cell_kj, goal_ki, goal_kj;
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    if (cell[i][j] == k) {
                        cell_ki = i;
                        cell_kj = j;
                    }
                    if (cell_goal[i][j] == k) {
                        goal_ki = i;
                        goal_kj = j;
                    }
                }
            }
            f += abs(cell_ki-goal_ki)+abs(cell_kj-goal_kj);
        }
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

    while (step > 0) {
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
            auto board_tmp = open.begin();
            auto board_minf = open.begin();
            while (board_tmp != open.end()) {
                if (board_tmp->f < board_minf->f) {
                    board_minf = board_tmp;
                }
                ++board_tmp;
            }
            cur_board = *board_minf;
            open.erase(board_minf);
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

                    if (next_cell == cell_goal) {
                        BOARD parent_board = cur_board;
                        cur_board = BOARD(next_cell, parent_board.depth + 1, next_i1, next_j1, std::make_shared<BOARD>(parent_board));
                        num_child ++;
                        std::cout << "depth : " << cur_board.depth << ", f : " << cur_board.f << "\n";
                        std::cout <<  cur_board;
                        std::cout << "**** Answer found... ****\n";
                        step = 0;
                        goto end_loop;
                    }

                    if (!is_included) {
                        BOARD parent_board = cur_board;
                        BOARD new_board = BOARD(next_cell, parent_board.depth + 1, next_i1, next_j1, std::make_shared<BOARD>(parent_board));
                        //if (new_board.f <= parent_board.f) {
                            open.push_front(new_board);
                        //}
                        num_child ++;
                        std::cout << "depth : " << cur_board.depth << ", f : " << cur_board.f << "\n";
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