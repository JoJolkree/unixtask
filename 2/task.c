#include <stdio.h>
#include <unistd.h>

#define TRUE 1 
#define FALSE 0

typedef unsigned short bool;

void handle(char* filename) {
    const int bufsize = 1024;

    char rdbuf[bufsize];
    char wrbuf[bufsize];

    int bytes = 0;
    FILE* file = fopen(filename, "wb");

    if (file == NULL) {
        printf("Ошибка при открытии файла\n");
        return;
    }

    while((bytes = read(0, rdbuf, sizeof(rdbuf))) > 0) {
        int file_pos = 0;
        int zeros = 0;
        bool sparsed = FALSE;

        for (int i = 0; i < bytes; i++) {
            if (rdbuf[i] == 0) {
                sparsed = TRUE;
                zeros++;
                continue;
            }
            if (sparsed) {
                fwrite(wrbuf, 1, file_pos, file);
                fseek(file, zeros, SEEK_CUR);
                file_pos = 0;
                zeros = 0;
                sparsed = FALSE;
            }
            wrbuf[file_pos] = rdbuf[i];
            file_pos++;
        }

        if (sparsed) {
            fseek(file, zeros, SEEK_CUR);        
        }

        fwrite(wrbuf, 1, file_pos, file);
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Укажите выходной файл\n");
        return 1;
    }
    handle(argv[1]);
    return 0;
}