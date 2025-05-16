#ifndef main_h
#define main_h

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../../con2db/pgsql.h"
#include "../../client/src/main.h"
#include "../../databases/db_functions/src/functions.h"
#include "../../con2redis/src/con2redis.h"
#include "../../server/src/main.h"
#include "../../monitor/src/main.h"

#define MAX_TIME 1000000

using namespace std;

#endif