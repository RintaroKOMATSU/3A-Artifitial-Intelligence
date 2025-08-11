#include <iostream>
#include <memory>
#include <deque>
#include <array>
#include <queue>
#include <cmath>
#include <chrono>
#include <unordered_set>


#define BOARD_SIZE 4
#define MIN_NUM_LENGTH 4
#define D 8

//problem3
std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cell_start = {{
    {16, 12, 8, 4},
    {15, 11, 7, 3},
    {14, 10, 6, 2},
    {13, 9, 5, 1}
}};

//problem2
// std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cell_start = {{
//     {10, 1, 12, 4},
//     {5, 13, 7, 9},
//     {15, 3, 2, 6},
//     {11, 14, 8, 16}
// }};

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
    char num_move = 1; //num move
    int  num_length = 0; //number of times num was moved
    int num_i = 0; //position of num;
    int num_j = 0; //position of num;
    std::shared_ptr<BOARD> parent_board = nullptr;

    BOARD(){}
    BOARD(std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cells, int depth, char num_move, int num_length, std::shared_ptr<BOARD> parent_board):depth(depth), num_move(num_move),num_length(num_length), parent_board(parent_board) {
        cell = cells;
        find_num();
    }
    BOARD(std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cells, int depth, int num_i, int num_j, char num_move, int num_length,std::shared_ptr<BOARD> parent_board):depth(depth) , num_i(num_i), num_j(num_j) ,num_move(num_move) , num_length(num_length), parent_board(parent_board) {
        cell = cells;
    }


    void find_num() {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (cell[i][j] == num_move) {
                    num_i = i;
                    num_j = j;
                    return;
                }
            }
        }
    }

    void change_num(char num) {
        num_move = num;
        find_num();
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

struct Equal{
    bool operator()(const std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE>& board1, const std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE>& board2) const {
        return board1 == board2;
    }
};

struct Hash{
    std::size_t operator()(const std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE>& board) const {
        unsigned long long seed = 0;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                seed = board[i][j] -1  + (seed << 4);
            }
        }
        return seed;
    }
};




std::deque<BOARD> open;
std::deque<BOARD> closed;
std::unordered_set<std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE>, Hash, Equal> board_set;

bool update_cell(BOARD& cur_board, int i, int& num_child, int& step ) {
    char delta_i = move[i][0];
    char delta_j = move[i][1];
    int cur_numi = cur_board.num_i;
    int cur_numj = cur_board.num_j;
    int next_numi = cur_numi + delta_i;
    int next_numj = cur_numj + delta_j;

    if (0 <= next_numi && next_numi < BOARD_SIZE && 0 <= next_numj && next_numj < BOARD_SIZE) {
        auto next_cell = cur_board.cell;
        next_cell[next_numi][next_numj] = cur_board.cell[cur_numi][cur_numj];
        next_cell[cur_numi][cur_numj] = cur_board.cell[next_numi][next_numj];

        bool is_checked = false;
 
        if (board_set.find(next_cell) != board_set.end()) {
            is_checked = true;
        }

        if (next_cell == cell_goal) {
            BOARD& parent_board = cur_board;
            cur_board = BOARD(next_cell, parent_board.depth + 1, next_numi, next_numj, parent_board.num_move, parent_board.num_length+1, std::make_shared<BOARD>(parent_board));
            num_child ++;
            // std::cout << "depth : " << cur_board.depth << "\n";
            // std::cout <<  cur_board;
            std::cout << "**** Answer found... ****\n";
            step = 0;
            return true;
        }

        if (!is_checked) {
            BOARD& parent_board = cur_board;
            BOARD new_board = BOARD(next_cell, parent_board.depth + 1, next_numi, next_numj, parent_board.num_move, parent_board.num_length+1, std::make_shared<BOARD>(parent_board));
            open.push_front(new_board);
            board_set.insert(next_cell);
            num_child ++;
            if (num_child % 100000 == 0) {
                std::cout << "number of children : "<< num_child << "\r";
                std::cout.flush();
            }
            // std::cout << "depth : " << cur_board.depth << "\n";
            // std::cout << new_board;
        }
    }
    return false;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    int num_child = 0;
    int n = 1;

    //step1
    open.push_front(BOARD(cell_start, 0, 1, 0, nullptr));
    board_set.insert(cell_start);
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
            while (board_tmp != open.end()) {
                if (board_tmp->depth <= D * n) {
                    break;
                }
                ++board_tmp;
            }

            if (board_tmp == open.end()) {
                step = 5;
                continue;
            }
            cur_board = *board_tmp;
            open.erase(board_tmp);
            closed.push_back(cur_board);
            step = 4;
        }

        //step4
        if (step == 4) {

            for (int i = 0; i < 8; i++) {
                if (update_cell(cur_board, i, num_child, step)) {
                    goto end_loop;
                }
                if (cur_board.num_length > MIN_NUM_LENGTH) {
                    for (int num_move = 1; num_move < BOARD_SIZE*BOARD_SIZE+1; num_move++) {
                        if (num_move == cur_board.num_move) {
                            continue;
                        }
                        cur_board.change_num(num_move); //change moving number
                        if (update_cell(cur_board, i, num_child, step)) {
                            goto end_loop;
                        }
                    }
                }
            }
            step = 2;
        }

        //step5
        if (step == 5) {
            n++;
            step = 3;
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
    std::cout.flush();
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << duration*1e-6 << " ms"<< "\n";
    
    return 0;
}