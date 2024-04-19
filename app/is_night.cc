#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

void * day_run(void * arg);

int is_night_thread()
{
	
	pthread_t day_task;
	int retval = pthread_create(&day_task, NULL, day_run, NULL);
	return 0;
}

void * day_run(void * arg)
{
	int is_day;
	while (1) {
		is_day = detect_is_day();
		if (is_day == 1) {
		/* close frared lamp */

		} else {
		/* open frared lamp */


		}
		sleep(10);
	}
}
