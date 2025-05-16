#include "main.h"

void customer_test(){
    srand((unsigned int) time(NULL));
    micro_sleep(5000);
    
    Con2DB db("localhost", "5432", "ecommerce_server", "02468", "ecommerce_main_db");
    
    vector<Customer> customers;
    
    vector<Credential> credentials = load_credentials(db,0);
    vector<address> addresses = load_addresses();

    vector<tuple<string,string,address,string,string,double>> new_customers = load_new_customers();
    vector<thread> threads;
    int it = 0, inserted = 0;
    while(it < ITERATIONS){
        auto rng = default_random_engine {};
        if(micro_time()%100 > 80 && new_customers.size()>0){
            inserted++;
            shuffle(begin(new_customers), end(new_customers), rng);
            Customer c(get<0>(new_customers.at(new_customers.size()-1)),get<1>(new_customers.at(new_customers.size()-1)),get<2>(new_customers.at(new_customers.size()-1)),get<3>(new_customers.at(new_customers.size()-1)),get<4>(new_customers.at(new_customers.size()-1)),get<5>(new_customers.at(new_customers.size()-1)));
            micro_sleep(1000);
            if(c.get_cust_ok()){
                credentials.push_back({get<3>(new_customers.at(new_customers.size()-1)),get<4>(new_customers.at(new_customers.size()-1)),generate_random_str(10)});
                new_customers.pop_back();
            }
        }
        shuffle(begin(credentials), end(credentials), rng);
        int wrong;
        for (int i = 0; i < TESTING_CUSTOMERS + inserted && i < (int)credentials.size(); i++) {
            wrong = micro_time()%100;
            Customer c(credentials.at(i).email, wrong < 75? credentials.at(i).correct_password : credentials.at(i).wrong_password);
            micro_sleep(1000);
            if(wrong<75 && c.get_cust_ok()){
                customers.push_back(c);
            }
        }
        for (int i=0; i < TESTING_CUSTOMERS + inserted && i < (int)customers.size(); ++i) {
            threads.emplace_back(customer_behavior, ref(customers.at(i)), ref(addresses));
        }
        for (auto& t : threads) {
            t.join();
        }
        it++;
        Customer c1(credentials.at(micro_time()%credentials.size()).email,credentials.at(micro_time()%credentials.size()).wrong_password);
        micro_sleep(1000000);
        threads.clear();
        customers.clear();
        //printf("Customer test: iteration number %d just completed!\n",it);
    }
    puts("Ending customer tests...");
}

void customer_behavior(Customer& c, vector<address>& addr){
    int state = 0;
    vector<tuple<int, string, int, double>> sr;
    vector<pair<int,string>> history;
    int elem, quantity, id, op, cart;
    double money;
    while (state < 5){
        if(state == 0 && micro_time() % 100 > 10) {
            cart = c.get_shopping_cart().size();
            state = 1;
        }
        switch (state){
            case 0:
                cart = c.get_shopping_cart().size();
                if(micro_time() % 2 == 0) {
                    c.change_password(c.get_password(), generate_random_str((micro_time()%10)+8));
                }
                else {
                    c.change_address(addr.at(micro_time() % addr.size()));
                }
                state++;
                break;
            case 1:
                money = c.get_current_money();
                if(money < 100.0) state = micro_time()%100 > 10? 5 : 4;
                else{
                    sr = c.search(random_category());
                    if(sr.size()==0) state = micro_time()%100 > 10? 5 : 1; 
                    else state = micro_time()%100 > 60? 2 : 3;
                }
                break;
            case 2:
                elem = micro_time()%sr.size();
                quantity = min(1+(int)(micro_time()%(get<2>(sr.at(elem))+1)),5);
                id = c.make_order(get<0>(sr.at(elem)), quantity);
                if(id < 1) state = 4;
                else{
                    if(micro_time() % 100 >= 95) c.delete_order(id);
                    state = 5;
                }
                break;
            case 3:
                op = micro_time() % 10;
                if(op == 0 && cart > 0) {
                    c.empty_shopping_cart();
                    cart = 0;
                }
                else if(op > 3 && (cart > 3 || (op > 5 && cart > 0))) {
                    id = c.order_all();
                    if(id < 1) state++;
                    else state = 5;
                }
                else {
                    elem = (17*micro_time())%sr.size();
                    cart++;
                    c.add_to_cart(get<0>(sr.at(elem)),get<1>(sr.at(elem)),min(1+(int)(micro_time()%(get<2>(sr.at(elem))+1)),5));
                    state = 1;
                }
                break;
            case 4:
                if(money < 1000) c.add_money(1000.0 + (double)(micro_time()%10000));
                state = micro_time()%100 > 10? 5 : 1;
                break;
            case 5:
                history = c.get_customer_orders();
                if(history.size()>0 && (micro_time()%100 > 30 || c.get_id() % 2 == 0)){
                    int ind = micro_time() % history.size();
                    c.delete_order(history.at(ind).first);
                }
                state++;
                break;
            default:
                break;
        }
    }
}