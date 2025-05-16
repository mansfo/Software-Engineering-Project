#include "main.h"

#define WRITE_STREAM_0 "carr0_read_stream"
#define READ_STREAM_0 "carr0_write_stream"

void carrier_server(){
    redisContext* c2r = redisConnect("localhost",6379);
    Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
    log_event(logdb, getpid(), CARRIER_SERVER, "Server", "Starting carrier server", "Success", "The carrier server is started successfully!");
    redisReply *r;
    r = RedisCommand(c2r, "DEL %s", READ_STREAM_0);
    assertReply(c2r, r);
    freeReplyObject(r);
    r = RedisCommand(c2r, "DEL %s", WRITE_STREAM_0);
    assertReply(c2r, r);
    freeReplyObject(r);
    initStreams(c2r, READ_STREAM_0);
    initStreams(c2r, WRITE_STREAM_0);
    redisFree(c2r);
    c2r = nullptr;
    int status;
    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid==0){
        int status2;
        pid_t pid2 = fork();
        if(pid < 0){
            perror("fork2");
            exit(EXIT_FAILURE);
        }
        else if(pid2==0){
            redisContext* c2r = redisConnect("localhost",6379);
            Con2DB db("localhost", "5432", "ecommerce_server", "02468", "ecommerce_main_db");
            Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
            redisReply *reply;
            int time = 0;
            string rid = "";
            vector<string> rv;
            char descr[100];
            while(time < MAX_TIME){
                time++;
                if(rid.length()==0) reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS %s $", READ_STREAM_0);
                else reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS %s %s", READ_STREAM_0, rid.c_str());
                if(reply==NULL || reply->type == REDIS_REPLY_NIL) {
                    freeReplyObject(reply);
                    continue;
                }
                rv = getReply(reply);
                if(rv.size()<2) rid = string(reply->str);
                else rid = rv.at(1);
                if(!strcmp(rv.at(2).c_str(),"get")) {
                    freeReplyObject(reply);
                    continue;
                }
                if(badRequest(reply)) log_event(logdb, getpid(), CARRIER_SERVER, "Server", "Unknown", "Error", "The Carrier server received a bad formatted request");
                else if(rv.size()>3 && !strcmp(rv.at(3).c_str(),"new_carr")){
                    tuple<string,string,string,string> tup = create_carrier(db, c2r, rv);
                    freeReplyObject(reply);
                    reply = RedisCommand(c2r, "XADD %s * redis_id %s id %s readstream %s writestream %s", WRITE_STREAM_0, get<2>(tup).c_str(), get<3>(tup).c_str(), get<0>(tup).c_str(), get<1>(tup).c_str());
                    assertReply(c2r, reply);
                    if(strcmp(get<0>(tup).c_str(),"")){
                        snprintf(descr,100,"A new carrier is inserted in the database with id %s", get<3>(tup).c_str());
                        log_event(logdb, getpid(), CARRIER_SERVER, "Server", "New carrier", "Success", descr);
                    }
                    else log_event(logdb, getpid(), CARRIER_SERVER, "Server", "New carrier", "Error", "An error occurred while inserting a new carrier in the database");
                }
                freeReplyObject(reply);
            }
            redisFree(c2r);
            c2r = nullptr;
        }
        else{
            redisContext* c2r = redisConnect("localhost",6379);
            Con2DB db("localhost", "5432", "ecommerce_server", "02468", "ecommerce_main_db");
            Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
            redisReply *reply;
            int time = 0;
            string rid_s = "";
            vector<string> rv;
            unordered_map<int,string> carr_read_streams;
            unordered_map<int,string> carr_write_streams;
            unordered_map<int,int> carr_orders;
            queue<int> to_be_assigned;
            char descr[100];
            while(time < MAX_TIME){
                time++;
                carr_read_streams = get_read_streams(db,2);
                carr_write_streams = get_write_streams(db,2);
                if(rid_s.length()==0) reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS %s $", CARR_READ_SUPPL);
                else reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS %s %s", CARR_READ_SUPPL, rid_s.c_str());
                if(reply==NULL || reply->type == REDIS_REPLY_NIL) {
                    freeReplyObject(reply);
                    continue;
                }
                rv = getReply(reply);
                if(rv.size()<2) rid_s = string(reply->str);
                else rid_s = rv.at(1);
                if(badRequest(reply)) log_event(logdb, getpid(), CARRIER_SERVER, "Server", "Unknown", "Error", "The Carrier server received a bad formatted request");
                else if(rv.size()>5 && !strcmp(rv.at(3).c_str(),"new_order")){
                    to_be_assigned.push(stoi(rv.at(5)));
                    int assigned = 0, timeout = 0;
                    int ord_id = to_be_assigned.front();
                    do{
                        assigned = assign_carrier(db, ord_id, (int)carr_read_streams.size());
                        micro_sleep(5000);
                        timeout++;
                    } while(!assigned && timeout<20);
                    if(assigned){
                        to_be_assigned.pop();
                        carr_orders.insert({ord_id,assigned});
                        snprintf(descr, 500, "The order with id %d is successfully assigned to the carrier with id %d", ord_id, assigned);
                        log_event(logdb, getpid(), CARRIER_SERVER, "Server", "New order", "Success", descr);
                    }
                }
                else if(rv.size()>5 && !strcmp(rv.at(3).c_str(),"deleted_order") && carr_orders.count(stoi(rv.at(5)))>0){
                    int ord_id = stoi(rv.at(5));
                    freeReplyObject(reply);
                    reply = RedisCommand(c2r, "XADD %s * deleted_order %d", carr_read_streams.at(carr_orders.at(ord_id)).c_str(), ord_id);
                    assertReply(c2r, reply);
                    snprintf(descr, 500, "The order with id %d is successfully deleted", ord_id);
                    log_event(logdb, getpid(), CARRIER_SERVER, "Server", "Deleted order", "Success", descr);
                }
                else log_event(logdb, getpid(), CARRIER_SERVER, "Server", "Unknown", "Error","The server did not know how to deal with this input");
                freeReplyObject(reply);
            }
            waitpid(pid2, &status2,0);
            redisFree(c2r);
            c2r = nullptr;
        }
    }
    else{
        redisContext* c2r = redisConnect("localhost",6379);
        Con2DB db("localhost", "5432", "ecommerce_server", "02468", "ecommerce_main_db");
        Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
        redisReply *reply;
        reply = RedisCommand(c2r, "XADD %s * get starting_rid", AUX_STREAM);
        assertReply(c2r,reply);
        string rid = string(reply->str);
        freeReplyObject(reply);
        unordered_map<int,string> last_redis_id;
        unordered_map<int,string> carr_read_streams;
        unordered_map<int,string> carr_write_streams;
        int time = 0;
        while(time < MAX_TIME){
            time++;
            carr_read_streams = get_read_streams(db,2);
            carr_write_streams = get_write_streams(db,2);
            for(auto const& stream: carr_write_streams){
                if(last_redis_id.count(stream.first)<1) last_redis_id.insert({stream.first,rid});
                if(last_redis_id.count(stream.first)>0 && last_redis_id.at(stream.first).length()>0) reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 100 STREAMS %s %s", stream.second.c_str(), last_redis_id.at(stream.first).c_str());
                else reply = RedisCommand(c2r, "XREAD COUNT 1 STREAMS %s $", stream.second.c_str());
                assertReply(c2r, reply);
                if(reply == NULL || reply->type == REDIS_REPLY_NIL) {
                    freeReplyObject(reply);
                    continue;
                }
                if(badRequest(reply)) log_event(logdb, getpid(), CARRIER_SERVER, "Server", "Unknown", "Error", "The Carrier server received a bad formatted request");
                else if(reply->type != REDIS_REPLY_NIL){
                    vector<string> rv = getReply(reply);
                    if(rv.size()>1) {
                        if(last_redis_id.count(stream.first)<1) last_redis_id.insert({stream.first,rv.at(1)});
                        else last_redis_id.at(stream.first) = rv.at(1);
                    }
                    tuple<string,string,string,string> tup;
                    string orders;
                    pair<int,string> result;
                    pair<int,int> ord_id_dist;
                    char descr[100];
                    switch(str_to_carr_event(getMsg(reply))){
                        case carr_login:
                            tup = login_carrier(db,rv);
                            if(strcmp(get<0>(tup).c_str(), "")){
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg login name %s surname %s comp %s status %s", 
                                    carr_read_streams.at(stream.first).c_str(), get<0>(tup).c_str(), get<1>(tup).c_str(), get<2>(tup).c_str(), get<3>(tup).c_str());
                                assertReply(c2r,reply);
                                snprintf(descr, 100, "The carrier with id %d has logged in",stream.first);
                                log_event(logdb, getpid(), stream.first, "Carrier", "Login carrier", "Success", descr);
                            } 
                            else {
                                snprintf(descr, 100, "An error occurred while the carrier with id %d was logging in",stream.first);
                                log_event(logdb, getpid(), stream.first, "Carrier", "Login carrier", "Error", descr);
                            }
                            break;
                        case order_assigned:
                            ord_id_dist = get_assigned_order(db,stream.first);
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg assigned_order id %d distance %d", carr_read_streams.at(stream.first).c_str(), ord_id_dist.first, ord_id_dist.second);
                            assertReply(c2r, reply);
                            break;
                        case orders_delivered:
                            orders = get_orders_delivered(db,reply);
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg orders_delivered orders %s", carr_read_streams.at(stream.first).c_str(), orders.c_str());
                            assertReply(c2r, reply);
                            if(strcmp(orders.c_str(),"err")){
                                snprintf(descr, 100, "The carrier with id %d accessed the chronology of the orders he delivered",stream.first);
                                log_event(logdb, getpid(), stream.first, "Carrier", "Orders delivered", "Success", descr);
                            }
                            else {
                                snprintf(descr, 100, "An error occurred while the carrier with id %d was accessing his chronology of the orders",stream.first);
                                log_event(logdb, getpid(), stream.first, "Carrier", "Orders delivered", "Error", descr);
                            }
                            break;
                        case picked_up:
                            result = update_order_status(db,logdb,reply);
                            if(result.first){
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * order %d status %s", CARR_WRITE_SUPPL, result.first, result.second.c_str());
                                assertReply(c2r, reply);
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg order_picked_up distance %d", carr_read_streams.at(stream.first).c_str(), get_cust_distance(db,result.first));
                                assertReply(c2r, reply);
                                snprintf(descr, 100, "The carrier with id %d has successfully picked up the order with id %d",stream.first, result.first);
                                log_event(logdb, getpid(), stream.first, "Carrier", "Pick up order", "Success", descr);
                            }
                            else {
                                snprintf(descr, 100, "An error occurred while the carrier with id %d was picking up an order",stream.first);
                                log_event(logdb, getpid(), stream.first, "Carrier", "Pick up order", "Error", descr);
                            }
                            break;
                        case delivered:
                            result = update_order_status(db,logdb,reply);
                            if(result.first){
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * order %d status %s", CARR_WRITE_SUPPL, result.first, result.second.c_str());
                                assertReply(c2r, reply);
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg delivered distance %d", carr_read_streams.at(stream.first).c_str(), get_carr_distance(db,result.first));
                                assertReply(c2r, reply);
                                snprintf(descr, 100, "The carrier with id %d has successfully delivered the order with id %d",stream.first, result.first);
                                log_event(logdb, getpid(), stream.first, "Carrier", "Deliver order", "Success", descr);
                                free_carrier(db,stream.first,get_carr_distance(db,result.first));
                            }
                            else {
                                snprintf(descr, 100, "An error occurred while the carrier with id %d was delivering an order",stream.first);
                                log_event(logdb, getpid(), stream.first, "Carrier", "Deliver order", "Error", descr);
                            }
                            break;
                        default:
                            log_event(logdb, getpid(), stream.first, "Carrier", "Unknown", "Error", "The server did not know how to deal with this input");
                            break;
                    }
                    if(reply != nullptr && last_redis_id.count(stream.first)>0) last_redis_id.at(stream.first) = string(reply->str);
                }
                freeReplyObject(reply);
            }
            micro_sleep(1000);
        }
        redisFree(c2r);
        c2r = nullptr;
        waitpid(pid, &status,0);
        log_event(logdb, getpid(), CARRIER_SERVER, "Server", "Stopping carrier server", "Success", "The carrier server is stopped successfully!");
    }
}