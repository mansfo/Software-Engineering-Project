#include "main.h"

#define CUST_READ_STREAM "cust_read_stream_0"
#define CUST_WRITE_STREAM "cust_write_stream_0"
#define CARR_READ_STREAM "carr_read_stream_0"
#define CARR_WRITE_STREAM "carr_write_stream_0"
#define SUPPL_READ_STREAM "suppl_read_stream_0"
#define SUPPL_WRITE_STREAM "suppl_write_stream_0"

int main(){
    puts("Starting servers...");
    micro_sleep(1000000);
    
    redisContext *c2r = redisConnect("localhost", 6379);
    redisReply* reply;

    reply = RedisCommand(c2r, "DEL %s", CUST_READ_STREAM);
    assertReply(c2r, reply);

    reply = RedisCommand(c2r, "DEL %s", CUST_WRITE_STREAM);
    assertReply(c2r, reply);

    reply = RedisCommand(c2r, "DEL %s", CARR_READ_STREAM);
    assertReply(c2r, reply);

    reply = RedisCommand(c2r, "DEL %s", CARR_WRITE_STREAM);
    assertReply(c2r, reply);

    reply = RedisCommand(c2r, "DEL %s", SUPPL_READ_STREAM);
    assertReply(c2r, reply);

    reply = RedisCommand(c2r, "DEL %s", SUPPL_WRITE_STREAM);
    assertReply(c2r, reply);

    reply = RedisCommand(c2r, "DEL %s", AUX_STREAM);
    assertReply(c2r, reply);

    initStreams(c2r, CARR_READ_STREAM);
    initStreams(c2r, CARR_WRITE_STREAM);
    initStreams(c2r, CUST_READ_STREAM);
    initStreams(c2r, CUST_WRITE_STREAM);
    initStreams(c2r, SUPPL_READ_STREAM);
    initStreams(c2r, SUPPL_WRITE_STREAM);
    initStreams(c2r, AUX_STREAM);

    freeReplyObject(reply);
    redisFree(c2r);
    
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
        else if(pid2 ==0) carrier_server();
        else {
            supplier_server();
            waitpid(pid2, &status2, 0);
        }
    }
    else {
        customer_server();
        waitpid(pid, &status, 0);
    }
    puts("Stopping servers...");
    return 0;
}