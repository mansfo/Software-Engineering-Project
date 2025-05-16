#ifndef db_func_h
#define db_func_h

#include "../../../client/src/domains.h"
#include "../../../ecommerce_backend/src/main.h"
#include <ctime>
#include <cstring>
#include <queue>

using namespace std;

PGresult* create_customer(Con2DB db, string name, string surname, string addr, string email, string password, string money);
PGresult* login_customer(Con2DB db, string id);
int login_with_psw_customer(Con2DB db, Con2DB logdb, string email, string password);
unordered_map<int,string> get_read_streams(Con2DB db, int table);
unordered_map<int,string> get_write_streams(Con2DB db, int table);
PGresult* get_infos(Con2DB db, string ord);
PGresult* search(Con2DB db, string cat);
pair<int,int> delete_ord(Con2DB db, Con2DB logdb, string ord, int id);
void deleting_handled(Con2DB db, int ord);
order_status orderstatus_from_id(Con2DB db, int ord);
int same_customer(Con2DB db, int ord, int cid);
int make_an_order(Con2DB db, Con2DB logdb, string prod, string cust, string quant);
string get_history(Con2DB db, string cust);
PGresult* create_supplier(Con2DB db, string name, string address, string email, string password);
PGresult* login_supplier(Con2DB db, string id);
int login_with_psw_supplier(Con2DB db, Con2DB logdb, string email, string password);
int add_product(Con2DB db, string name, string price, string avail, string cat, string id);
int add_product(Con2DB db, string prod_id, string avail, string suppl);
int del_product(Con2DB db, string suppl, string prod);
int set_prod_price(Con2DB db, string prod, string price, string suppl);
string get_products(Con2DB db, string suppl, int avail);
int get_supplier(Con2DB db, int product);
PGresult *create_carrier(Con2DB db, string name, string surn, string comp);
PGresult *login_carrier(Con2DB db, string id);
string get_orders_delivered(Con2DB db, string carr_id);
pair<int,string> pick_up_order(Con2DB db, Con2DB logdb, string id);
pair<int,string> deliver_order(Con2DB db, Con2DB logdb, string id);
queue<int> get_pending_orders(Con2DB db);
unordered_map<int,int> get_assigned_pending_orders(Con2DB db);
queue<int> get_deleted_orders(Con2DB db);
pair<int,int> get_assigned_order(Con2DB db, int carrier);
int assign_carrier(Con2DB db, int ord_id, int maxid);
int supplier_from_order(Con2DB db, int ord);
int cust_from_order(Con2DB db, int ord);
int free_email(Con2DB db, string email, string usertype);
int change_password(Con2DB db, string old, string new_psw, int userid, string table);
void log_event(Con2DB db, int pid, int userid, string usertype, string operation, string outcome, string description);
void log_purchase(Con2DB db, double price, double money, int avail, int quantity, int order);
void log_access(Con2DB db, string email, int successfull);
void log_order_update(Con2DB logdb, int order, int status);
order_status int_to_status(string str);
int change_address(Con2DB db, string addr, int userid, string table);
int add_money_to_cust(Con2DB db, string money, int userid);
double get_money(Con2DB db, int id);
int get_suppl_distance(Con2DB db, int order);
int get_cust_distance(Con2DB db, int order);
int get_carr_distance(Con2DB db, int order);
void add_streams_to_db(Con2DB db, int id, string readstr, string writestr, int table);
void free_carrier(Con2DB db, int id, int distance);
long micro_time();

#endif