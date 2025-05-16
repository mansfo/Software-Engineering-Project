#include "main.h"

string search(Con2DB db, redisReply *r){
    if(r->type == REDIS_REPLY_NIL) return string("error");
    vector<string> rv = getReply(r);
    string cat = rv.at(5);
    PGresult *res = search(db, cat);
    int rows = PQntuples(res);
    string products="";
    for(int i=0;i<rows;i++){
        if(i!=0) products.append(" ");
        products.append(PQgetvalue(res,i,PQfnumber(res,"id")));
        products.append(" ");
        products.append(PQgetvalue(res,i,PQfnumber(res,"name")));
        products.append(" ");
        products.append(PQgetvalue(res,i,PQfnumber(res,"avail")));
        products.append(" ");
        products.append(PQgetvalue(res,i,PQfnumber(res,"price")));
    }
    PQclear(res);
    if(products.length()==0) return string("error");
    return products;
}