#ifndef TEST_MAIN_H
#define TEST_MAIN_H

#include "../../ecommerce_backend/src/main.h"
#include "../../con2db/pgsql.h"
#include "../../databases/db_functions/src/functions.h"
#include <fstream>
#include <algorithm>
#include <random>
#include <thread>

#define ITERATIONS 3
#define TESTING_CUSTOMERS 15
#define TESTING_SUPPLIERS 8
#define TESTING_CARRIERS 12

struct Credential {
    string email;
    string correct_password;
    string wrong_password;
};

struct Product {
    string name;
    double price;
    int avail; 
    product_category cat;
};

void customer_test();
void carrier_test();
void supplier_test();
void customer_behavior(Customer& c, vector<address>& addr);
void supplier_behavior(Supplier& s, const vector<Product>& prods);
void carrier_behavior(Carrier& c);
vector<tuple<string,address,string,string>> load_new_suppliers();
vector<tuple<string,string,address,string,string,double>> load_new_customers();
vector<tuple<string,string,string>> load_new_carriers();
string generate_random_str(int len);
product_category random_category();
vector<address> load_addresses();
vector<Credential> load_credentials(Con2DB db, int table);
vector<Product> load_new_products();
long micro_time();

#endif