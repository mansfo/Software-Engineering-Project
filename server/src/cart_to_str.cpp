#include "main.h"

string cart_to_str(vector<tuple<int,string,int>> cart){
    string result = "";
    for(long unsigned int i=0; i<cart.size(); i++){
        if(i!=0) result.append(" ");
        result.append(to_string(get<0>(cart.at(i))));
        result.append(" ");
        result.append(get<1>(cart.at(i)));
        result.append(" ");
        result.append(to_string(get<2>(cart.at(i))));
    }
    if(cart.size()==0) return string("err");
    return result;
}