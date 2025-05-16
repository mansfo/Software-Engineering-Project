#include "main.h"

void availability_monitor(){
    Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
    micro_sleep(1000);
    log_event(logdb, getpid(), AVAIL_MONITOR, "Monitor", "Starting monitor", "Success", "The monitor started successfully");
    int last_id = 0;
    int time = 0;
    while(time < MAX_TIME/100){
        char sqlcmd[100];
        snprintf(sqlcmd,100,"SELECT id,avail,quantity,ord FROM PurchasesLogTable WHERE id > %d ORDER BY id ASC", last_id);
        PGresult *res = logdb.ExecSQLtuples(sqlcmd);
        int rows = PQntuples(res);
        for(int i=0; i<rows; i++){
            if(i==rows-1) last_id = atoi(PQgetvalue(res,i,PQfnumber(res,"id")));
            int order = atoi(PQgetvalue(res,i,PQfnumber(res,"ord")));
            int avail = atoi(PQgetvalue(res,i,PQfnumber(res,"avail")));
            int quantity = atoi(PQgetvalue(res,i,PQfnumber(res,"quantity")));
            if(quantity > avail){
                char descr[100];
                snprintf(descr,100,"An error occurred on the order %d, as there are not enough products in the storage", order);
                log_event(logdb, getpid(), AVAIL_MONITOR, "Monitor", "WARNING: not enough products", "Success", descr);
            }
            else{
                char descr[100];
                snprintf(descr,100,"The order %d is accepted", order);
                log_event(logdb, getpid(), AVAIL_MONITOR, "Monitor", "INFO: order accepted", "Success", descr);
            }
        }
        time++;
        micro_sleep(100000);
    }
    log_event(logdb, getpid(), AVAIL_MONITOR, "Monitor", "Stopping monitor", "Success", "The monitor stopped successfully");
}