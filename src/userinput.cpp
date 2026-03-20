#include <userinput.hpp>
#if defined(_WIN32)
#include <conio.h>
#endif
#if defined(__linux__)
#include <unistd.h>
#include <termios.h>
int getch() {
    struct termios old_settings, new_settings;
    int ch;
    tcgetattr(STDIN_FILENO, &old_settings);
    new_settings = old_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
    return ch;
}
#endif
InputType GetPlayerInput() {
    int ch =
#ifdef _WIN32
        _getch();
#else
        getch();
#endif
#ifdef _WIN32
    if (ch == 13) return InputType::Enter;
    if (ch == 27) return InputType::Escape;
    if (ch == 0 || ch == 0xE0) {
        switch (_getch()) {
        case 72: return InputType::MoveUp;
        case 80: return InputType::MoveDown;
        case 75: return InputType::MoveLeft;
        case 77: return InputType::MoveRight;
        }
        return InputType::None;
    }
#else
    if (ch == 10 || ch == 13) return InputType::Enter;
    if (ch == 27) {
        int next = getch();
        if (next == 91) {
            switch (getch()) {
            case 'A': return InputType::MoveUp;
            case 'B': return InputType::MoveDown;
            case 'C': return InputType::MoveRight;
            case 'D': return InputType::MoveLeft;
            }
            return InputType::None;
        }
        return InputType::Escape;
    }
#endif
    if (ch >= '0' && ch <= '9') {
        return static_cast<InputType>(
            static_cast<int>(InputType::Top0) + (ch - '0')
            );
    }
    switch (ch) {
    case 'd': case 'D': return InputType::D;
    case 'r': case 'R': return InputType::R;
    case 'q': case 'Q': return InputType::Q;
    case 'c': case 'C': return InputType::C;
    case 'p': case 'P': return InputType::P;
    case 'v': case 'V': return InputType::V;
    case 'n': return InputType::n;
    case 'N': return InputType::N;
    }
    return InputType::None;
}
#if defined(_WIN32)
int readKey() {
    int ch = _getch();
    if (ch == 0 || ch == 224) {
        int ch2 = _getch();
        return 1000 + ch2;
    }
    return ch;
}
#else
int readKey() {
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int ch = getchar();
    if (ch == 27) {
        if (getchar() == '[') {
            ch = 1000 + getchar();
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif