#include "main.h"

int main(){
    puts("Starting monitors...");
    micro_sleep(1000000);

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
            perror("fork2");
            exit(EXIT_FAILURE);
        }
        else if(pid2 == 0){
            int status3;
            pid_t pid3 = fork();
            if(pid3 < 0){
                perror("fork3");
                exit(EXIT_FAILURE);
            }
            else if(pid3 == 0) availability_monitor();
            else {
                enough_money_monitor();
                waitpid(pid3, &status3, 0);
            }
        }
        else {
            int status4;
            pid_t pid4 = fork();
            if(pid4 < 0){
                perror("fork4");
                exit(EXIT_FAILURE);
            }
            else if(pid4 == 0) access_monitor();
            else {
                delivery_time_monitor();
                waitpid(pid4, &status4, 0);
            }
            waitpid(pid2, &status2, 0);
        }
    }
    else {
        update_order_monitor();
        waitpid(pid, &status, 0);
    }
    puts("Stopping monitors");
    return 0;
}