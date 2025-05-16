#include "main.h"

int set_prod_price(Con2DB db, vector<string> rv){
    if(rv.size() < 10 || strcmp(rv.at(4).c_str(), "id") || strcmp(rv.at(6).c_str(), "price") || strcmp(rv.at(8).c_str(), "suppl")) return 0;
    string prod = rv.at(5);
    string price = rv.at(7);
    string suppl = rv.at(9);
    return set_prod_price(db, prod, price, suppl);
}