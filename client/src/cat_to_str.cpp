#include "main.h"

string cat_to_str(product_category cat){
    switch (cat){
    case guitar:
        return string("guitar");
    case bass:
        return string("bass");
    case piano:
        return string("piano");
    case keyboard:
        return string("keyboard");
    case percussions:
        return string("percussions");
    case wind_instruments:
        return string("wind");
    default:
        return string("other");
    }
}