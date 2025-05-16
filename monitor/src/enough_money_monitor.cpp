#include "main.h"

void enough_money_monitor(){
    Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
    micro_sleep(1000);
    log_event(logdb, getpid(), EN_MONEY_MONITOR, "Monitor", "Starting monitor", "Success", "The monitor started successfully");
    int time = 0;
    int last_id = 0;
    while(time < MAX_TIME/100){
        char sqlcmd[100];
        snprintf(sqlcmd,100,"SELECT id,price,money,quantity,ord FROM PurchasesLogTable WHERE id > %d ORDER BY id ASC", last_id);
        PGresult *res = logdb.ExecSQLtuples(sqlcmd);
        int rows = PQntuples(res);
        for(int i=0; i<rows; i++){
            if(i==rows-1) last_id = atoi(PQgetvalue(res,i,PQfnumber(res,"id")));
            double price = stod(PQgetvalue(res,i,PQfnumber(res,"price")));
            double money = stod(PQgetvalue(res,i,PQfnumber(res,"money")));
            int quantity = atoi(PQgetvalue(res,i,PQfnumber(res,"quantity")));
            int order = atoi(PQgetvalue(res,i,PQfnumber(res,"ord")));
            if(money < quantity*price) {
                char descr[100];
                snprintf(descr,100,"An error occurred on the order %d, as the customer did not have enough money", order);
                log_event(logdb, getpid(), EN_MONEY_MONITOR, "Monitor", "WARNING: not enough money", "Success", descr);
            }
            else{
                char descr[100];
                snprintf(descr,100,"The order %d is created, as the customer paid the total price", order);
                log_event(logdb, getpid(), EN_MONEY_MONITOR, "Monitor", "INFO: transaction ok", "Success", descr);
            }
        }
        time++;
        micro_sleep(100000);
    }
    log_event(logdb, getpid(), EN_MONEY_MONITOR, "Monitor", "Stopping monitor", "Success", "The monitor stopped successfully");
}