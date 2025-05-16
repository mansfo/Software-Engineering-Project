#include "main.h"

void supplier_test(){
    srand((unsigned int) time(NULL));
    micro_sleep(5000);
    Con2DB db("localhost", "5432", "ecommerce_server", "02468", "ecommerce_main_db");
    vector<thread> threads;
    
    vector<Credential> credentials = load_credentials(db,1);
    vector<tuple<string,address,string,string>> new_suppliers = load_new_suppliers();
    vector<Product> loaded_prods = load_new_products();
    vector<Supplier> suppliers;

    int it = 0, inserted = 0;
    while(it<ITERATIONS){
        auto rng = default_random_engine {};
        if(micro_time()%100 > 80 && new_suppliers.size()>0){
            shuffle(begin(new_suppliers), end(new_suppliers), rng);
            Supplier s(get<0>(new_suppliers.at(new_suppliers.size()-1)),get<1>(new_suppliers.at(new_suppliers.size()-1)),get<2>(new_suppliers.at(new_suppliers.size()-1)),get<3>(new_suppliers.at(new_suppliers.size()-1)));
            micro_sleep(1000);
            if(s.get_suppl_ok()){
                inserted++;
                credentials.push_back({get<2>(new_suppliers.at(new_suppliers.size()-1)),get<3>(new_suppliers.at(new_suppliers.size()-1)),generate_random_str(10)});
                new_suppliers.pop_back();
            }
        }
        shuffle(begin(credentials), end(credentials), rng);
        int wrong;
        for (int i = 0; i < TESTING_SUPPLIERS + inserted && i < (int)credentials.size(); i++) {
            wrong = micro_time()%100;
            Supplier s(credentials.at(i).email, wrong < 95? credentials.at(i).correct_password : credentials.at(i).wrong_password);
            if(wrong < 95 && s.get_suppl_ok()) {
                micro_sleep(1000);
                suppliers.push_back(s);
            }
        }
        for (int i=0; i < TESTING_SUPPLIERS + inserted && i < (int)suppliers.size(); ++i) {
            threads.emplace_back(supplier_behavior, ref(suppliers[i]), cref(loaded_prods));
        }
        for (auto& t : threads) {
            t.join();
        }
        it++;
        micro_sleep(100000);
        threads.clear();
        suppliers.clear();
        //printf("Supplier test: iteration number %d just completed!\n",it);
    }
    puts("Ending supplier tests...");
}

void supplier_behavior(Supplier& s, const vector<Product>& products){
    int state = 0;
    Product p;
    vector<pair<int, int>> prod_avail;
    vector<pair<int, string>> prods;
    vector<int> inserted;
    int ind;
    while(state < 4){
        switch (state){
            case 0:
                if(micro_time()%100 >= 95) s.change_password(s.get_password(),generate_random_str(12));
                state++;
                break;
            case 1:
                if(micro_time()%100>85 && products.size()>0) {
                    do{
                        ind = micro_time() % products.size();
                    } while(count(inserted.begin(), inserted.end(), ind));
                    p = products.at(ind);
                    inserted.push_back(ind);
                    s.add_product(p.name,p.price,p.avail,p.cat);
                }
                state++;
                break;
            case 2:
                prod_avail = s.get_products_avail();
                for(size_t i=0; i<prod_avail.size(); i++){
                    if(prod_avail.at(i).second==0){
                        if(micro_time()%100 > 70) s.remove_product(prod_avail.at(i).first);
                        else s.add_product(prod_avail.at(i).first,(micro_time()%30)+1);
                    }
                }
                state++;
                break;
            case 3:
                prods = s.get_products();
                for(size_t i=0; i<prods.size(); i++){
                    if(micro_time()%100 < 15) s.set_price(prods.at(i).first, 100.0 + (double) (micro_time() % 1000));
                }
                state++;
                break;
            default:
                break;
        }
    }
}