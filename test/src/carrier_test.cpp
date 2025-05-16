#include "main.h"

void carrier_test(){
    srand((unsigned int) time(NULL));
    micro_sleep(5000);
    vector<thread> threads;

    vector<tuple<string,string,string>> new_carriers = load_new_carriers();
    vector<Carrier> carriers;
    vector<int> carr_ids;
    for(int i=1; i<TESTING_CARRIERS+1; i++) carr_ids.push_back(i);
    int it = 0, inserted = 0;
    while(it < ITERATIONS+1){
        auto rng = default_random_engine {};
        if(micro_time()%100 > 80 && new_carriers.size()>0){
            inserted++;
            shuffle(begin(new_carriers), end(new_carriers), rng);
            Carrier c(get<0>(new_carriers.at(new_carriers.size()-1)),get<1>(new_carriers.at(new_carriers.size()-1)),get<2>(new_carriers.at(new_carriers.size()-1)));
            micro_sleep(1000);
            carr_ids.push_back(c.get_id());
            new_carriers.pop_back();
        }
        shuffle(begin(carr_ids), end(carr_ids), rng);
        for (int i = 0; i < TESTING_CARRIERS + inserted && i < (int) carr_ids.size(); i++){
            carriers.emplace_back(carr_ids.at(i));
        }
        for (int i=0; i < TESTING_CARRIERS + inserted; ++i) {
            threads.emplace_back(carrier_behavior, ref(carriers[i]));
        }
        for (auto& t : threads) {
            t.join();
        }
        it++;
        micro_sleep(1000);
        threads.clear();
        carriers.clear();
        //printf("Carrier test: iteration number %d just completed!\n",it);
    }
    puts("Ending carrier tests...");
}

void carrier_behavior(Carrier& c){
    int state = 0, assigned = 0, timeout = 0;
    vector<pair<int,string>> history;
    while(state < 2){
        switch (state){
            case 0:
                while(assigned == 0){
                    assigned = c.get_order_assigned();
                    timeout++;
                    if (timeout > 30) break;
                }
                state++;
                break;
            case 1:
                if(rand()%100 > 80) history = c.get_orders_delivered();
                state++;
                break;
            default:
                break;
        }
    }
}