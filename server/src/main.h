#ifndef s_main_h
#define s_main_h

#include <string>
#include <cstring>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <vector>
#include <queue>
#include <algorithm>
#include "../../client/src/domains.h"
#include "../../ecommerce_backend/src/main.h"
#include "../../con2db/pgsql.h"
#include "../../databases/db_functions/src/functions.h"

#define CUSTOMER_SERVER 1
#define SUPPLIER_SERVER 2
#define CARRIER_SERVER 3

#define CUST_READ_SUPPL "cust_read_suppl"
#define CUST_WRITE_SUPPL "cust_write_suppl"
#define SUPPL_READ_CUST "cust_write_suppl"
#define SUPPL_WRITE_CUST "cust_read_suppl"
#define SUPPL_READ_CARR "carr_write_suppl"
#define SUPPL_WRITE_CARR "carr_read_suppl"
#define CARR_READ_SUPPL "carr_read_suppl"
#define CARR_WRITE_SUPPL "carr_write_suppl"
#define AUX_STREAM "aux_stream"

typedef enum cust_server_event{
    cust_login,
    searching,
    delete_order,
    buy_something,
    buy_from_cart,
    history,
    cust_change_address,
    cust_change_psw,
    to_cart,
    empty_cart,
    get_cart,
    add_money,
    get_curr_money,
    cust_unknown
} cust_server_event;

typedef enum suppl_server_event{
    suppl_login,
    all_products,
    add_prod,
    del_prod,
    set_price,
    suppl_change_psw,
    suppl_unknown
} suppl_server_event;

typedef enum carr_server_event{
    carr_login,
    orders_delivered,
    picked_up,
    delivered,
    order_assigned,
    carr_unknown
} carr_server_event;

using namespace std;

int micro_sleep(long usec);
void customer_server();
void carrier_server();
void supplier_server();
cust_server_event str_to_cust_event(string str);
tuple<string,string,string,string,bool,bool> create_customer(Con2DB db, redisContext* c2r, vector<string> rv);
tuple<string,string,string> login_customer(Con2DB db, redisReply *r);
tuple<string,int,string,string> login_with_psw_customer(Con2DB db, Con2DB logdb, redisContext* c2r, vector<string> rv);
string search(Con2DB db, redisReply *r);
int delete_ord(Con2DB db, redisContext* c2r, redisReply *r);
int make_an_order(Con2DB db, redisContext* c2r, redisReply *r);
string get_history(Con2DB db, redisReply *r);
tuple<string,string,string,string> create_supplier(Con2DB db, redisContext* c2r, vector<string> rv);
suppl_server_event str_to_suppl_event(string str);
tuple<string,string> login_supplier(Con2DB db, vector<string> rv);
pair<string,int> login_with_psw_supplier(Con2DB db, Con2DB logdb, vector<string> rv);
int add_product(Con2DB db, redisReply *r);
int add_product(Con2DB db, vector<string> rv);
int set_prod_price(Con2DB db, vector<string> rv);
pair<int,int> del_product(Con2DB db, redisReply *r);
pair<int,string> get_products(Con2DB db, redisReply *r);
tuple<string,string,string,string> create_carrier(Con2DB db, redisContext* c2r, vector<string> rv);
tuple<string,string,string,string> login_carrier(Con2DB db, vector<string> rv);
carr_server_event str_to_carr_event(string str);
string get_orders_delivered(Con2DB db, redisReply *r);
pair<int,string> update_order_status(Con2DB db, Con2DB logdb, redisReply *r);
int change_password(Con2DB db, redisReply *r, int userid, string usertype);
int change_address(Con2DB db, redisReply *r, int userid, string usertype);
int add_money_to_cust(Con2DB db, redisReply *r, int userid);
string cart_to_str(vector<tuple<int,string,int>> cart);

#endif