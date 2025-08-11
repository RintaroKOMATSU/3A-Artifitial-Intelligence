#include <iostream>
#include <fstream>
#include <memory>
#include <array>
#include <deque>
#include <queue>
#include <unordered_set>
#include <cmath>
#include <chrono>

#define BOARD_SIZE 4
#define MIN_NUM_LENGTH 1

class BOARD;

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
    int f = 0;
    char num_move = 1; // active number
    int num_length = 0; // The number of consecutive moves of the same number
    char num_i = 0; // The column index of the active number 
    char num_j = 0; // The row index of the active number

    std::shared_ptr<BOARD> parent_board = nullptr;

    BOARD(): f(depth){}
    BOARD(std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cells, int depth, char num_move, int num_length, std::shared_ptr<BOARD> parent_board)
    :depth(depth), num_move(num_move),num_length(num_length), parent_board(parent_board) 
    {
        cell = cells;
        find_num();
        cal_f();
    }
    BOARD(std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cells, int depth, int num_i, int num_j, char num_move, int num_length,std::shared_ptr<BOARD> parent_board)
    :depth(depth) , num_i(num_i), num_j(num_j) ,num_move(num_move) , num_length(num_length), parent_board(parent_board) 
    {
        cell = cells;
        cal_f();
    }

    //find active number
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

    void cal_f() {
        //cal_f1();
        //cal_f2();
        cal_f3();
        //cal_f4();
    }

    //change active number
    void change_num(const char num) {
        num_move = num;
        find_num();
    }

    //目標と異なる位置にあるコマの数
    void cal_f1() {
        f = depth;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (cell[i][j] != cell_goal[i][j]) {
                    f ++;
                }
            }
        }
    }
    //目標と異なる位置にあるコマの数 ＋ マンハッタン距離
    void cal_f2() {
        f = depth;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (cell[i][j] != cell_goal[i][j]) {
                    f ++;
                }
            }
        }
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
            f += (abs(cell_ki-goal_ki)+abs(cell_kj-goal_kj));
        }
    }

    //目標と異なる位置にあるコマの数 ＋ マンハッタン距離 % 3
    void cal_f3() {
        f = depth;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (cell[i][j] != cell_goal[i][j]) {
                    f ++;
                }
            }
        }
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
            f += (abs(cell_ki-goal_ki)+abs(cell_kj-goal_kj))%3;
        }
    }

    void cal_f4() {
        f = depth;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (cell[i][j] != cell_goal[i][j]) {
                    f ++;
                }
            }
        }
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
            f += (abs(cell_ki-goal_ki)+abs(cell_kj-goal_kj))%3 + (abs(cell_ki-goal_ki)+abs(cell_kj-goal_kj));
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



struct Compare{
    bool operator()(const BOARD& board1, const BOARD& board2) {
        return board1.f > board2.f;
    }
};

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




std::priority_queue<BOARD, std::vector<BOARD>, Compare> open;
std::deque<BOARD> closed;
std::unordered_set<std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE>, Hash, Equal> board_set;


//step 4
bool update_cell(BOARD& cur_board, int i, int& num_child, char& step ) {
    char delta_i = move[i][0];
    char delta_j = move[i][1];
    char cur_numi = cur_board.num_i;
    char cur_numj = cur_board.num_j;
    char next_numi = cur_numi + delta_i;
    char next_numj = cur_numj + delta_j;

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
            std::cout << "**** Answer found... ****             \n";
            step = 0;
            return true;
        }

        if (!is_checked) {
            BOARD& parent_board = cur_board;
            BOARD new_board = BOARD(next_cell, parent_board.depth + 1, next_numi, next_numj, parent_board.num_move, parent_board.num_length+1, std::make_shared<BOARD>(parent_board));
            open.push(new_board);
            board_set.insert(next_cell);
            num_child ++;
            if (num_child % 100000 == 0) {
                std::cout << "number of children : "<< num_child << "\r";
                std::cout.flush();
            }
        }
    }
    return false;
}

int main(int argc, char** argv) {
    auto start = std::chrono::high_resolution_clock::now();
    int num_child = 0;

    //step1
    open.push(BOARD(cell_start, 0, 1, 0, nullptr));
    board_set.insert(cell_start);
    std::cout << open.top();
    char step = 2;
    BOARD cur_board;
    while (step > 0) {
        //step2
        if (step == 2) {
            if (open.empty()) {
                std::cout << "Answer did not found...            \n";
               return 0;
            } else {
                step = 3;
            }
        }

        //step3
        if (step == 3) {
            cur_board = open.top();
            open.pop();
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
    std::cout << "Length of solution : " << answer.size() << "\n";
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    duration *=1e-6;
    std::cout << "Duration time : " << duration << " ms"<< "\n";

    return 0;
}