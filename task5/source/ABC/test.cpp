#include <iostream>
#include <termios.h>
#include <unistd.h>

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

    std::cout << "Select the dimension for the simulation: " ;
    std::cout  << option2D << " / " << option3D << std::endl;

}

int main() {
    Mode mode = MODE_2D;
    displaySelection(mode);

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

    std::cout << "You selected: " << (mode == MODE_2D ? "2D" : "3D") << std::endl;
    return 0;
}
