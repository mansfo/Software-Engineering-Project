#ifndef domains_h
#define domains_h

#include <string>

typedef struct address{
    std::string roadname;
    int number;
    std::string city;
    std::string nation;
    std::string continent;
} address;

typedef enum order_status{
    not_sent,
    sent,
    arrived,
    deleted,
    deleting_ok,
    not_found
} order_status;

typedef enum product_category{
    guitar,
    bass,
    keyboard,
    piano,
    percussions,
    wind_instruments,
    other
} product_category;

typedef enum carrier_status{
    carr_free,
    assigned,
    delivering,
    coming_back
} carrier_status;

#endif