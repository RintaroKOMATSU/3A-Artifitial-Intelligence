#include <iostream>
#include <string>
#include "decode.h"

static void syntax_error(const std::string& input, const std::string::const_iterator& p) {
    int idx = std::distance(input.begin(), p);
    std::cerr << "  ";
    for (int i = 0; i < idx; i++) {
        std::cerr << " ";
    }
    std::cerr << "^\n";
    std::cerr << "syntax error " << "\n";
    exit(1);
}

std::string decode(const std::string input) {
    static auto p = input.begin();
    std::string output = "";
    while (true) {
        switch (*p) {
        case 'H':{
            p++;
            while (*p == ' ') {
                p++;
            }
            if ('1' <= *p && *p <= '9') {
                std::string n = "";
                while ('0' <= *p && *p <= '9') {
                    n+= *p;
                    p++;
                    while (*p == ' ') {
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
            while (*p == ' ') {
                p++;
            }
            if ('1' <= *p && *p <= '9') {
                std::string n = "";
                while ('0' <= *p && *p <= '9') {
                    n+= *p;
                    p++;
                    while (*p == ' ') {
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
            while (*p == ' ') {
                p++;
            }
            std::string a = decode(input);
            switch (*p) {
            case '1' ... '9': {
                std::string n = "";
                while ('0' <= *p && *p <= '9') {
                    n+= *p;
                    p++;
                }
                for (int i = 0; i < stoi(n) ; i++) {
                    output += a;
                }
                break;
            }
            default:
                syntax_error(input, p);
            }
        break;
        }
        case ')': {
            p++;
            while (*p == ' ') {
                p++;
            }
            return output;
        }
        case ' ': {
            p++;
            break;
        }
        default:
            syntax_error(input, p);
        }
        if (p == input.end()) {
            return output;
        }
    }  
}

