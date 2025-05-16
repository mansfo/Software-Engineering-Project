#include "main.h"

string addr_to_string(address addr){
    return to_string(addr.number)+","+addr.roadname+","+addr.city+","+addr.nation+","+addr.continent;
}