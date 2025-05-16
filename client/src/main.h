#ifndef cl_main_h
#define cl_main_h

using namespace std;

#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include "../../con2redis/src/con2redis.h"
#include "domains.h"
#include "Carrier.h"
#include "Supplier.h"
#include "Customer.h"
#include <unordered_map>
#include <sstream>
#include <random>

int micro_sleep(long usec);
string addr_to_string(address addr);
address str_to_addr(string str);
order_status str_to_status(string str);
product_category str_to_cat(string s);
string cat_to_str(product_category cat);
carrier_status str_to_carr_status(string str);
#endif