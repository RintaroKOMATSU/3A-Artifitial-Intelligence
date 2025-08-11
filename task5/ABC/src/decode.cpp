#include <iostream>
#include <string>
#include "decode.h"

static int syntax_error(const std::string& input, const int p) {
    return p;
}

std::string decode(const std::string input, int& p) {
    std::string output = "";
    int error_idx;
    while (true) {
        switch (input[p]) {
        case 'H':{
            p++;
            while (input[p] == ' ') {
                p++;
            }
            if ('1' <= input[p] && input[p] <= '9') {
                std::string n = "";
                while ('0' <= input[p] && input[p] <= '9') {
                    n+= input[p];
                    p++;
                    while (input[p] == ' ') {
                        p++;
                    }
                }
                for (int i = 0; i < stoi(n) ; i++) {
                    output += 'H';
                }
            } else {
                output += 'H';
            }
            break;
        }
        case 'P': {
            p++;
            while (input[p] == ' ') {
                p++;
            }
            if ('1' <= input[p] && input[p] <= '9') {
                std::string n = "";
                while ('0' <= input[p] && input[p] <= '9') {
                    n+= input[p];
                    p++;
                    while (input[p] == ' ') {
                        p++;
                    }
                }
                for (int i = 0; i < stoi(n) ; i++) {
                    output += 'P';
                }
            } else {
                output += 'P';
            }
            break;
        }
        case '(': {
            p++;
            while (input[p] == ' ') {
                p++;
            }
            std::string a = decode(input, p);
            switch (input[p]) {
            case '1' ... '9': {
                std::string n = "";
                while ('0' <= input[p] && input[p] <= '9') {
                    n+= input[p];
                    p++;
                }
                for (int i = 0; i < stoi(n) ; i++) {
                    output += a;
                }
                break;
            }
            case 'H':
            case 'P': {
                output += a;
                break;
            }
            default:
                error_idx =  syntax_error(input, p);
                return "0"+std::to_string(error_idx);
            }
        break;
        }
        case ')': {
            p++;
            while (input[p] == ' ') {
                p++;
            }
            return output;
        }
        case ' ': {
            p++;
            break;
        }
        default:
            error_idx = syntax_error(input, p);
            return "0"+std::to_string(error_idx);
        }
        if (p == (int)input.size()) {
            return output;
            
        }
    }  
}

