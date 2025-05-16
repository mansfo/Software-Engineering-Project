#include "main.h"

void access_monitor(){
    Con2DB logdb("localhost", "5432", "ecommerce_server", "02468", "ecommerce_logdb");
    micro_sleep(1000);
    log_event(logdb, getpid(), ACCESS_MONITOR, "Monitor", "Starting monitor", "Success", "The monitor started successfully");
    unordered_map<string,int> wrong_accesses;
    int last_id = 0;
    int time = 0;
    while(time < MAX_TIME/10){
        char sqlcmd[100];
        snprintf(sqlcmd,100,"SELECT id,email,successfull FROM AccessLogTable WHERE id > %d ORDER BY id ASC",last_id);
        PGresult *res;
        res = logdb.ExecSQLtuples(sqlcmd);
        int rows = PQntuples(res);
        for(int i=0;i<rows;i++){
            string em = PQgetvalue(res,i,PQfnumber(res,"email"));
            if(!wrong_accesses.count(em)) wrong_accesses.insert({em,0});
            else if(atoi(PQgetvalue(res,i,PQfnumber(res,"successfull")))) wrong_accesses.at(em) = 0;
            else wrong_accesses.at(em) += 1;
            if(wrong_accesses.at(em)>5){
                char descr[100];
                snprintf(descr,100,"A wrong password for the email %s was inserted %d times in a row", em.c_str(), wrong_accesses.at(em));
                log_event(logdb, getpid(), ACCESS_MONITOR, "Monitor", "WARNING: multiple access error", "Success", descr);
            }
            else if(wrong_accesses.at(em)>0 && wrong_accesses.at(em)<= 5){
                char descr[100];
                snprintf(descr,100,"A wrong password for the email %s was inserted %d times in a row", em.c_str(), wrong_accesses.at(em));
                log_event(logdb, getpid(), ACCESS_MONITOR, "Monitor", "INFO: multiple access error", "Success", descr);
            }
            if(i == rows-1) last_id = atoi(PQgetvalue(res,i,PQfnumber(res,"id")));
        }
        time++;
        micro_sleep(1000);
    }
    log_event(logdb, getpid(), ACCESS_MONITOR, "Monitor", "Stopping monitor", "Success", "The monitor stopped successfully");
}