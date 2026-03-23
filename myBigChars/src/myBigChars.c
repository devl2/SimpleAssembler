#include "myBigChars.h"

int bc_strlen(char *str) {
    if (!str) return 0;
    
    setlocale(LC_ALL, "en_US.utf8");
    mbstate_t state = {0};
    const char *ptr = str;
    size_t len = 0;
    
    while (*ptr) {
        wchar_t wc;
        size_t bytes = mbrtowc(&wc, ptr, MB_CUR_MAX, &state);
        
        if (bytes == (size_t)-1 || bytes == (size_t)-2) {
            return 0;
        }
        
        if (bytes == 0) break;
        
        ptr += bytes;
        len++;
    }
    
    return (int)len;
}

int bc_printA(char *str) {
    if (!str) return -1;
    
    if (bc_strlen(str) == 0) return -1;
    
    printf("%s", str);
    return 0;
}

int bc_box(int x1, int y1, int x2, int y2, enum my_colors box_fg, enum my_colors box_bg, 
           char *header, enum my_colors header_fg, enum my_colors header_bg) {
    if (x2 < 2 || y2 < 2) return -1;
    
    printf("\033[s");
    
    printf("\033[%d;%dm", box_fg + 30, box_bg + 40);
    
    printf("\033[%d;%dH┌", x1, y1);
    for (int i = 1; i < y2 - 1; i++) printf("─");
    printf("┐");
    
    for (int i = 1; i < x2 - 1; i++) {
        printf("\033[%d;%dH│", x1 + i, y1);
        printf("\033[%d;%dH│", x1 + i, y1 + y2 - 1);
    }
    
    printf("\033[%d;%dH└", x1 + x2 - 1, y1);
    for (int i = 1; i < y2 - 1; i++) printf("─");
    printf("┘");
    
    if (header && *header) {
        int header_len = bc_strlen(header);
        if (header_len > 0) {
            int header_pos = y1 + (y2 - header_len) / 2;
            if (header_pos < y1 + 1) header_pos = y1 + 1;
            
            printf("\033[%d;%dH\033[%d;%dm", x1, header_pos, header_fg + 30, header_bg + 40);
            
            printf("%s", header);
        }
    }
    
    printf("\033[0m");
    printf("\033[u");
    
    return 0;
}

int bc_setbigcharpos(int *big, int x, int y, int value) {
    if (!big || x < 0 || x >= 8 || y < 0 || y >= 8) return -1;
    
    int index = x / 4;
    int bit_pos = (x % 4) * 8 + y;
    
    if (value) {
        big[index] |= (1 << bit_pos);
    } else {
        big[index] &= ~(1 << bit_pos);
    }
    
    return 0;
}

int bc_getbigcharpos(int *big, int x, int y, int *value) {
    if (!big || !value || x < 0 || x >= 8 || y < 0 || y >= 8) return -1;
    
    int index = x / 4;
    int bit_pos = (x % 4) * 8 + y;
    
    *value = (big[index] >> bit_pos) & 1;
    return 0;
}

int bc_printbigchar(int big[2], int x, int y, enum my_colors fg, enum my_colors bg,WINDOW * charwin) {
    if (!big) return -1;
    
    printf("\033[s");
    
    printf("\033[%d;%dm", fg + 30, bg + 40);
    
    for (int i = 0; i < 8; i++) {
        int value;
        if (bc_getbigcharpos(big, i, 0, &value) == -1) return -1;
        
        printf("\033[%d;%dH", x + i, y);
        
        for (int j = 0; j < 8; j++) {
            if (bc_getbigcharpos(big, i, j, &value) == -1) return -1;
            printf(value ? "█" : " ");
        }
    }
    
    printf("\033[0m");
    printf("\033[u");
    
    return 0;
}

int bc_bigcharwrite(int fd, int *big, int count) {
    if (fd < 0 || !big || count <= 0) return -1;
    
    ssize_t written = write(fd, big, count * 2 * sizeof(int));
    if (written != (ssize_t)(count * 2 * sizeof(int))) return -1;
    
    return 0;
}

int bc_bigcharread(int fd, int *big, int need_count, int *count) {
    if (fd < 0 || !big || need_count <= 0 || !count) return -1;
    
    *count = 0;
    ssize_t bytes_read = read(fd, big, need_count * 2 * sizeof(int));
    
    if (bytes_read <= 0) return -1;
    if (bytes_read % (2 * sizeof(int)) != 0) return -1;
    
    *count = bytes_read / (2 * sizeof(int));
    return 0;
}