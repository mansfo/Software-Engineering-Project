#include "main.h"

vector<Product> load_new_products(){
    ifstream file("../../test/products.txt");
    vector<Product> prods;
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string name, aux;
        int avail;
        double price;
        product_category cat;
        getline(ss, name, ',');
        getline(ss, aux, ',');
        price = stod(aux);
        getline(ss, aux, ',');
        avail = stoi(aux);
        getline(ss, aux, ',');
        cat = str_to_cat(aux.c_str());
        prods.push_back({name, price, avail, cat});
    }
    return prods;
}