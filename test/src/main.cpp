#include "main.h"

int main(){
    puts("Starting tests...");
    micro_sleep(3000000);
    int status;
    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        int status2;
        pid_t pid2 = fork();
        if(pid2 < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if(pid2 == 0) supplier_test();
        else{
            customer_test();
            waitpid(pid2, &status2, 0);
        }
    }
    else{
        carrier_test();
        waitpid(pid, &status, 0);
    }
    return 0;
}