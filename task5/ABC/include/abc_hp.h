#pragma once

#include <vector>
#include <string>
#include "vec3.h"

// タンパク質折り畳み方向の定義
#define D -2    //down
#define U 2     //up    
#define R -1    //right
#define L 1     //left
#define S 0     //straight

// グローバル変数の外部宣言
extern std::vector<std::vector<int>> population;
extern std::vector<int> population_fval;
extern std::vector<int> population_s;
extern int N;
extern int S_limit;

// 関数宣言
std::vector<vec3> calPosArray(const std::vector<int>& sequence, const int sequence_length);
int FitnessFunction(const std::vector<int>& sequence, const std::string& input, const int sequence_length);
int RouletteSelection();

std::vector<int> GenerateRandomSequence3D(const std::string& input, const int sequence_length);
void Employed3D(const std::string& input, const int sequence_length, const int idx1); 
void Onlook3D(const std::string& input, const int sequence_length);
void Scout3D(const std::string& input, const int sequence_length, const int idx1);

std::vector<int> GenerateRandomSequence2D(const std::string& input, const int sequence_length);
void Employed2D(const std::string& input, const int sequence_length, const int idx1); 
void Onlook2D(const std::string& input, const int sequence_length);
void Scout2D(const std::string& input, const int sequence_length, const int idx1);

// ヘルパー関数
int random_array(const std::vector<int>& integers);
int random_int(int start, int end);
bool is_occupied(std::vector<vec3>& pos_arr, vec3 pos, const int size);
