#include "main.h"

int Customer::order_all(){
    vector<tuple<int,string,int>> cart = get_shopping_cart();
    redisReply *r;
    int ord_id;
    for(long unsigned int i=0; i<cart.size(); i++){
        ord_id = make_order(get<0>(cart.at(i)), get<2>(cart.at(i)));
        if (ord_id < 1) break;
        r = RedisCommand(c2r, "XADD %s * msg buy_from_cart product %d", write_stream.c_str(), get<0>(cart.at(i)));
        assertReply(c2r,r);
        freeReplyObject(r);
    }
    return ord_id;
}