#include "functions.h"

PGresult* create_supplier(Con2DB db, string name, string address, string email, string password){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "INSERT INTO Supplier (name,addr,email,password,readstream,writestream) VALUES ('%s','%s','%s','%s','""','""') RETURNING id", name.c_str(), address.c_str(), email.c_str(), password.c_str());
    return db.ExecSQLtuples(sqlcmd);
}