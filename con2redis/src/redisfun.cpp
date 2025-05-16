#include "con2redis.h"
#include "local.h"

void print_reply_types()
{
  printf("REDIS_REPLY_STRING=%d,\
REDIS_REPLY_STATUS=%d,\
REDIS_REPLY_INTEGER=%d,\
REDIS_REPLY_NIL=%d,\
REDIS_REPLY_ERROR=%d,\
REDIS_REPLY_ARRAY=%d\n",
	 REDIS_REPLY_STRING,
	 REDIS_REPLY_STATUS,
	 REDIS_REPLY_INTEGER,
	 REDIS_REPLY_NIL,
	 REDIS_REPLY_ERROR,
	 REDIS_REPLY_ARRAY
	 );
  
}


void assertReplyType(redisContext *c, redisReply *r, int type) {
    if (r == NULL)
        dbg_abort("NULL redisReply (error: %s)", c->errstr);
    if (r->type != type)
      { print_reply_types();
	dbg_abort("Expected reply type %d but got type %d", type, r->type);
      }
}

bool badRequest(redisReply *r){
    vector<string> rv = getReply(r);
    bool bad = r == NULL || r->type == REDIS_REPLY_NIL || (rv.size()>3 && strcmp(rv.at(2).c_str(),"msg"));
    return bad;
}

void assertReply(redisContext *c, redisReply *r) {
    if (r == NULL)
        dbg_abort("pid %d NULL redisReply (error: %s)", getpid(), c->errstr);
}

string getMsg(redisReply *r){
    if(!badRequest(r)){
        vector<string> rv = getReply(r);
        if(rv.size()>3) return rv.at(3);
    }
    return "";
}

string add_reply_minus_one(string r){
    int i=3;
    while(r.at(r.length()-i) == '0'){
        r.at(r.length()-i) ='9';
        i++;
    }
    r.at(r.length()-i) = r.at(r.length()-i)-1;
    return r;
}


void dumpReply(redisReply *r, int indent) {

    std::string buffer = "";
    char aux[100];

    switch (r->type) {
        case REDIS_REPLY_STRING:
            snprintf(aux,100,"(string) %s\n",r->str);
            buffer.append(aux);
            break;
        case REDIS_REPLY_STATUS:
            snprintf(aux,100,"(status) %s\n", r->str);
            buffer.append(aux);
            break;
        case REDIS_REPLY_INTEGER:
            snprintf(aux,100,"(integer) %lld\n", r->integer);
            buffer.append(aux);
            break;
        case REDIS_REPLY_NIL:
            snprintf(aux,100,"(nill)\n");
            buffer.append(aux);
            break;
        case REDIS_REPLY_ERROR:
            snprintf(aux,100," (error) %s", r->str);
            buffer.append(aux);
            break;
        case REDIS_REPLY_ARRAY:
            for (size_t i = 0; i < r->elements; i++) {
                dumpReply(r->element[i], indent + 2);
            }
            break;
        default:
            dbg_abort("PID: %d, Don't know how to deal with reply type %d", getpid(), r->type);
    }
    if (buffer.length() > 0) {
        for (int i = 0; i < indent; i++)
            printf(" ");

        printf("%s", buffer.c_str());
    }
}




void initStreams(redisContext *c, const char *stream) {
    redisReply *r = RedisCommand(c, "XGROUP CREATE %s diameter $ MKSTREAM", stream);
    assertReply(c, r);
    freeReplyObject(r);
}
