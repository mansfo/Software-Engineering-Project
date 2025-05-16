#ifndef Customer_h
#define Customer_h
#include "main.h"

class Customer{
    private:
        int id;
        string name;
        string surname;
        address addr;
        string email;
        string password;
        double curr_money;
        string read_stream;
        string write_stream;
        vector<pair<int,string>> orders_chronology;
        vector<tuple<int,string,int,double>> search_result;
        vector<tuple<int,string,int>> shopping_cart;
        bool cust_ok = true;
        bool change_pswd = false;
        redisContext *c2r = redisConnect("localhost",6379);

    public:
        Customer(string cust_name, string surn, address address, string email, string password, double money);
        Customer(int id);
        Customer(string email, string password);
        address get_address();
        void change_address(address addr);
        int get_id();
        string get_name();
        string get_surname();
        string get_email();
        string get_password();
        vector<pair<int,string>> get_customer_orders();
        vector<pair<int,string>> get_customer_orders(vector<string> rv);
        int make_order(int prod_id, int quantity);
        void delete_order(int order_id);
        double get_current_money();
        void add_money(double m);
        vector<tuple<int,string,int,double>> search(product_category p_cat);
        vector<tuple<int,string,int,double>> get_search_result(vector<string> rv);
        vector<tuple<int,string,int>> get_shopping_cart();
        vector<tuple<int,string,int>> get_shopping_cart(vector<string> rv); 
        void add_to_cart(int product, string name, int quantity);
        void empty_shopping_cart();
        int order_all();
        void change_password(string old, string new_psw);
        int listen_server(string redis_id);
        vector<string> listen_server(string readstream,string redis_id);
        string get_readstream();
        string get_writestream();
        bool get_cust_ok();
};

#endif