#include "functions.h"

PGresult* login_supplier(Con2DB db, string id){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "SELECT name,addr FROM Supplier WHERE id = '%s'", id.c_str());
    return db.ExecSQLtuples(sqlcmd);
}

int login_with_psw_supplier(Con2DB db, Con2DB logdb, string email, string password){
    char sqlcmd[500];
    snprintf(sqlcmd,500,"SELECT id FROM Supplier WHERE email = '%s'",email.c_str());
    PGresult *res;
    res = db.ExecSQLtuples(sqlcmd);
    if(!PQntuples(res)) return -1;
    snprintf(sqlcmd,500,"SELECT id FROM Supplier WHERE email = '%s' AND password = '%s'",email.c_str(), password.c_str());
    res = db.ExecSQLtuples(sqlcmd);
    if(!PQntuples(res)) {
        log_access(logdb,email,0);
        return -2;
    }
    int id = atoi(PQgetvalue(res,0,PQfnumber(res,"id")));
    log_access(logdb, email, 1);
    PQclear(res);
    return id;
}