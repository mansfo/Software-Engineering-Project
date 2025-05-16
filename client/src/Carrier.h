#ifndef Carrier_h
#define Carrier_h

#include "main.h"

class Carrier{
    private:
        int id;
        string name;
        string surname;
        string comp;
        carrier_status status;
        vector<pair<int,string>> deliveries_history;
        string read_stream;
        string write_stream;
        int current_order_id = 0;
        string last_rid;
        redisContext *c2r = redisConnect("localhost", 6379);
    public:
        Carrier(string name, string surname, string comp);
        Carrier(int id);
        int get_id();
        string get_name();
        string get_surname();
        string get_company();
        vector<pair<int,string>> get_orders_delivered();
        vector<pair<int,string>> get_orders_delivered(vector<string> rv);
        void pick_up_order();
        void deliver_order();
        int get_current_order_id();
        void set_current_order_id(int ord_id);
        int listen_server(string rid);
        vector<string> listen_server(string readstream, string redis_id);
        int get_order_assigned();
        void update_status();
        carrier_status get_status();
        string get_last_rid();
        string get_readstream();
        string get_writestream();
};

#endif