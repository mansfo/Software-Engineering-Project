#include "main.h"

vector<Credential> load_credentials(Con2DB db, int table) {
    
    char sqlcmd[100];
    snprintf(sqlcmd,100,"SELECT email,password FROM %s", table==0? "Customer" : "Supplier");
    PGresult* res = db.ExecSQLtuples(sqlcmd);
   
    vector<Credential> credentials;
    string line;

    for(int i=0; i<PQntuples(res); i++){
        string email, correct, wrong;
        email = PQgetvalue(res,i,PQfnumber(res,"email"));
        correct = PQgetvalue(res,i,PQfnumber(res,"password"));
        wrong = generate_random_str(10);
        credentials.push_back({email, correct, wrong});
    }

    return credentials;
}