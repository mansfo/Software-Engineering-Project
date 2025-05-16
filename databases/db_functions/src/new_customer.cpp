#include "functions.h"

PGresult *create_customer(Con2DB db, string name, string surname, string addr, string email, string password, string money){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "INSERT INTO Customer (name,surname,addr,email,password,money,readstream,writestream) VALUES ('%s','%s','%s','%s','%s','%s','""','""') RETURNING id", name.c_str(),surname.c_str(),addr.c_str(),email.c_str(),password.c_str(),money.c_str());
    return db.ExecSQLtuples(sqlcmd);
}