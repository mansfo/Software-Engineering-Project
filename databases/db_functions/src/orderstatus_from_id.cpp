#include "functions.h"

order_status orderstatus_from_id(Con2DB db, int ord){
    PGresult *res;
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "SELECT status FROM Orders WHERE id = '%d'", ord);
    res = db.ExecSQLtuples(sqlcmd);
    order_status status = int_to_status(PQgetvalue(res, 0, PQfnumber(res, "status")));
    return status;
}