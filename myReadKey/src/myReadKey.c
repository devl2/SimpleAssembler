#include "myReadKey.h"

static struct termios orig_term;

int rk_readkey (enum keys *key){
    static int initialized = 0;
    if (!initialized){
        rk_mytermregime(1, 0, 1, 0, 1);
        initialized = 1;
    }
    char buffer[8] = {0};
    int byte_read = read(STDIN_FILENO, buffer, sizeof(buffer));

    if (byte_read <= 0) return -1;

    if (buffer[0] == '\033'){
        if (byte_read >= 3 && buffer[1] == '['){
            switch (buffer[2]){
            case 'A':
                *key = N_KEY_UP;
                return 0;
            case 'B':
                *key = N_KEY_DOWN;
                return 0;
            case 'C':
                *key = N_KEY_RIGHT;
                return 0;
            case 'D':
                *key = N_KEY_LEFT;
                return 0;
            case '1':
                if (buffer [3] == '5' && buffer[4] == '~'){
                    *key = N_KEY_F5;
                }
                if (buffer [3] == '7' && buffer[4] == '~'){
                    *key = N_KEY_F6;
                }
            break;
            }
        }
        *key = N_KEY_ESC;
        return 0;
    }

    switch (buffer[0]){
        case '\n':
        case '\r':
            *key = N_KEY_ENTER;
            return 0;
        case 'l':
            *key = N_KEY_L;
            return 0;
        case 's':
            *key = N_KEY_S;
            return 0;
        case 'i':
            *key = N_KEY_I;
            return 0;
        case '0':
            *key = N_KEY_0;
            return 0;
        case '1':
            *key = N_KEY_1;
            return 0;
        case '2':
            *key = N_KEY_2;
            return 0;
        case '3':
            *key = N_KEY_3;
            return 0;
        case '4':
            *key = N_KEY_4;
            return 0;
        case '5':
            *key = N_KEY_5;
            return 0;
        case '6':
            *key = N_KEY_6;
            return 0;
        case '7':
            *key = N_KEY_7;
            return 0;
        case '8':
            *key = N_KEY_8;
            return 0;
        case '9':
            *key = N_KEY_9;
            return 0;
        case '+':
            *key = N_KEY_PLUS;
            return 0;
        case '-':
            *key = N_KEY_MINUS;
            return 0;
        case 127:
            *key = N_KEY_BACKSPACE;
            return 0;
        case 't':
            *key = N_KEY_T;
            return 0;
        case 'r':
            *key = N_KEY_R;
            return 0;
        case 'a':
            *key = N_KEY_A;
            return 0;
        case 'b':
            *key = N_KEY_B;
            return 0;
        case 'c':
            *key = N_KEY_C;
            return 0;
        case 'd':
            *key = N_KEY_D;
            return 0;
        case 'e':
            *key = N_KEY_E;
            return 0;
        case 'f':
            *key = N_KEY_F;
            return 0;
        case 'x':
            *key = N_KEY_X;
            return 0;
        default:
            *key = N_KEY_OTHER;
            return 0;
        }
}

int rk_mytermsave (void) {
    if(tcgetattr(STDIN_FILENO, &orig_term) == -1){
        return -1;
    }
    return 0;
}

int rk_mytermrestore (void){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term) == -1){
        return -1;
    }
    return 0;
}

int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint) {
    struct termios new_term;
    
    if (tcgetattr(STDIN_FILENO, &new_term) == -1) {
        return -1;
    }

    if (regime) {
        new_term.c_lflag &= ~(ICANON | ECHO);
        new_term.c_cc[VTIME] = vtime;
        new_term.c_cc[VMIN] = vmin;
    } else {
        new_term.c_lflag |= ICANON;
    }

    if (echo) {
        new_term.c_lflag |= ECHO;
    } else {
        new_term.c_lflag &= ~ECHO;
    }

    if (sigint) {
        new_term.c_lflag |= ISIG;
    } else {
        new_term.c_lflag &= ~ISIG;
    }

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term) == -1) {
        return -1;
    }
    return 0;
}

int rk_readvalue(int *value, int timeout) {
    char buffer[32] = {0};
    int pos = 0;
    char c;
    int ret;

    if (rk_mytermregime(1, timeout/100, 0, 1, 1) == -1) {
        return -1;
    }

    while (1) {
        ret = read(STDIN_FILENO, &c, 1);
        if (ret == -1) {
            rk_mytermrestore();
            return -1;
        }
        if (ret == 0) {
            rk_mytermrestore();
            return -2;
        }

        if (isdigit(c)) {
            buffer[pos++] = c;
            write(STDOUT_FILENO, &c, 1);
        } else if (c == '\n' || c == '\r') {
            break;
        } else if (c == 127 || c == 8) {
            if (pos > 0) {
                pos--;
                write(STDOUT_FILENO, "\b \b", 3);
            }
        }
    }

    buffer[pos] = '\0';
    *value = atoi(buffer);
    rk_mytermrestore();
    return 0;
}