#ifndef MONITOR_MAIN_H
#define MONITOR_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include "../../con2db/pgsql.h"
#include "../../databases/db_functions/src/functions.h"
#include "../../ecommerce_backend/src/main.h"

#define ACCESS_MONITOR 1
#define AVAIL_MONITOR 2
#define EN_MONEY_MONITOR 3
#define DELIVERY_TIME_MONITOR 4
#define UPDATE_ORDER_MONITOR 5

void update_order_monitor();
void enough_money_monitor();
void availability_monitor();
void access_monitor();
void delivery_time_monitor();
int micro_sleep(long usec);

#endif