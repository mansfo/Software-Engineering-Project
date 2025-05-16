#include "functions.h"

void log_access(Con2DB db, string email, int successfull){
    char sqlcmd[200];
    snprintf(sqlcmd, 200, "INSERT INTO AccessLogTable (email,successfull,tstamp,unixtime) VALUES ('%s', '%d', NOW(),'%ld')",email.c_str(),successfull,micro_time());
    PGresult *res;
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);
}