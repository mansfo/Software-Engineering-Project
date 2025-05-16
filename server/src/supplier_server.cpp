#include "main.h"

#define WRITE_STREAM_0 "suppl0_read_stream"
#define READ_STREAM_0 "suppl0_write_stream"

void supplier_server(){
    redisContext* c2r = redisConnect("localhost",6379);
    Con2DB log("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
    log_event(log, getpid(), SUPPLIER_SERVER, "Server", "Starting supplier server", "Success", "The supplier server is started successfully!");
    
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
    pid_t pid = fork();
    pid_t pid2 = fork();
    if(pid < 0 || pid2 < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid==0 && pid2==0){
        redisContext* c2r = redisConnect("localhost",6379);
        Con2DB db("localhost", "5432", "ecommerce_server", "02468", "ecommerce_main_db");
        Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
        redisReply *reply;
        int time = 0;
        char descr[200];
        string rid = "";
        while(time < MAX_TIME){
            time++;
            if(rid.length()==0) reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS %s $", READ_STREAM_0);
            else reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS %s %s", READ_STREAM_0, rid.c_str());
            if(reply == nullptr || reply->type == REDIS_REPLY_NIL) {
                freeReplyObject(reply);
                continue;
            }
            vector<string> rv = getReply(reply);
            if(rv.size()<4) rid = string(reply->str);
            else rid = rv.at(1);
            if(!strcmp(rv.at(2).c_str(),"get")){
                freeReplyObject(reply);
                continue;
            }
            if(badRequest(reply)) log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "Unknown", "Error", "The Supplier server received a bad formatted request");
            else if(reply->type == 2 && !strcmp(rv.at(3).c_str(),"new_suppl")){
                tuple<string,string,string,string> tup = create_supplier(db, c2r, rv);   
                freeReplyObject(reply);
                reply = RedisCommand(c2r, "XADD %s * redis_id %s id %s readstream %s writestream %s", WRITE_STREAM_0, get<2>(tup).c_str(), get<3>(tup).c_str(), get<0>(tup).c_str(), get<1>(tup).c_str());
                assertReply(c2r, reply);
                if(strcmp(get<3>(tup).c_str(),"err")){
                    snprintf(descr,100,"A new supplier is inserted in the database with id %s", get<3>(tup).c_str());
                    log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "New supplier", "Success", descr);
                }
            }
            else if(reply->type == 2 && !strcmp(rv.at(3).c_str(),"login")){
                pair<string,int> suppl_ids = login_with_psw_supplier(db,logdb,rv);
                if(suppl_ids.second > 0){
                    freeReplyObject(reply);
                    reply = RedisCommand(c2r, "XADD %s * redis_id %s id %d", WRITE_STREAM_0,suppl_ids.first.c_str(),suppl_ids.second);
                    assertReply(c2r, reply);
                    log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "Login supplier", "Success", "The id of the supplier is sent to the client");
                }
                else if(suppl_ids.second==-1){
                    freeReplyObject(reply);
                    reply = RedisCommand(c2r, "XADD %s * redis_id %d id %d", WRITE_STREAM_0,suppl_ids.first.c_str(),suppl_ids.second);
                    assertReply(c2r, reply);
                    log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "Login supplier", "Error", "The email inserted is not present in the database");
                }
                else if(suppl_ids.second==-2){
                    freeReplyObject(reply);
                    reply = RedisCommand(c2r, "XADD %s * redis_id %d id %d", WRITE_STREAM_0,suppl_ids.first.c_str(),suppl_ids.second);
                    assertReply(c2r, reply);
                    log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "Login supplier", "Error", "The password stored in the database does not match with the one inserted");
                }
                else log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "Login supplier", "Error", "An error occurred while retrieving a suppliers id from his email and password");
            }
            else if(reply->type == 2) log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "New supplier", "Error", "An error occurred while inserting a new supplier in the database");
            micro_sleep(1000);
            freeReplyObject(reply);
        }
        redisFree(c2r);
        c2r = nullptr;
    }
    else if(pid==0 && pid2 > 0){
        redisContext* c2r = redisConnect("localhost",6379);
        Con2DB db("localhost", "5432", "ecommerce_server", "02468", "ecommerce_main_db");
        Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
        int time = 0;
        char descr[200];
        string rid = "";
        redisReply* reply;
        unordered_map<int,string> suppl_read_streams;
        unordered_map<int,string> suppl_write_streams;
        unordered_map<int,string> last_redis_id;
        while(time < MAX_TIME){
            time++;
            suppl_read_streams = get_read_streams(db,1);
            suppl_write_streams = get_write_streams(db,1);
            for(auto const& stream: suppl_write_streams){
                if(last_redis_id.count(stream.first)<1) last_redis_id.insert({stream.first,""});
                if(last_redis_id.at(stream.first).length()==0) reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 10 STREAMS %s $", stream.second.c_str());
                else reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 10 STREAMS %s %s", stream.second.c_str(), last_redis_id.at(stream.first).c_str());
                if(reply == NULL || reply->type == REDIS_REPLY_NIL) {
                    freeReplyObject(reply);
                    continue;
                }
                vector<string> rv = getReply(reply);
                if(rv.size() <3 || badRequest(reply)) log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "Unknown", "Error", "The Supplier server received a bad formatted request"); 
                else if(reply->type != REDIS_REPLY_NIL){
                    tuple<string,string> tup;
                    int result;
                    pair<int,int> res;
                    pair<int,string> all_prods;
                    pair<int,string> ord_status;
                    switch(str_to_suppl_event(getMsg(reply))){
                        case suppl_login:
                            tup = login_supplier(db,rv);
                            if(strcmp(get<0>(tup).c_str(), "err")){
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg login name %s addr %s", suppl_read_streams.at(stream.first).c_str(), get<0>(tup).c_str(),get<1>(tup).c_str());
                                assertReply(c2r,reply);
                                snprintf(descr,100,"The supplier with id %d has successfully logged in",stream.first);
                                log_event(logdb, getpid(), stream.first, "Supplier", "Login supplier", "Success", descr);
                            }
                            else{
                                snprintf(descr, 100, "An error occurred while the supplier with id %d was logging in",stream.first);
                                log_event(logdb, getpid(), stream.first, "Supplier", "Login supplier", "Error", descr);
                            }
                            break;
                        case add_prod:
                            if(rv.size() > 4 && !strcmp(rv.at(4).c_str(),"name")){
                                result = add_product(db,reply);
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg new_prod prod_id %d", suppl_read_streams.at(stream.first).c_str(), result);
                                assertReply(c2r,reply);
                                if(result){
                                    snprintf(descr, 100, "The supplier %d has successfully added a new product with id %d",stream.first, result);
                                    log_event(logdb, getpid(), stream.first, "Supplier", "Add product", "Success", descr);
                                }
                                else{
                                    snprintf(descr, 100, "An error occurred while the supplier %d was adding a new product",stream.first);
                                    log_event(logdb, getpid(), stream.first, "Supplier", "Add product", "Error", descr);
                                }
                            }
                            else if(rv.size() > 4 && !strcmp(rv.at(4).c_str(),"id")){
                                result = add_product(db,rv);
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg added_prod prod_id %d", suppl_read_streams.at(stream.first).c_str(), stoi(rv.at(5)));
                                assertReply(c2r,reply);
                                if(result){
                                    snprintf(descr, 100, "The supplier %d has successfully increased the availability of the product with id %d",stream.first, stoi(rv.at(5)));
                                    log_event(logdb, getpid(), stream.first, "Supplier", "Increase avail", "Success", descr);
                                }
                                else{
                                    snprintf(descr, 100, "An error occurred while the supplier %d was increasing the availability of the product %d",stream.first, stoi(rv.at(5)));
                                    log_event(logdb, getpid(), stream.first, "Supplier", "Increase avail", "Error", descr);
                                }
                            }
                            break;
                        case del_prod:
                            res = del_product(db, reply);
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg remove_prod deleted_prod %d status %d", suppl_read_streams.at(stream.first).c_str(), res.first, res.second);
                            assertReply(c2r,reply);
                            if(res.second>0){
                                snprintf(descr, 100, "The supplier %d has successfully removed the product with id %d",stream.first, res.first);
                                log_event(logdb, getpid(), stream.first, "Supplier", "Remove product", "Success", descr);
                            }
                            else{
                                snprintf(descr, 100, "The supplier %d could not remove the product %d",stream.first, res.first);
                                log_event(logdb, getpid(), stream.first, "Supplier", "Remove product", "Error", descr);
                            }
                            break;
                        case set_price:
                            result = set_prod_price(db,rv);
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg set_price prod %s", suppl_read_streams.at(stream.first).c_str(), rv.at(5).c_str());
                            assertReply(c2r,reply);
                            if(result){
                                snprintf(descr, 100, "The supplier %d has successfully updated the price of the product with id %d",stream.first, stoi(rv.at(5)));
                                log_event(logdb, getpid(), stream.first, "Supplier", "Set price", "Success", descr);
                            }
                            else{
                                snprintf(descr, 100, "An error occurred while the supplier %d was updating the price of the product %d",stream.first, stoi(rv.at(5)));
                                log_event(logdb, getpid(), stream.first, "Supplier", "Add product", "Error", descr);
                            }
                            break;
                        case all_products:
                            all_prods = get_products(db, reply);
                            if(all_prods.first==0){
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg all_prods products %s", suppl_read_streams.at(stream.first).c_str(), all_prods.second.c_str());
                                assertReply(c2r, reply);
                                snprintf(descr, 100, "The supplier %d has successfully loaded his products",stream.first);
                                log_event(logdb, getpid(), stream.first, "Supplier", "Load products", "Success", descr);
                            }
                            else{
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg all_prods_a products %s", suppl_read_streams.at(stream.first).c_str(), all_prods.second.c_str());
                                assertReply(c2r, reply);
                                snprintf(descr, 100, "The supplier %d has successfully checked the availability his products",stream.first);
                                log_event(logdb, getpid(), stream.first, "Supplier", "Check availability", "Success", descr);
                            }
                            if(!strcmp(all_prods.second.c_str(), "err")){
                                snprintf(descr, 100, "An error occurred while the supplier %d was loading his products",stream.first);
                                log_event(logdb, getpid(), stream.first, "Supplier", "Load products", "Error", descr);
                            }
                            break;
                        case suppl_change_psw:
                            result = change_password(db,reply,stream.first,"Supplier");
                            if(result){
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg new_password status ok", 
                                    suppl_read_streams.at(stream.first).c_str());
                                assertReply(c2r,reply); 
                                snprintf(descr,100,"The supplier %d has successfully updated his password", stream.first);
                                log_event(logdb, getpid(), stream.first, "Supplier", "Change password", "Success", descr);
                            }
                            else{
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg new_password status error", 
                                    suppl_read_streams.at(stream.first).c_str());
                                assertReply(c2r,reply);
                                snprintf(descr,100,"An error occurred while the customer %d was updating his password", stream.first);
                                log_event(logdb, getpid(), stream.first, "Supplier", "Change password", "Error", descr);
                            }
                            break;
                        default: 
                            log_event(logdb, getpid(), stream.first, "Supplier", "Unknown", "Error", "The server did not know how to deal with this input");
                            break;
                    }
                    last_redis_id.at(stream.first) = string(reply->str);
                }
                freeReplyObject(reply);
            }
            micro_sleep(1000);
        }
        redisFree(c2r);
        c2r = nullptr;
    }
    else if(pid>0 && pid2==0){
        redisContext* c2r = redisConnect("localhost",6379);
        redisReply *reply;
        Con2DB db("localhost", "5432", "ecommerce_server", "02468", "ecommerce_main_db");
        Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
        int time = 0;
        char descr[200];
        unordered_map<int,string> suppl_read_streams;
        unordered_map<int,string> suppl_write_streams;
        queue<int> pending_orders;
        queue<int> deleted_orders;
        reply = RedisCommand(c2r, "XADD suppl0_write_stream * get redis_id");
        assertReply(c2r,reply);
        string rid = string(reply->str);
        freeReplyObject(reply);
        while(time < MAX_TIME){
            time++;
            suppl_read_streams = get_read_streams(db,1);
            suppl_write_streams = get_write_streams(db,1);
            if(pending_orders.size()) {
                reply = RedisCommand(c2r, "XADD %s * msg new_order order_id %d", SUPPL_WRITE_CARR, pending_orders.front());
                assertReply(c2r, reply);
                freeReplyObject(reply);
                snprintf(descr,100,"The Supplier server has sent the order %d to the Carrier server", pending_orders.front());
                log_event(logdb,getpid(),SUPPLIER_SERVER,"Server","New order","Success",descr);
                pending_orders.pop();
            }
            else pending_orders = get_pending_orders(db);
            if(deleted_orders.size()){
                reply = RedisCommand(c2r, "XADD %s * msg deleted_order order_id %d", SUPPL_WRITE_CARR, deleted_orders.front());
                assertReply(c2r, reply);
                freeReplyObject(reply);
                snprintf(descr,100,"The Supplier server has notified the Carrier server to delete the order %d", deleted_orders.front());
                log_event(logdb,getpid(),SUPPLIER_SERVER,"Server","Delete order","Success",descr);
                deleting_handled(db, deleted_orders.front());
                deleted_orders.pop();
            }
            else deleted_orders = get_deleted_orders(db);
            reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS %s %s", SUPPL_READ_CARR, rid.c_str());
            if(reply == nullptr || reply->type == REDIS_REPLY_NIL) {
                freeReplyObject(reply);
                continue;
            }
            vector<string> rv = getReply(reply);
            if(rv.size()>1) rid = rv.at(1);
            if(rv.size()>5 && !strcmp(rv.at(2).c_str(),"order") && !strcmp(rv.at(4).c_str(),"status")){
                string status = rv.at(5);
                string ord_id = rv.at(3);
                if(!strcmp(status.c_str(),"delivered") || !strcmp(status.c_str(),"sent")){
                    freeReplyObject(reply);
                    reply = RedisCommand(c2r, "XADD %s * msg order_infos order %s status %s", suppl_read_streams.at(supplier_from_order(db, stoi(ord_id))).c_str(), ord_id.c_str(), status.c_str());
                    assertReply(c2r, reply);
                    freeReplyObject(reply);
                    reply = RedisCommand(c2r, "XADD %s * order %s status %s", SUPPL_WRITE_CUST, ord_id.c_str(), status.c_str());
                    assertReply(c2r, reply);
                    snprintf(descr,200,"The Supplier server received an update from the Carrier server on the order %s, that has just been %s", ord_id.c_str(), status.c_str());
                    log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "Update order status", "Success", descr);
                }
                else log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "Update order status", "Error", "The server did not know how to deal with this input");
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
        int time = 0;
        char descr[200];
        string rid = "";
        redisReply* reply;
        unordered_map<int,string> suppl_read_streams;
        unordered_map<int,string> suppl_write_streams;
        while(time < MAX_TIME){
            time++;
            if(rid.length()<1) reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS %s $", SUPPL_READ_CUST);
            else reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS %s %s", SUPPL_READ_CUST, rid.c_str());
            if(reply == NULL || reply->type == REDIS_REPLY_NIL){
                freeReplyObject(reply);
                continue;
            }
            suppl_read_streams = get_read_streams(db,1);
            suppl_write_streams = get_write_streams(db,1);
            vector<string> rv = getReply(reply);
            if(rv.size()>1) rid = rv.at(1);
            int result;
            pair<int,int> del_result;
            if(badRequest(reply)) log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "Unknown", "Error", "The Supplier server received a bad formatted request");
            else if(rv.size()>3){
                if(rv.size()>9 && !strcmp(rv.at(3).c_str(), "del_order")){
                    del_result = delete_ord(db, logdb, rv.at(7), stoi(rv.at(9)));
                    if(del_result.first){
                        freeReplyObject(reply);
                        reply = RedisCommand(c2r, "XADD %s * redis_id %s status ok", SUPPL_WRITE_CUST, rv.at(5).c_str());
                        assertReply(c2r, reply);
                        freeReplyObject(reply);
                        reply = RedisCommand(c2r, "XADD %s * msg del_order id %d", suppl_read_streams.at(del_result.second).c_str(), del_result.first);
                        assertReply(c2r, reply);
                        snprintf(descr,100,"The Supplier server has received a request to delete the order %d from the Customer server", del_result.first);
                        log_event(logdb,getpid(),SUPPLIER_SERVER,"Server","Delete order","Success",descr);
                    }
                    else {
                        freeReplyObject(reply);
                        reply = RedisCommand(c2r, "XADD %s * redis_id %s status error", SUPPL_WRITE_CUST, rv.at(5).c_str());
                        assertReply(c2r, reply);
                        log_event(logdb,getpid(),SUPPLIER_SERVER,"Server","Delete order","Error","The Supplier server received a request to delete an order but was not able to process it");
                    }
                }
                else if(rv.size()>9 && !strcmp(rv.at(3).c_str(), "new_order")){
                    string cust = rv.at(9);
                    string prod = rv.at(7);
                    result = make_an_order(db, logdb, prod, cust, rv.at(11));
                    if(result){
                        freeReplyObject(reply);
                        reply = RedisCommand(c2r, "XADD %s * redis_id %s order_id %d status ok", SUPPL_WRITE_CUST, rv.at(5).c_str(), result);
                        assertReply(c2r, reply);
                        freeReplyObject(reply);
                        reply = RedisCommand(c2r, "XADD %s * msg new_order id %d", suppl_read_streams.at(get_supplier(db, stoi(prod))).c_str(), result);
                        assertReply(c2r, reply);
                        snprintf(descr,200,"The Supplier server has received a request to create an order for the product %s from the Customer server", prod.c_str());
                        log_event(logdb,getpid(),SUPPLIER_SERVER,"Server","New order","Success",descr);
                    }
                    else {
                        freeReplyObject(reply);
                        reply = RedisCommand(c2r, "XADD %s * redis_id %s order_id 0 status error", SUPPL_WRITE_CUST, rv.at(5).c_str());
                        assertReply(c2r, reply);
                        log_event(logdb,getpid(),SUPPLIER_SERVER,"Server","New order","Error","The Supplier server received a request to create an order but was not able to process it");
                    }
                }
                else log_event(logdb, getpid(), SUPPLIER_SERVER, "Server", "Customer order operation", "Error", "The server did not know how to deal with this input");
            }
            freeReplyObject(reply);
        }
        micro_sleep(1000);
        redisFree(c2r);
        c2r = nullptr;
    }
    log_event(log, getpid(), SUPPLIER_SERVER, "Server", "Stopping supplier server", "Success", "The supplier server is stopped successfully!");
}