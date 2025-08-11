#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <vector>
#include <array>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include "display.h"
#include "vec3.h"
#include "setting.h"
#include "abc_hp.h"

//入力配列
std::string input =  "H10H2(PH)3(P3)2HPH4(PH)3PH2";
int N  = 1000;   //個体数
int S_limit =  50 ;     //試行回数の上限
int GEN_MAX  = 100;  //世代数

std::vector<std::vector<int>> population(N);        //たんぱく質の構造配列
std::vector<int> population_fval(N);                //適応度（水素結合の数）
std::vector<int> population_s(N);                   //試行回数

// カラーコード定義
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";

// ログファイル用のグローバル変数
std::ofstream logFile;

// 統計情報構造体
struct Statistics {
    double mean;
    double max;
    double min;
    double stddev;
    int convergence_count;
};

// 統計情報計算関数
Statistics calculateStatistics() {
    Statistics stats;
    stats.max = *std::max_element(population_fval.begin(), population_fval.end());
    stats.min = *std::min_element(population_fval.begin(), population_fval.end());
    
    // 平均値計算
    double sum = std::accumulate(population_fval.begin(), population_fval.end(), 0.0);
    stats.mean = sum / N;
    
    // 標準偏差計算
    double variance = 0.0;
    for (int fval : population_fval) {
        variance += (fval - stats.mean) * (fval - stats.mean);
    }
    stats.stddev = std::sqrt(variance / N);
    
    // 収束個体数（最大値を持つ個体数）
    stats.convergence_count = std::count(population_fval.begin(), population_fval.end(), (int)stats.max);
    
    return stats;
}

// ログファイル作成関数
std::string createLogFileName() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    std::stringstream ss;
    ss << "data/simulation_log_" 
       << std::put_time(&tm, "%Y%m%d_%H%M%S") 
       << ".txt";
    return ss.str();
}

// ログファイル初期化関数
void initializeLog(const std::string& filename, bool is3D, const std::string& input_seq, 
                   int sequence_length, int population_size, int max_generations, int trial_limit) {
    // dataディレクトリが存在しない場合は作成
    mkdir("data", 0755);
    
    logFile.open(filename);
    if (!logFile.is_open()) {
        std::cerr << "Error: Could not create log file: " << filename << std::endl;
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    logFile << "============================================================================\n";
    logFile << "                     PROTEIN FOLDING OPTIMIZATION                    \n";
    logFile << "                           Using ABC Algorithm                             \n";
    logFile << "============================================================================\n\n";
    
    logFile << "Simulation Started: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n\n";
    
    logFile << "CONFIGURATION PARAMETERS:\n";
    logFile << "------------------------\n";
    logFile << "Input Sequence        : " << input_seq << "\n";
    logFile << "Sequence Length       : " << sequence_length << "\n";
    logFile << "Population Size       : " << population_size << "\n";
    logFile << "Maximum Generations   : " << max_generations << "\n";
    logFile << "Trial Limit           : " << trial_limit << "\n";
    logFile << "Optimization Mode     : " << (is3D ? "3D" : "2D") << "\n\n";
    
    logFile << "EVOLUTION PROGRESS:\n";
    logFile << "==================\n";
    logFile << std::left 
            << std::setw(10) << "Gen" 
            << std::setw(8) << "Best" 
            << std::setw(8) << "Min" 
            << std::setw(10) << "Average" 
            << std::setw(10) << "StdDev" 
            << std::setw(12) << "Converged" 
            << "Notes\n";
    logFile << std::string(70, '-') << "\n";
    logFile.flush();
}

// ログに世代統計情報を記録する関数
void logGenerationStats(int generation, const Statistics& stats, bool newBest = false) {
    if (!logFile.is_open()) return;
    
    logFile << std::left 
            << std::setw(10) << generation
            << std::setw(8) << (int)stats.max
            << std::setw(8) << (int)stats.min
            << std::setw(10) << std::fixed << std::setprecision(2) << stats.mean
            << std::setw(10) << std::setprecision(2) << stats.stddev
            << std::setw(12) << (std::to_string(stats.convergence_count) + "/" + std::to_string(N));
    logFile << "\n";
    logFile.flush();
}

// ログファイルを閉じる関数
void finalizeLog(const Statistics& finalStats, double executionTime, const std::vector<int>& optimalSequence) {
    if (!logFile.is_open()) return;
    
    logFile << "\nSimulation finished\n";
    logFile <<  "\n\n";
    
    logFile << "FINAL RESULTS:\n";
    logFile << "-------------\n";
    logFile << "Maximum Hydrogen Bonds    : " << (int)finalStats.max << "\n";
    logFile << "Minimum Fitness          : " << (int)finalStats.min << "\n";
    logFile << "Average Fitness          : " << std::fixed << std::setprecision(2) << finalStats.mean << "\n";
    logFile << "Standard Deviation       : " << std::setprecision(2) << finalStats.stddev << "\n";
    logFile << "Converged Individuals    : " << finalStats.convergence_count << "/" << N << "\n";
    logFile << "Execution Time           : " << std::setprecision(3) << executionTime << " seconds\n\n";
    
    logFile << "OPTIMAL SEQUENCE:\n";
    logFile << "----------------\n";
    logFile << "Moves: ";
    for (size_t i = 0; i < optimalSequence.size(); i++) {
        switch(optimalSequence[i]) {
            case 0: logFile << "S"; break;   // S
            case 1: logFile << "L"; break;   // L
            case -1: logFile << "R"; break;  // R
            case 2: logFile << "U"; break;   // U
            case -2: logFile << "D"; break;  // D
        }
        if (i < optimalSequence.size() - 1) logFile << "-";
    }
    logFile << "\n\n";
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    logFile << "Simulation finished: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n";
    logFile << std::string(70, '=') << "\n";
    
    logFile.close();
}


// プログレスバー表示関数
void showProgressBar(int current, int total, int width = 50) {
    float progress = (float)current / total;
    int pos = width * progress;
    
    std::cout << "[";
    for (int i = 0; i < width; ++i) {
        if (i < pos) std::cout << "█";
        else if (i == pos) std::cout << "▶";
        else std::cout << " ";
    }
    std::cout << "] " << std::setw(5)<< std::fixed << std::setprecision(1) << progress * 100.0 << "%";
}

// 統計情報計算関数
int main() {

    std::vector<int> optimal_sequence;
    bool is3D = false;

    setting(is3D, input, GEN_MAX, N, S_limit, input, GEN_MAX, N, S_limit);
    
    // ログファイルを初期化
    std::string logFileName = createLogFileName();
    initializeLog(logFileName, is3D, input, input.size()-2, N, GEN_MAX, S_limit);
    
    std::cout << "\n";
    std::cout << BOLD  << "╔══════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD  << "║" << RESET << BOLD << "              PROTEIN FOLDING OPTIMIZATION                " << RESET << BOLD << "║" << RESET << "\n";
    std::cout << BOLD  << "║" << RESET << BOLD << "                 Using ABC Algorithm                      " << RESET << BOLD << "║" << RESET << "\n";
    std::cout << BOLD  << "╚══════════════════════════════════════════════════════════╝" << RESET << "\n";
    
    int  sequence_length = input.size()-2;
    int input_length = input.size();
    const int max_chars_per_line = 50;

    std::cout << "\n" << BOLD  << "=== Parameters ===" << RESET << "\n";
    std::cout << "Input Sequence: \n";
    int tmp_idx = 0;
    while (tmp_idx < input_length) {
        if (tmp_idx + max_chars_per_line < input_length) {
            std::cout << "   " << input.substr(tmp_idx, max_chars_per_line) << "\n";
            tmp_idx += max_chars_per_line;
        } else {
            std::cout << "   " << input.substr(tmp_idx) << "\n";
            tmp_idx += input.length() - tmp_idx; // Move to the end
        }
    }
    std::cout << "Input Length: " << input_length << "\n";
    std::cout << "Population Size: " << N << " \n";
    std::cout << "Generations: " << GEN_MAX << "\n";
    std::cout << "Trial Limit: " << S_limit << "\n";
    std::cout << "Optimization Mode: " << (is3D ? "3D" : "2D") << "\n";
    

    auto start = std::chrono::high_resolution_clock::now();
    
    int optimal_fval = -1; //水素結合の数の最大値
    //ランダムな構造をN個生成
    for (int i = 0; i < N; i++) {
        if (is3D) {
            population[i] = GenerateRandomSequence3D(input, sequence_length);
        } else {
            population[i] = GenerateRandomSequence2D(input, sequence_length);
        }
        population_fval[i] = FitnessFunction(population[i], input,  sequence_length);
        population_s[i] = 0;
        
        // 初期最適値更新
        if (population_fval[i] > optimal_fval) {
            optimal_fval = population_fval[i];
            optimal_sequence = population[i];
        }
    }
    
    // 初期統計情報をログに記録
    Statistics initialStats = calculateStatistics();
    logGenerationStats(0, initialStats);
    
    //次世代を生成
    std::cout << BOLD << "\n=== Evolution Process ===\n\n" << RESET;
    std::cout << "Generating initial population ...";
    std::cout << "\nInitial population generated\n";
    std::cout << "Initial best fitness: " << optimal_fval << " hydrogen bonds\n";
    std::cout << "Log file: " << logFileName << "\n\n";
    

    for (int generation = 1; generation < GEN_MAX+1; generation++) {
        // 現在の統計情報を計算
        Statistics stats = calculateStatistics();
        
        // ヘッダー情報表示 - 幅を調整してずれを修正
        std::cout << "\rGeneration " << std::setw(4) << generation << "/" << GEN_MAX;
        std::cout << " │ ";
        
        // プログレスバー表示
        showProgressBar(generation, GEN_MAX, 30);
        std::cout << " │ ";
        
        // 統計情報表示 
        std::cout << "Best: " << std::setw(2) << (int)stats.max;
        std::cout << " │ Avg: " << std::fixed << std::setprecision(1) << std::setw(4) << stats.mean;
        std::cout << " │ Conv: " << std::setw(3) << stats.convergence_count << "/" << N;
        
        std::cout.flush();

        for (int j =0; j < N; j++) {
            if (is3D) {
                Employed3D(input,  sequence_length, j);
            } else {
                Employed2D(input,  sequence_length, j);
            }
        }

        for (int j = 0; j < N; j++) {
            if (is3D) {
                Onlook3D(input,  sequence_length);
            } else {
                Onlook2D(input,  sequence_length);
            }
        }
        for (int j = 0; j < N; j++) {
            if (is3D) {
                Scout3D(input,  sequence_length, j);
            } else {
                Scout2D(input,  sequence_length, j);
            }
        }
        
        int fval = *std::max_element(population_fval.begin(), population_fval.end());
        bool newBest = false;
        if ( fval > optimal_fval) {
            optimal_fval = fval;
            optimal_sequence = population[std::distance(population_fval.begin(), std::max_element(population_fval.begin(), population_fval.end()))];
        }
        
        // 現在の統計情報をログに記録
        Statistics currentStats = calculateStatistics();
        logGenerationStats(generation, currentStats, newBest);
        
        // 詳細統計を一定間隔で表示
        if (generation % 10 == 0 || generation == GEN_MAX) {
            Statistics detailed_stats = calculateStatistics();
            std::cout << "\n  Detailed Statistics:";
            std::cout << " Max=" << detailed_stats.max;
            std::cout << " Min=" << detailed_stats.min;
            std::cout << " Mean=" << std::fixed << std::setprecision(2) << detailed_stats.mean;
            std::cout << " StdDev=" << std::setprecision(2) << detailed_stats.stddev;
            std::cout << " Convergence=" << detailed_stats.convergence_count << "/" << N;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "\n\n" <<  "Simulation finished\n\n" << RESET;
    
    // 結果をボックスで囲んで表示
    const int box_width = 58; // ボックスの内側の幅
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    SIMULATION RESULTS                    ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════╣\n";
    
    // Configuration section
    std::cout << "║ Configuration:" << std::string(43, ' ') << "║\n";
    
    // Input Sequence handling
    std::cout << "║   Input Sequence:" << std::string(40, ' ') << "║\n";
    
    // Handle input sequence of arbitrary length with proper line wrapping
    
    std::string padding_left = std::string(5, ' ');
    tmp_idx = 0;
    while (tmp_idx < input_length) {
        if (tmp_idx+max_chars_per_line < input_length) {
            std::string seq_line_content = padding_left + input.substr(tmp_idx, max_chars_per_line);
            int padding = box_width - seq_line_content.length();
            std::cout << "║" << seq_line_content << std::string(padding, ' ') << "║\n";
            tmp_idx += max_chars_per_line;
        } else {
            std::string seq_line_content = padding_left + input.substr(tmp_idx);
            int padding = box_width - seq_line_content.length();
            std::cout << "║" << seq_line_content << std::string(padding, ' ') << "║\n";
            tmp_idx += input.length() - tmp_idx; // Move to the end
        }
    }
    
    // Other configuration items
    std::stringstream ss;
    ss << "   Input Length: " << input_length;
    std::string seq_len_str = ss.str();
    int padding = box_width - seq_len_str.length();
    std::cout << "║" << seq_len_str << std::string(padding, ' ') << "║\n";
    
    ss.str(""); ss << "   Population Size: " << N;
    std::string pop_str = ss.str();
    padding = box_width - pop_str.length();
    std::cout << "║" << pop_str << std::string(padding, ' ') << "║\n";
    
    ss.str(""); ss << "   Max Generations: " << GEN_MAX;
    std::string gen_str = ss.str();
    padding = box_width - gen_str.length();
    std::cout << "║" << gen_str << std::string(padding, ' ') << "║\n";
    
    ss.str(""); ss << "   Trial Limit: " << S_limit;
    std::string trial_str = ss.str();
    padding = box_width - trial_str.length();
    std::cout << "║" << trial_str << std::string(padding, ' ') << "║\n";
    
    ss.str(""); ss << "   Optimization Mode: " << (is3D ? "3D" : "2D");
    std::string mode_str = ss.str();
    padding = box_width - mode_str.length();
    std::cout << "║" << mode_str << std::string(padding, ' ') << "║\n";
    
    std::cout << "╠══════════════════════════════════════════════════════════╣\n";
    
    // Results section
    std::cout << "║ Results:" << std::string(49, ' ') << "║\n";
    
    ss.str(""); ss << "   Maximum Hydrogen Bonds: " << optimal_fval;
    std::string max_str = ss.str();
    padding = box_width - max_str.length();
    std::cout << "║" << max_str << std::string(padding, ' ') << "║\n";
    
    double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    duration *= 1e-9;
    ss.str(""); ss << "   Execution Time: " << std::fixed << std::setprecision(3) << duration << " sec";
    std::string time_str = ss.str();
    padding = box_width - time_str.length();
    std::cout << "║" << time_str << std::string(padding, ' ') << "║\n";
    
    std::cout << "╚══════════════════════════════════════════════════════════╝\n\n";
    
    // 最終統計情報をログファイルに記録
    Statistics finalStats = calculateStatistics();
    finalizeLog(finalStats, duration, optimal_sequence);
    
    // ログファイル書き込み完了メッセージ
    std::cout << "Simulation log has been written to: " << logFileName << "\n\n";

    std::cout << "Opening 3D visualization...\n";

    display(input, calPosArray(optimal_sequence, sequence_length));
    
    return 0;
}

