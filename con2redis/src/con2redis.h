#ifndef con2redis_h

#define con2redis_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <hiredis/hiredis.h>

using namespace std;

#define dbg_log(fmt, ... )			\
    do { \
        fprintf(stderr, "%s:%d : " fmt "\n", __FILE__, __LINE__,__VA_ARGS__); \
    } while (0);


#define dbg_abort(fmt, ...) \
    do { \
        dbg_log(fmt, __VA_ARGS__); exit(-1); \
    } while (0)


#define RedisCommand(fmt, ...)			\
  (redisReply*) redisCommand(fmt, __VA_ARGS__)



void assertReplyType(redisContext *c, redisReply *r, int type);

void assertReply(redisContext *c, redisReply *r);

void dumpReply(redisReply *r, int indent);

void initStreams(redisContext *c, const char *stream);

void ReadStreamReply(redisReply *r, int indent, int readstatus);

int ReadStreamName(redisReply *r, char *streamname, long unsigned int k);

int ReadNumStreams(redisReply *r);

int ReadStreamNumMsg(redisReply *r, long unsigned int streamnum);

int ReadStreamNumMsgID(redisReply *r, long unsigned int streamnum, int msgnum, char *msgid);

int ReadStreamMsgNumVal(redisReply *r, long unsigned int streamnum, int msgnum);

int ReadStreamMsgVal(redisReply *r, long unsigned int streamnum, int msgnum, int entry, char *fval);

bool badRequest(redisReply *r);

string getMsg(redisReply *r);

vector<string> getReply(redisReply *r);

int getReplyRec(redisReply* r, vector<string> &result);

string add_reply_minus_one(string r);

#endif
