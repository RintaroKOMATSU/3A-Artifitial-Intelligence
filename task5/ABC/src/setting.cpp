#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include "setting.h"
#include "decode.h"

bool isPositiveInteger(const std::string& str) {
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    if (str.size() == 1 && str[0] == '0') {
        return false;
    }
    return !str.empty();
}

enum Mode { MODE_2D, MODE_3D };

int getKey() {
    struct termios oldt, newt;
    int ch;
    
    tcgetattr(STDIN_FILENO, &oldt); // 現在の端末設定を保存
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // バッファリングを無効化 & エコーをオフ
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar(); // 1 文字取得

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // 設定を元に戻す
    return ch;
}

void displaySelection(Mode mode) {

    std::string option2D = "2D";
    std::string option3D = "3D";

    if (mode == MODE_2D) {
        option2D = "\033[34;4m2D\033[0m"; // 青色 & 下線
    } else {
        option3D = "\033[34;4m3D\033[0m"; // 青色 & 下線
    }

    std::cout << "\r\033[K\033[32m? \033[0m\033[1mSelect the dimension for the optimization:\033[0m " ;
    std::cout  << option2D << " / " << option3D;

}


void setting(
    bool& is3D,
    std::string& inputArray, 
    int& generations, 
    int& population, 
    int& maxTrials, 
    std::string defaultInputArray, 
    int defaultGenerations, 
    int defaultPopulation, 
    int defaultMaxTrials
){
    for (int i = 0; i < 20; i++) {
        std::cout << "\n";
    }
    std::cout << "\033[20A";
    std::string dimensionStr;
    std::string decodedInput;
    std::string generationsStr;  // 世代数（文字列から整数に変換）
    std::string populationStr;   // 個体数（文字列から整数に変換）
    std::string maxTrialsStr;    // Scout Bee になる間での試行回数の上限（文字列から整数に変換）


    std::cout << "Please configure the simulation settings. If you want to use the default value, just press Enter." << std::endl;

    Mode mode = MODE_2D;
    displaySelection(mode);
    
    //次元の選択
    while (true) {
        int key = getKey();
        
        if (key == 27) { // ESCシーケンスの始まり
            if (getKey() == 91) { // '[' のコード
                switch (getKey()) {
                    case 'D': // Left Arrow (←)
                        mode = MODE_2D;
                        break;
                    case 'C': // Right Arrow (→)
                        mode = MODE_3D;
                        break;
                }
            }
        } else if (key == '\n') { // Enter
            break;
        }

        displaySelection(mode);
    }
    std::cout << "\r\033[K\033[32m✔ \033[0m\033[1mDimension for the optimization: \033[0m" << (mode == MODE_2D ? "2D" : "3D") << std::endl;
    is3D = (mode == MODE_3D);

    //入力配列の設定
    while (true) {
        std::cout << "\r\033[K\033[32m? \033[0m\033[1mEnter the input array (default = " << defaultInputArray << "): \033[0m";
        std::getline(std::cin, inputArray);
        if (inputArray.empty()) {
            inputArray = defaultInputArray;
            int p = 0;
            decodedInput = decode(inputArray, p);
            break;
        }
        int p = 0;
        decodedInput = decode(inputArray, p);
        if (decodedInput[0] == '0') {
            int errorIdx = std::stoi(decodedInput.substr(1, decodedInput.size()-1));
            std::cout << "\033[K\033[31mError: Syntax error detected. Please enter a valid input array.\n";
            for (int i = 0; i < 10; i++) {
                std::cout << " ";
            }
            std::cout << inputArray << "\n\033[K";
            for (int i = 0; i < 10; i++) {
                std::cout << " ";
            }
            for (int i = 0; i < errorIdx; i++) {
                std::cout << " ";
            }
            std::cout << "^\033";
            std::cout << "\033[0m\033[A\033[A\033[A\r\033[K";
            continue;
        } else if (decodedInput.size() == 0){
            inputArray = defaultInputArray;
            int p = 0;
            decodedInput = decode(inputArray, p);
            break;
        } else if (decodedInput.size() < 4) {
            std::cout << "\n\r\033[K\n\r\033[K\033[2A";
            std::cout << "\033[31mError: The input array is too short to create a hydrogen bond in the lattice model. Please enter a input array with a length of more than 4.\033[0m\033[A\r\033[K";
            continue;
        }
        break;
    }
    std::cout << "\n\r\033[K\n\r\033[K\033[1A\r\033[K\033[1A\r\033[K";
    std::cout << "\033[1A\r\033[K\033[32m✔ \033[0m\033[1mInput array: \033[0m" << inputArray << std::endl;
    inputArray = decodedInput;


    //世代数の設定
    while (true) {
        std::cout << "\r\033[K\033[32m? \033[0m\033[1mEnter the number of generations (default=" << defaultGenerations << "): \033[0m";
        std::getline(std::cin, generationsStr);
        if (generationsStr.empty()) {
            generationsStr = std::to_string(defaultGenerations);
        }
        if (isPositiveInteger(generationsStr)) {
            std::cout << "\033[A\r\033[K\033[32m✔ \033[0m\033[1mGenerations: \033[0m"+generationsStr << std::endl;
            break;
        } else {
            std::cerr << "\033[31mError: Invalid input. Please enter a positive integer for the number of generations.\033[0m\033[A\r\033[K";
        }
    }


    // 個体数の設定
    while (true) {
        std::cout << "\r\033[K\033[32m? \033[0m\033[1mEnter the population size (default = " << defaultPopulation << "): \033[0m";
        std::getline(std::cin, populationStr);
        if (populationStr.empty()) {
            populationStr = std::to_string(defaultPopulation);
        }
        if (isPositiveInteger(populationStr)) {
            std::cout << "\033[A\r\033[K\033[32m✔ \033[0m\033[1mPopulation size: \033[0m"+populationStr << std::endl;
            break;
        } else {
            std::cerr << "\033[31mError: Invalid input. Please enter a positive integer for the number of population.\033[0m\033[A\r\033[K";
        }
    }


    // Scout Bee になる間での試行回数の上限の設定
    while (true) {
        std::cout << "\r\033[K\033[32m? \033[0m\033[1mEnter the max trials for Scout Bee (default = " << defaultMaxTrials << "): \033[0m";
        std::getline(std::cin, maxTrialsStr);
        if (maxTrialsStr.empty()) {
            maxTrialsStr = std::to_string(defaultMaxTrials);
        }
        if (isPositiveInteger(maxTrialsStr)) {
            std::cout << "\033[A\r\033[K\033[32m✔ \033[0m\033[1mMax trials for Scout Bee: \033[0m"+maxTrialsStr;
            break;
        } else {
            std::cerr << "\033[31mError: Invalid input. Please enter a positive integer for max trials.\033[0m\033[A\r\033[K";
        }
    }
    std::cout << "\n\r\033[K";

    // 入力の読み込み
    generations = std::stoi(generationsStr);
    population = std::stoi(populationStr);
    maxTrials = std::stoi(maxTrialsStr);
}