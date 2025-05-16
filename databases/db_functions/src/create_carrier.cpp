#include "functions.h"

PGresult *create_carrier(Con2DB db, string name, string surn, string comp){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "INSERT INTO Carrier (name,surname,company,status,readstream,writestream) VALUES ('%s','%s','%s',0,'""','""') RETURNING id", name.c_str(), surn.c_str(), comp.c_str());
    return db.ExecSQLtuples(sqlcmd);
}