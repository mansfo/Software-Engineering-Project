#include "main.h"

void update_order_monitor(){
    Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
    micro_sleep(1000);
    log_event(logdb, getpid(), UPDATE_ORDER_MONITOR, "Monitor", "Starting monitor", "Success", "The monitor started successfully");
    unordered_map<int,int> curr_status;
    int last_id = 0;
    int time = 0;
    while(time < MAX_TIME/100){
        char sqlcmd[100];
        snprintf(sqlcmd,100,"SELECT id,ord,status FROM OrderLogTable WHERE id > %d", last_id);
        PGresult *res = logdb.ExecSQLtuples(sqlcmd);
        int rows = PQntuples(res);
        for(int i=0; i<rows; i++){
            if(i == rows-1) last_id = atoi(PQgetvalue(res,i,PQfnumber(res,"id")));
            int order = atoi(PQgetvalue(res,i,PQfnumber(res,"ord")));
            int status = atoi(PQgetvalue(res,i,PQfnumber(res,"status")));
            if(!curr_status.count(order)) curr_status.insert({order,order_status::not_sent});
            if(curr_status.at(order)!=order_status::not_sent && status == order_status::deleted){
                char descr[100];
                snprintf(descr,100,"The customer tried to delete the order %d but it is not possible anymore",order);
                log_event(logdb, getpid(), UPDATE_ORDER_MONITOR, "Monitor", "WARNING: invalid order cancellation", "Success", descr);
            }
            else {
                if(curr_status.at(order)==order_status::not_sent && status == order_status::deleted){
                    char descr[100];
                    snprintf(descr,100,"The customer deleted the order %d",order);
                    log_event(logdb, getpid(), UPDATE_ORDER_MONITOR, "Monitor", "INFO: valid cancellation", "Success", descr);
                }
                curr_status.at(order) = status;
            }
        }
        time++;
        micro_sleep(100000);
    }
    log_event(logdb, getpid(), UPDATE_ORDER_MONITOR, "Monitor", "Stopping monitor", "Success", "The monitor stopped successfully");
}