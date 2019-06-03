#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define TRUE 1 
#define FALSE 0

typedef unsigned short bool;

int wait_for_lock(char* lckfilename) {
    int i = 0;
    for (; i < 100; i++) {
        if (access(lckfilename, F_OK) != -1) {
            printf("Ожидаем отмены блокировки: %d секунд\n", i);
            sleep(1);
        } else {
            return i;
        }
    }
}

void write_lock(FILE* lockfile, bool is_write) {
    fprintf(lockfile, "%d\n", getpid());
    fprintf(lockfile, is_write ? "w" : "r");
}

void slp() {
    for (int i = 0; i < 5; i++) {
        printf("Спим: %d\n", i);
        sleep(1);
    }
}

void wrt(int argc, char* argv[], char* wfilename) {
    printf("Запись\n");
    FILE* wfile = fopen(wfilename, "a");
    if (wfile == NULL) {
        printf("Не удалось открыть файл для записи\n");
        return;
    }
    fprintf (wfile, "\n");
    for (int i = 2; i < argc; i++) {
        fprintf(wfile, "%s ", argv[i]);
    }
    if (fclose(wfile) == -1) {
        printf("Не удалось закрыть файл");
        return;
    }
}

void rd(int argc, char* argv[], char* rfilename) {
    printf("Чтение\n");
    FILE* rfile = fopen(rfilename, "r");
    if (rfile == NULL) {
        printf("Не удалось открыть файл для чтения\n");
        return;
    }
    char ch;
    while((ch = fgetc(rfile)) != EOF) {
        printf("%c", ch);
    }
    if (fclose(rfile) == -1) {
        printf("Не удалось закрыть файл");
        return;
    }
}

void trytoreadwrite(int argc, char* argv[], bool is_write) {
    slp();
    if (is_write) {
        wrt(argc, argv, argv[1]);
    } else {
        rd(argc, argv, argv[1]);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Укажите имя файла\n");
        return 1;   
    }

    bool is_write = FALSE;
    if (argc > 2) {
        is_write = TRUE;
    }

    char lckfilename[strlen(argv[1] + 4)];
    char* extension = ".lck";
    strcpy(lckfilename, argv[1]);
    strcat(lckfilename, extension);

    int sec = wait_for_lock(lckfilename);
    if (sec >= 100) {
        printf("Не дождались снятия блокировки\n");
        return 1;
    }

    FILE* lockfile = fopen(lckfilename, "wa");
    if (lockfile == NULL) {
        printf("Не удалось получить доступ к файлу блкировки\n");
        return 1;
    }
    write_lock(lockfile, is_write);
    fclose(lockfile);

    trytoreadwrite(argc, argv, is_write);

    if (remove(lckfilename) == -1) {
        printf("Не удалось снять блокировку\n");
        return 1;
    }

    return 0;
}