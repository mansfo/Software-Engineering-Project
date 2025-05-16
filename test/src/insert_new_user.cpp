#include "main.h"

vector<tuple<string,address,string,string>> load_new_suppliers(){
    ifstream file("../../test/suppliers.txt");
    vector<tuple<string,address,string,string>> suppls;
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string email, password, name, addr;
        getline(ss, name, ';');
        getline(ss, addr, ';');
        getline(ss, email, ';');
        getline(ss, password, ';');
        suppls.push_back(tuple<string,address,string,string>(name,str_to_addr(addr),email,password));
    }

    return suppls;
}

vector<tuple<string,string,address,string,string,double>> load_new_customers(){
    ifstream file("../../test/customers.txt");
    vector<tuple<string,string,address,string,string,double>> custs;
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string email, password, name, surname, addr, money;
        getline(ss, name, ';');
        getline(ss, surname, ';'); 
        getline(ss, addr, ';');
        getline(ss, email, ';');
        getline(ss, password, ';');
        getline(ss, money, ';'); 
        custs.push_back(tuple<string,string,address,string,string,double>(name,surname,str_to_addr(addr),email,password,stod(money)));
    }

    return custs;
}

vector<tuple<string,string,string>> load_new_carriers(){
    ifstream file("../../test/carriers.txt");
    vector<tuple<string,string,string>> carriers;
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string name, surname, company;
        getline(ss, name, ';');
        getline(ss, surname, ';');
        getline(ss, company, ';');
        carriers.push_back(tuple<string,string,string>(name,surname,company));
    }

    return carriers;
}