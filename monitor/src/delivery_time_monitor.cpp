#include "main.h"

void delivery_time_monitor(){
    Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
    micro_sleep(1000);
    unordered_map<int,long> orders_time;
    log_event(logdb, getpid(), DELIVERY_TIME_MONITOR, "Monitor", "Starting monitor", "Success", "The monitor started successfully");
    int mtime = 0;
    int last_id = 0;
    while(mtime < MAX_TIME/100){
        char sqlcmd[100];
        snprintf(sqlcmd,100,"SELECT id,ord,status,unixtime FROM OrderLogTable WHERE id > %d", last_id);
        PGresult *res = logdb.ExecSQLtuples(sqlcmd);
        int rows = PQntuples(res);
        for(int i=0; i<rows; i++){
            if(i==rows-1) last_id = atoi(PQgetvalue(res,i,PQfnumber(res,"id")));
            int order = atoi(PQgetvalue(res,i,PQfnumber(res,"ord")));
            int status = atoi(PQgetvalue(res,i,PQfnumber(res,"status")));
            long ord_time = stol(PQgetvalue(res,i,PQfnumber(res,"unixtime")));
            char descr[100];
            if(!orders_time.count(order)) orders_time.insert({order,ord_time});
            else if(status == order_status::deleted || status == order_status::deleting_ok) orders_time.erase(order);
            else if(micro_time()-ord_time>700000L){ 
                if(status == order_status::arrived){
                    snprintf(descr,100,"The order %d has been delivered with %ld days of delay", order, (micro_time()-ord_time)/100000L + 1L);
                    log_event(logdb, getpid(), DELIVERY_TIME_MONITOR, "Monitor", "WARNING: delivery on late warning", "Success", descr);
                    orders_time.erase(order);
                }
                else {
                    snprintf(descr,100,"The delivery of the order %d has some delay", order);
                    log_event(logdb, getpid(), DELIVERY_TIME_MONITOR, "Monitor", "WARNING: delivery on late warning", "Success", descr);
                }
            }
            else{
                snprintf(descr,100,"The delivery of the order %d is in progress without known delays", order);
                log_event(logdb, getpid(), DELIVERY_TIME_MONITOR, "Monitor", "INFO: delivery", "Success", descr);
            }
        }
        mtime++;
        micro_sleep(100000);
    }
    log_event(logdb, getpid(), DELIVERY_TIME_MONITOR, "Monitor", "Stopping monitor", "Success", "The monitor stopped successfully");
}