#include "main.h"

int main(){
    setvbuf(stdout, (char*) NULL, _IONBF, 0);
    setvbuf(stderr, (char*) NULL, _IONBF, 0);
    int status;
    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid==0){
        int status2;
        pid_t pid2 = fork();
        if(pid2 < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if(pid2 == 0){
            const char executable2[] = "../../monitor/bin/main";
            execl(executable2, executable2, NULL);
        }
        else{
            const char executable3[] = "../../test/bin/main";
            execl(executable3, executable3, NULL);
            waitpid(pid2, &status2, 0);
        }
    }
    else{
        const char executable[] = "../../server/bin/main";
        execl(executable, executable, NULL);
        waitpid(pid, &status, 0);
    }
    return status;
}