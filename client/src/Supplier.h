#ifndef Supplier_h
#define Supplier_h

#include "main.h"

class Supplier{
    private:
        int id;
        string name;
        address addr;
        string email;
        string password;
        vector<pair<int,int>> prod_avail;
        vector<pair<int,string>> prod_names;
        string read_stream;
        string write_stream;
        unordered_map<int,order_status> orders_received;
        bool suppl_ok = true;
        bool change_pswd = false;
        redisContext *c2r = redisConnect("localhost", 6379);
    public:
        Supplier(string name, address addr, string email, string password);
        Supplier(int id);
        Supplier(string email, string password);
        int get_id();
        string get_name();
        address get_address();
        string get_password();
        void add_product(string name, double price, int avail, product_category cat);
        void add_product(int id, int avail);
        void set_price(int id, double price);
        void remove_product(int p);
        vector<pair<int,int>> get_products_avail();
        vector<pair<int,int>> get_products_avail(vector<string> rv);
        vector<pair<int,string>> get_products();
        vector<pair<int,string>> get_products(vector<string> rv);
        void change_password(string old, string new_psw);
        int listen_server(string redis_id);
        vector<string> listen_server(string readstream, string redis_id);
        string get_readstream();
        string get_writestream();
        bool get_suppl_ok();
};

#endif