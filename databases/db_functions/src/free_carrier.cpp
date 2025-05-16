#include "functions.h"

void free_carrier(Con2DB db, int id, int distance){
    char sqlcmd[500];
    micro_sleep(distance*100000);
    snprintf(sqlcmd,500,"UPDATE Carrier SET status = 0 WHERE id = '%d'", id);
    db.ExecSQLcmd(sqlcmd);
}