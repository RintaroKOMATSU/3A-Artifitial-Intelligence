#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <vector>
#include <array>
#include <cmath>
#include <chrono>
#include <omp.h>
#include "decode.h"
#include "display.h"
#include "vec3.h"

//入力配列
#define INPUT "(HP)2PH2PHP2HPH2P2HPH";
#define N 400   //個体数
#define S_limit 50      //試行回数の上限
#define GEN_MAX 100  //世代数

#define D -2    //down
#define U 2     //up    
#define R -1    //right
#define L 1     //left
#define S 0     //straight

std::vector<int> GenerateRandomSequence(const std::string& input, const int sequence_length);
std::vector<vec3> calPosArray(const std::vector<int>& sequence, const int sequence_length);
int FitnessFunction(const std::vector<int>& sequence, const std::string& input, const int sequence_length);
int RouletteSelection();
void Employed(const std::string& input, const int sequence_length, const int idx1); 
void Onlook(const std::string& input, const int  sequence_length);
void Scout(const std::string& input, const int  sequence_length, const int idx1);

std::array<std::vector<int>, N> population;        //たんぱく質の構造配列
std::array<int, N> population_fval;                //適応度（水素結合の数）
std::array<int, N> population_s;                   //試行回数


int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::string input = INPUT;

    std::vector<int> optimal_sequence;
    int optimal_fval = 0;

    std::cout << "\n  ";
    std::cout << input << "\n";

    input = decode(input);

    int  sequence_length = input.size()-2;
    std::cout << "\n";
    std::cout << "*****Simulation start*******\n";

    //ランダムな構造をN個生成
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        population[i] = GenerateRandomSequence(input, sequence_length);
        population_fval[i] = FitnessFunction(population[i], input,  sequence_length);
        population_s[i] = 0;
    }
    //次世代を生成
    for (int generation = 1; generation < GEN_MAX+1; generation++) {
        std::cout <<" Generation : " << generation << "\n";
        std::cout << " Maximum hydrogen bonds found: " << optimal_fval << "\r\033[A";
        std::cout.flush();

        #pragma omp parallel for
        for (int j =0; j < N; j++) {
            Employed(input,  sequence_length, j);
        }

        for (int j = 0; j < N; j++) {
            Onlook(input,  sequence_length);
        }
        #pragma omp parallel for
        for (int j = 0; j < N; j++) {
            Scout(input,  sequence_length, j);
        }
        int fval = *std::max_element(population_fval.begin(), population_fval.end());
        if ( fval > optimal_fval) {
            optimal_fval = fval;
            optimal_sequence = population[std::distance(population_fval.begin(), std::max_element(population_fval.begin(), population_fval.end()))];
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "\033[A*****Simulation finish*******\n";
    std::cout << "\n Maximum hydrogen bonds found: " << optimal_fval << "\n";
    double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    duration *=1e-9;
    std::cout << " Duration time : " << duration << " s"<< "\n";
    display(input,calPosArray(optimal_sequence, sequence_length));
    return 0;
}

//int配列の中からランダムに要素を返す
int random_array(const std::vector<int>& integers) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribute(0, integers.size()-1);
    return integers[distribute(gen)];
}

//startからendの中の整数からランダムに返す
int random_int(int start, int end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribute(start, end);
    return distribute(gen);
}

//posにpos_arr中のアミノ酸がが存在するか判定
bool is_occupied(std::vector<vec3>& pos_arr, vec3 pos, const int size) {
    for (int i = 0; i < size; i++) {
        if (pos_arr[i] == pos) {
            return true;
        }
    }
    return false;
}

//sequenceから各アミノ酸の位置を計算
std::vector<vec3> calPosArray(const std::vector<int>& sequence, const int sequence_length) {
    //各アミノ酸の位置
    std::vector<vec3> pos_arr(sequence_length+2);
    pos_arr[0] = vec3(0, 0, 0);
    pos_arr[1] = vec3(1, 0, 0);

    //ローカル座標系を定義
    vec3 local_x_axis(1, 0, 0); 
    vec3 local_y_axis(0, 1, 0);
    vec3 local_z_axis(0, 0, 1);

    //アミノ酸の鎖の伸長方向
    vec3 dir = local_x_axis;
    
    for (int i= 0; i <  sequence_length; i++) {
        int move = sequence[i];
        switch (move){
            case S: {
                break;
            }
            case L: {
                dir = local_y_axis;
                local_x_axis = dir;
                local_y_axis = cross(local_z_axis, local_x_axis);
                break;
            }
            case R: {
                dir = -local_y_axis;
                local_x_axis = dir;
                local_y_axis = cross(local_z_axis, local_x_axis);
                break;
            }
            case U: {
                dir = local_z_axis;
                local_x_axis = dir;
                local_z_axis = cross(local_x_axis, local_y_axis);
                break;
            }
            case D: {
                dir = -local_z_axis;
                local_x_axis = dir;
                local_z_axis = cross(local_x_axis, local_y_axis);
                break;
            }
        }
        vec3 next_pos = pos_arr[i+1]+dir;
        if (is_occupied(pos_arr, next_pos, i+1)) {
            pos_arr.clear();
            for (int j = 0; j < sequence_length+2; j++) {
                pos_arr[j] = vec3(0, 0, 0);
            }
            return pos_arr;
        }
        pos_arr[i+2] = next_pos;
    }
    return pos_arr;
}


//sequenceの適合度を計算
int FitnessFunction(const std::vector<int>& sequence, const std::string& input, const int  sequence_length) {
    int value = 0;
    //アミノ酸の座標
    std::vector<vec3> pos_arr(sequence_length+2);
    pos_arr[0] = vec3(0, 0, 0);
    pos_arr[1] = vec3(1, 0, 0);
    //親水性アミノ酸の座標
    std::vector<vec3> Hpos_arr;
    if (input[0] == 'H') {
        Hpos_arr.push_back(vec3(0, 0, 0));
    }
    if (input[1] == 'H') {
        Hpos_arr.push_back(vec3(1, 0, 0));
    }

    //ローカル座標系を定義
    vec3 local_x_axis(1, 0, 0); 
    vec3 local_y_axis(0, 1, 0);
    vec3 local_z_axis(0, 0, 1);

    //アミノ酸の鎖の伸長方向
    vec3 dir = local_x_axis;

    for (int i= 0; i <  sequence_length; i++) {
        int move = sequence[i];
        switch (move){
            case S: {
                break;
            }
            case L: {
                dir = local_y_axis;
                local_x_axis = dir;
                local_y_axis = cross(local_z_axis, local_x_axis);
                break;
            }
            case R: {
                dir = -local_y_axis;
                local_x_axis = dir;
                local_y_axis = cross(local_z_axis, local_x_axis);
                break;
            }
            case U: {
                dir = local_z_axis;
                local_x_axis = dir;
                local_z_axis = cross(local_x_axis, local_y_axis);
                break;
            }
            case D: {
                dir = -local_z_axis;
                local_x_axis = dir;
                local_z_axis = cross(local_x_axis, local_y_axis);
                break;
            }
        }
        vec3 next_pos = pos_arr[i+1]+dir;
        if (input[i+2] == 'H') {
            std::vector<vec3> adjacent_vecs = {vec3(0, 0, 1), vec3(0, 0, -1),  vec3(1, 0, 0), 
                                               vec3(-1, 0, 0), vec3(0, 1, 0), vec3(0, -1, 0)};
            for (const vec3 adjacent_vec: adjacent_vecs) {
                if (adjacent_vec != -dir && is_occupied(Hpos_arr, next_pos + adjacent_vec, Hpos_arr.size())) {
                    value += 1;
                }
            }
            Hpos_arr.push_back(next_pos);
        }
        if (is_occupied(pos_arr, next_pos, i+1)) {
            return -1;
        }
        pos_arr[i+2] = next_pos;
    }
    return value;
}

//ランダムなsequenceを生成
std::vector<int> GenerateRandomSequence(const std::string& input, const int  sequence_length) {
    std::vector<int> sequence(sequence_length);
    //各アミノ酸の位置
    std::vector<vec3> pos_arr(sequence_length+2);
    pos_arr[0] = vec3(0, 0, 0);
    pos_arr[1] = vec3(1, 0, 0);

    //ローカル座標系を定義
    vec3 local_x_axis(1, 0, 0); 
    vec3 local_y_axis(0, 1, 0);
    vec3 local_z_axis(0, 0, 1);

    vec3 dir = local_x_axis;

    for (int i = 0; i < sequence_length; i++) {
        std::vector<int> next_moves;
        //次に進むことのできる方向を探索
        vec3 L_dir = local_y_axis;  //left
        vec3 R_dir = -local_y_axis; //right
        vec3 U_dir = local_z_axis;  //up
        vec3 D_dir = -local_z_axis; //down
        vec3 S_dir = local_x_axis;  //straight
        if (!is_occupied(pos_arr, pos_arr[i+1]+S_dir, i+1)) {
            next_moves.push_back(S);
        }
        if (!is_occupied(pos_arr, pos_arr[i+1]+L_dir, i+1)) {
            next_moves.push_back(L);
        }
        if (!is_occupied(pos_arr, pos_arr[i+1]+R_dir, i+1)) {
            next_moves.push_back(R);
        }
        if (!is_occupied(pos_arr, pos_arr[i+1]+U_dir, i+1)) {
            next_moves.push_back(U);
        }
        if (!is_occupied(pos_arr, pos_arr[i+1]+D_dir, i+1)) {
            next_moves.push_back(D);
        }
        //進めなかったら最初からやり直し
        if (next_moves.empty()) {
            sequence = GenerateRandomSequence(input,  sequence_length);
            return sequence;
        }
        //次に進む方向をランダムに決定
        int next_move = random_array(next_moves);
        switch (next_move){
            case S: {
                break;
            }
            case L: {
                dir = L_dir;
                local_x_axis = dir;
                local_y_axis = cross(local_z_axis, local_x_axis);
                break;
            }
            case R: {
                dir = R_dir;
                local_x_axis = dir;
                local_y_axis = cross(local_z_axis, local_x_axis);
                break;
            }
            case U: {
                dir = U_dir;
                local_x_axis = dir;
                local_z_axis = cross(local_x_axis, local_y_axis);
                break;
            }
            case D: {
                dir = D_dir;
                local_x_axis = dir;
                local_z_axis = cross(local_x_axis, local_y_axis);
                break;
            }
        }
        pos_arr[i+2] = pos_arr[i+1]+dir;
        sequence[i] = next_move;
    }
    return sequence;
}

//ルーレット選択
int RouletteSelection() {
    int sum_fval = N + std::accumulate(population_fval.begin(), population_fval.end(), 0);
    std::vector<double> probabilities(N);
    for (int i = 0; i < N; i++) {
        probabilities[i] = ((double)population_fval[i]+1.0)/(double)sum_fval;
    }
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::discrete_distribution<> distribution(probabilities.begin(), probabilities.end());
    return distribution(gen);
}


void Employed(const std::string& input, const int  sequence_length, const int idx1) {
    int idx2 = random_int(0, N-1);
    while (idx1 == idx2) {
        idx2 = random_int(0, N-1);
    }

    const std::vector<int>& sequence1 = population[idx1];
    const std::vector<int>& sequence2 = population[idx2];
    std::vector<int> child_sequence(sequence_length);

    int crossover_point1 = random_int(0, sequence_length - 2);
    int crossover_point2 = random_int(crossover_point1, sequence_length - 1);

    std::copy(sequence1.begin(),  sequence1.begin() + crossover_point1, child_sequence.begin());
    std::copy(sequence2.begin() + crossover_point1, sequence2.begin() + crossover_point2, child_sequence.begin() + crossover_point1);
    std::copy(sequence1.begin()+crossover_point2, sequence1.end(), child_sequence.begin()+crossover_point2);

    std::vector<int> moves = {S, L, R, U, D};
    int max_fval = -1;
    int max_move1 = -1;
    int max_move2 = -1;
    for (std::size_t i = 0; i < moves.size(); i++) {
        for (std::size_t j = 0; j < moves.size(); j++) {
            int move1 = moves[i];
            int move2 = moves[j];
            *(child_sequence.begin()+crossover_point1) = move1;
            *(child_sequence.begin()+crossover_point2) = move2;
            int fval_ij = FitnessFunction(child_sequence, input,  sequence_length);
            if (fval_ij > max_fval) {
                max_fval = fval_ij;
                max_move1 = move1;
                max_move2 = move2;
            }
        }
    }
    if (max_fval == -1) {
        Employed(input,  sequence_length, idx1);
        return;
    }

    if (max_fval > population_fval[idx1]) {
        *(child_sequence.begin()+crossover_point1) = max_move1;
        *(child_sequence.begin()+crossover_point2) = max_move2;
        population[idx1] = child_sequence;
        population_fval[idx1] = max_fval;
        population_s[idx1] = 0;
    } else {
        population_s[idx1]++;
    }
}



void Onlook(const std::string& input, const int  sequence_length) {
    int idx1 = RouletteSelection();
    int idx2 = random_int(0, N-1);
    while (idx1 == idx2) {
        idx2 = random_int(0, N-1);
    }
    const std::vector<int>& sequence1 = population[idx1];
    const std::vector<int>& sequence2 = population[idx2];
    std::vector<int> child_sequence(sequence_length);

    int crossover_point1 = random_int(0, sequence_length - 2);
    int crossover_point2 = random_int(crossover_point1, sequence_length - 1 );

    std::copy(sequence1.begin(),  sequence1.begin() + crossover_point1, child_sequence.begin());
    std::copy(sequence2.begin() + crossover_point1, sequence2.begin() + crossover_point2, child_sequence.begin() + crossover_point1);
    std::copy(sequence1.begin()+crossover_point2, sequence1.end(), child_sequence.begin()+crossover_point2);

    std::vector<int> moves = {L, S, R, U, D};
    int max_fval = -1;
    int max_move1 = -1;
    int max_move2 = -1;
    for (std::size_t i = 0; i < moves.size(); i++) {
        for (std::size_t j = 0; j < moves.size(); j++) {
            int move1 = moves[i];
            int move2 = moves[j];
            *(child_sequence.begin()+crossover_point1) = move1;
            *(child_sequence.begin()+crossover_point2) = move2;
            int fval_ij = FitnessFunction(child_sequence, input,  sequence_length);
            if (fval_ij > max_fval) {
                max_fval = fval_ij;
                max_move1 = move1;
                max_move2 = move2;
            }
        }
    }
    if (max_fval == -1) {
        Employed(input,  sequence_length, idx1);
        return;
    }
    if (max_fval > population_fval[idx1]) {
        *(child_sequence.begin()+crossover_point1) = max_move1;
        *(child_sequence.begin()+crossover_point2) = max_move2;
        population[idx1] = child_sequence;
        population_fval[idx1] = max_fval;
        population_s[idx1] = 0;
    }
}


void Scout(const std::string& input, const int sequence_length, const int idx1) {
    if (population_s[idx1] == S_limit) {
        population[idx1] = GenerateRandomSequence(input,  sequence_length);
        population_fval[idx1] = FitnessFunction(population[idx1], input,  sequence_length);
        population_s[idx1] = 0;
    }
}

