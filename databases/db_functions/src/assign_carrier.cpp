#include "functions.h"

int assign_carrier(Con2DB db, int ord_id, int maxid){
    srand((unsigned) time(NULL));
    char sqlcmd[500];
    PGresult *res;
    sprintf(sqlcmd, "SELECT id FROM Carrier WHERE status = 0 AND id <= '%d'", maxid);
    res = db.ExecSQLtuples(sqlcmd);
    int nrows = PQntuples(res);
    if(nrows < 1) return 0;
    int id = atoi(PQgetvalue(res,rand()%nrows,PQfnumber(res,"id")));
    sprintf(sqlcmd, "UPDATE Carrier SET status = 1 WHERE id = '%d'", id);
    res = db.ExecSQLcmd(sqlcmd);
    sprintf(sqlcmd, "UPDATE Orders SET carr = '%d' WHERE id = '%d'", id, ord_id);
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);
    return id;
}