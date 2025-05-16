#include "main.h"

product_category random_category(){
    int cat = micro_time() % 7;
    if(cat==0) return product_category::bass;
    if(cat==1) return product_category::guitar;
    if(cat==2) return product_category::keyboard;
    if(cat==3) return product_category::percussions;
    if(cat==4) return product_category::piano;
    if(cat==5) return product_category::wind_instruments;
    return product_category::other;
}