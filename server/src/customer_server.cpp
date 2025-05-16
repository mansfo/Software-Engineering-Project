#include "main.h"

#define READ_STREAM_0 "cust0_write_stream"
#define WRITE_STREAM_0 "cust0_read_stream"

void customer_server(){
    redisContext* c2r = redisConnect("localhost",6379);
    Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
    log_event(logdb, getpid(), CUSTOMER_SERVER, "Server", "Starting customer server", "Success", "The customer server is started successfully!");

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
        int time = 0;
        redisReply *reply;
        redisReply *reply_suppl;
        redisContext* c2r = redisConnect("localhost",6379);
        Con2DB db("localhost", "5432", "ecommerce_server", "02468", "ecommerce_main_db");
        Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
        string redid = "";
        string rid_suppl = "";
        while(time < MAX_TIME){
            time++;
            if(redid.length()==0) reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 10000 STREAMS %s $", READ_STREAM_0);
            else reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 10000 STREAMS %s %s", READ_STREAM_0, redid.c_str()); 
            if(reply == nullptr || reply->type == REDIS_REPLY_NIL || reply->type > 6) {
                freeReplyObject(reply);
                continue;
            }
            vector<string> rv = getReply(reply);
            if(rv.size()<2) redid = string(reply->str);
            else redid = rv.at(1);
            if(badRequest(reply)) log_event(logdb, getpid(), CUSTOMER_SERVER, "Server", "Unknown", "Error", "The Customer server received a bad formatted request");
            else if(reply->type == 2 && rv.size()>3 && !strcmp(rv.at(3).c_str(),"new_cust")){
                tuple<string,string,string,string,bool,bool> tup = create_customer(db, c2r, rv);
                if(strcmp(get<2>(tup).c_str(),"err")){
                    char descr[100];
                    string outcome = "Error";
                    if(get<5>(tup) && get<4>(tup)) {
                        snprintf(descr,100,"A new customer is successfully inserted in the database with id %s", get<3>(tup).c_str());
                        outcome = "Success";
                    }
                    else if(get<4>(tup)) snprintf(descr,100,"The password inserted by the customer is too short");
                    else snprintf(descr,100,"The email is already associated to a customer");
                    freeReplyObject(reply);
                    micro_sleep(100);
                    reply = RedisCommand(c2r, "XADD %s * redis_id %s id %s readstream %s writestream %s", WRITE_STREAM_0, get<2>(tup).c_str(), get<3>(tup).c_str(), get<0>(tup).c_str(), get<1>(tup).c_str());
                    assertReply(c2r, reply);
                    log_event(logdb, getpid(), CUSTOMER_SERVER, "Server", "New customer", outcome, descr);
                }
                else log_event(logdb, getpid(), CUSTOMER_SERVER, "Server", "New customer", "Error", "An error occurred while inserting a new customer in the database");
            }
            else if(reply->type == 2 && rv.size()>3 && !strcmp(rv.at(3).c_str(),"login")){
                tuple<string,int,string,string> tup = login_with_psw_customer(db,logdb,c2r,rv);
                pair<string,int> cust_ids = pair<string,int>(get<0>(tup),get<1>(tup));
                if(cust_ids.second > 0){
                    freeReplyObject(reply);
                    reply = RedisCommand(c2r, "XADD %s * redis_id %s id %d", WRITE_STREAM_0,cust_ids.first.c_str(),cust_ids.second);
                    assertReply(c2r, reply);
                    log_event(logdb, getpid(), CUSTOMER_SERVER, "Server", "Login customer", "Success", "The id of the customer is sent to the client");
                }
                else if(cust_ids.second==-1){
                    freeReplyObject(reply);
                    reply = RedisCommand(c2r, "XADD %s * redis_id %s id %d", WRITE_STREAM_0,cust_ids.first.c_str(),cust_ids.second);
                    assertReply(c2r, reply);
                    log_event(logdb, getpid(), CUSTOMER_SERVER, "Server", "Login customer", "Error", "The email inserted is not present in the database");
                }
                else if(cust_ids.second==-2){
                    freeReplyObject(reply);
                    reply = RedisCommand(c2r, "XADD %s * redis_id %s id %d", WRITE_STREAM_0,cust_ids.first.c_str(),cust_ids.second);
                    assertReply(c2r, reply);
                    log_event(logdb, getpid(), CUSTOMER_SERVER, "Server", "Login customer", "Error", "The password stored in the database does not match with the one inserted");
                }
                else log_event(logdb, getpid(), CUSTOMER_SERVER, "Server", "Login customer", "Error", "An error occurred while retrieving a customer's id from his email and password");
            }
            
            if(rid_suppl.length()==0) reply_suppl = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS %s $", CUST_READ_SUPPL);
            else reply_suppl = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS %s %s", CUST_READ_SUPPL, rid_suppl.c_str());
            if(reply_suppl == nullptr || reply_suppl->type == REDIS_REPLY_NIL || reply_suppl->type > 6) {
                freeReplyObject(reply_suppl);
                continue;
            }
            vector<string> rvs = getReply(reply_suppl);
            if(rvs.size()>5 && !strcmp(rvs.at(2).c_str(),"order") && !strcmp(rv.at(4).c_str(),"status")){
                rid_suppl = rvs.at(1);
                string ord = rvs.at(3);
                string status = rvs.at(5);
                freeReplyObject(reply_suppl);
                reply_suppl = RedisCommand(c2r, "XADD %s * order %s status %s", cust_from_order(db,stoi(ord)), ord.c_str(), status.c_str());
                assertReply(c2r, reply_suppl);
                char descr[100];
                snprintf(descr,100,"The status of the order %s is now %s", ord.c_str(), status.c_str());
                log_event(logdb, getpid(), cust_from_order(db,stoi(ord)),"Customer","Delivering status","Success",descr);
            }
            freeReplyObject(reply_suppl);
            micro_sleep(1000);
        }
        redisFree(c2r);
        c2r = nullptr;
    }
    else{
        int time = 0;
        redisReply *reply;
        redisContext* c2r = redisConnect("localhost",6379);
        Con2DB db("localhost", "5432", "ecommerce_server", "02468", "ecommerce_main_db");
        Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
        unordered_map<int,vector<tuple<int,string,int>>> shopping_carts;
        unordered_map<int,string> last_redis_id;
        unordered_map<int,string> cust_read_streams;
        unordered_map<int,string> cust_write_streams;
        while(time < MAX_TIME){
            time++;
            cust_read_streams = get_read_streams(db,0);
            cust_write_streams = get_write_streams(db,0);
            for(auto const& stream: cust_write_streams){
                if(last_redis_id.count(stream.first)<1) last_redis_id.insert({stream.first,""});
                if(last_redis_id.count(stream.first)>0 && last_redis_id.at(stream.first).length()>0) reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 10 STREAMS %s %s", stream.second.c_str(), last_redis_id.at(stream.first).c_str());
                else reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 10 STREAMS %s $", stream.second.c_str());
                assertReply(c2r,reply);
                if(reply == NULL || reply->type == REDIS_REPLY_NIL) continue;
                if(badRequest(reply)) log_event(logdb, getpid(), CUSTOMER_SERVER, "Server", "Unknown", "Error", "The Customer server received a bad formatted request");
                if(reply->type != REDIS_REPLY_NIL){
                    vector<string> rv = getReply(reply);
                    if(rv.size()>1) last_redis_id.at(stream.first) = rv.at(1);
                    tuple<string,string,string> tup;
                    int res;
                    string prods;
                    string ord_id;
                    pair<int,string> ord_status;
                    string all_orders;
                    double budget;
                    char descr[100];
                    switch(str_to_cust_event(getMsg(reply))){
                        case cust_login:
                            tup = login_customer(db,reply);
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg login name %s surname %s addr %s", 
                                cust_read_streams.at(stream.first).c_str(), get<0>(tup).c_str(),get<1>(tup).c_str(),get<2>(tup).c_str());
                            assertReply(c2r,reply);
                            if(strcmp(get<0>(tup).c_str(), "err")){
                                if(shopping_carts.count(stream.first)<1) shopping_carts.insert({stream.first,vector<tuple<int,string,int>>()});
                                snprintf(descr, 100, "The customer with id %d has logged in",stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Login customer", "Success", descr);
                            } 
                            else{
                                snprintf(descr, 100, "An error occurred while the customer with id %d was logging in",stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Login customer", "Error", descr);
                            }
                            break;
                        case searching:
                            prods = search(db,reply);
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg search products %s", cust_read_streams.at(stream.first).c_str(), prods.c_str());
                            assertReply(c2r,reply);
                            if(rv.size()>5 && strcmp(prods.c_str(),"error")){
                                string category = rv.at(5);
                                snprintf(descr, 100, "The customer %d searched products belonging to the %s category", stream.first, category.c_str());
                                log_event(logdb, getpid(), stream.first, "Customer", "Search", "Success", descr);
                            }
                            else{
                                snprintf(descr,100, "An error occurred while the customer %d was searching products in the %s category", stream.first, rv.at(5).c_str());
                                log_event(logdb, getpid(), stream.first, "Customer", "Search", "Error", descr);
                            }
                            break;
                        case delete_order:
                            res = delete_ord(db, c2r, reply);
                            ord_id = rv.size()>5? rv.at(5) : "err";
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg deleted ord_id %s", 
                                cust_read_streams.at(stream.first).c_str(),ord_id.c_str());
                            assertReply(c2r,reply);
                            if(res){
                                snprintf(descr,100,"The customer %d has successfully deleted the order %s", stream.first, ord_id.c_str());
                                log_event(logdb, getpid(), stream.first, "Customer", "Delete order", "Success", descr);
                            }
                            else{
                                snprintf(descr,100,"An error occurred while the customer %d was trying to delete an order", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Delete order", "Error", descr);
                            }
                            break;
                        case buy_something:
                            res = make_an_order(db, c2r, reply);
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg new_order order_id %d", cust_read_streams.at(stream.first).c_str(), res>0? res : 0);
                            assertReply(c2r,reply);
                            if(res>0){
                                snprintf(descr,100,"The customer %d has successfully created the order %d", stream.first, res);
                                log_event(logdb, getpid(), stream.first, "Customer", "Make an order", "Success", descr);
                            }
                            else{
                                snprintf(descr,100,"An error occurred while the customer %d was creating an order", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Make an order", "Error", descr);
                            }
                            break;
                        case buy_from_cart:
                            if(rv.size()>5 && !strcmp(rv.at(4).c_str(),"product")){
                                int prod = stoi(rv.at(5));
                                shopping_carts.at(stream.first).erase(
                                    remove_if(shopping_carts.at(stream.first).begin(), shopping_carts.at(stream.first).end(),
                                        [prod](auto p){return get<0>(p)==prod;}), shopping_carts.at(stream.first).end());
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg order_cart_ok", cust_read_streams.at(stream.first).c_str());
                                assertReply(c2r, reply);
                                snprintf(descr,100,"The customer %d has ordered the product %d from the shopping cart, the product is removed", stream.first, prod);
                                log_event(logdb, getpid(), stream.first, "Customer", "Buy from cart", "Success", descr);
                            }
                            else{
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg order_cart_error", cust_read_streams.at(stream.first).c_str());
                                assertReply(c2r, reply);
                                snprintf(descr,100,"An error occurred while the customer %d was buying a product from the cart", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Buy from cart", "Error", descr);
                            }
                            break;
                        case history:
                            all_orders = get_history(db, reply);
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg orders_history orders %s", cust_read_streams.at(stream.first).c_str(), all_orders.c_str());
                            assertReply(c2r, reply);
                            if(strcmp(all_orders.c_str(),"err")){
                                snprintf(descr,100,"The customer %d has successfully loaded the chronology of his orders", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Orders history", "Success", descr);
                            }
                            else{
                                snprintf(descr,100,"An error occurred while the customer %d was loading the chronology of his orders", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Orders history", "Error", descr);
                            }
                            break;
                        case cust_change_address:
                            res = change_address(db,reply,stream.first,"Customer");
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg new_address status ok", 
                                cust_read_streams.at(stream.first).c_str());
                            assertReply(c2r,reply);
                            if(res){
                                snprintf(descr,100,"The customer %d has successfully updated his address", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Change address", "Success", descr);
                            }
                            else{
                                snprintf(descr,100,"An error occurred while the customer %d was updating his address", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Change address", "Error", descr);
                            }
                            break;
                        case cust_change_psw:
                            res = change_password(db,reply,stream.first,"Customer");
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg new_password status %s", cust_read_streams.at(stream.first).c_str(), res? "ok":"error");
                            assertReply(c2r,reply); 
                            if(res){
                                snprintf(descr,100,"The customer %d has successfully updated his password", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Change password", "Success", descr);
                            }
                            else{
                                snprintf(descr,100,"An error occurred while the customer %d was updating his password", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Change password", "Error", descr);
                            }
                            break;
                        case empty_cart:
                            shopping_carts.at(stream.first).clear();
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg empty_cart status ok", 
                                cust_read_streams.at(stream.first).c_str());
                            assertReply(c2r,reply); 
                            snprintf(descr,100,"The customer %d has successfully cleared his shopping cart", stream.first);
                            log_event(logdb, getpid(), stream.first, "Customer", "Empty cart", "Success", descr);
                            break;
                        case to_cart:
                            if(rv.size() > 9 && !strcmp(rv.at(4).c_str(),"product") &&
                                !strcmp(rv.at(6).c_str(),"name") &&
                                !strcmp(rv.at(8).c_str(),"quantity")){

                                int prod = stoi(rv.at(5));
                                string prod_name = rv.at(7);
                                int quantity = stoi(rv.at(9));
                                shopping_carts.at(stream.first).push_back(tuple<int,string,int>(prod,prod_name,quantity));
                                freeReplyObject(reply);
                                reply = RedisCommand(c2r, "XADD %s * msg to_cart status ok", cust_read_streams.at(stream.first).c_str());
                                assertReply(c2r,reply); 
                                snprintf(descr,100,"The customer %d has successfully added the product %d to the cart", stream.first, prod);
                                log_event(logdb, getpid(), stream.first, "Customer", "To cart", "Success", descr);
                            }
                            else{
                                snprintf(descr,100,"An error occurred while the customer %d was adding a product to the cart", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "To cart", "Error", descr);
                            }
                            break;
                        case get_cart:
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg get_cart cart %s", cust_read_streams.at(stream.first).c_str(),cart_to_str(shopping_carts.at(stream.first)).c_str());
                            assertReply(c2r,reply);
                            snprintf(descr,100,"The customer %d has successfully loaded his shopping cart", stream.first);
                            log_event(logdb, getpid(), stream.first, "Customer", "Get cart", "Success", descr);
                            break;
                        case get_curr_money:
                            budget = get_money(db, stream.first);
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg get_money money %s", cust_read_streams.at(stream.first).c_str(), to_string(budget).c_str());
                            assertReply(c2r,reply); 
                            snprintf(descr,100,"The customer %d has successfully checked his finances", stream.first);
                            log_event(logdb, getpid(), stream.first, "Customer", "Get money", "Success", descr);
                            break;
                        case add_money:
                            res = add_money_to_cust(db,reply,stream.first);
                            freeReplyObject(reply);
                            reply = RedisCommand(c2r, "XADD %s * msg added_money status ok", 
                                cust_read_streams.at(stream.first).c_str());
                            assertReply(c2r,reply);
                            if(res){ 
                                snprintf(descr,100,"The customer %d has successfully updated his finances", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Add money", "Success", descr);
                            }
                            else{
                                snprintf(descr,100,"An error occurred while the customer %d was updating his finances", stream.first);
                                log_event(logdb, getpid(), stream.first, "Customer", "Add money", "Error", descr);
                            }
                            break;
                        default: 
                            log_event(logdb, getpid(), stream.first, "Customer", "Unknown", "Error", "The server did not know how to deal with this input");
                            break;
                    }
                }
                last_redis_id.at(stream.first) = string(reply->str);
                freeReplyObject(reply);
            }
            micro_sleep(1000);
        }
        redisFree(c2r);
        c2r = nullptr;
        waitpid(pid, &status,0);
        log_event(logdb, getpid(), CUSTOMER_SERVER, "Server", "Stopping customer server", "Success", "The customer server is stopped successfully!");
    }
}