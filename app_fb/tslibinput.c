// for sched.h
#define _GNU_SOURCE


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>
#include <sched.h>

#include <tslib.h>

#include "tslibinput.h"

#define SAMPLES 1
#define SLOTS 1

static int last_x = 0;
static int last_y = 0;
static int event = 0;

static struct tsdev *ts;
static struct ts_sample_mt **samp_mt = NULL;
static pthread_t thread1;


void schedule_cpu(int cpuIndex) {
    cpu_set_t mask;
    
    CPU_ZERO(&mask);
    CPU_SET(cpuIndex, &mask);
	if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
		printf("set_cpu: failed to set thread affinity\n");
	}
}

static void* read_input(void* data) {
    int ret, i, j;

    schedule_cpu(2);

    while(1) {
//    printf("I: tslibinput_read\n");


    ret = ts_read_mt(ts, samp_mt, SLOTS, SAMPLES);
    if (ret < 0) {
            printf("ERROR: ts_read_mt");
            return NULL;
    }

    for (j = 0; j < ret; j++) {
   	for (i = 0; i < SLOTS; i++) {
        #ifdef TSLIB_MT_VALID
        if (!(samp_mt[j][i].valid & TSLIB_MT_VALID))
            continue;
        #else
        if (samp_mt[j][i].valid < 1)
            continue;
        #endif
        last_x = samp_mt[j][i].x;
        last_y = samp_mt[j][i].y;
        event = samp_mt[j][i].pressure ? 2 : 1;
		
#if 0
        printf("%ld.%06ld: (slot %d) %6d %6d %6d\n",
           samp_mt[j][i].tv.tv_sec,
           samp_mt[j][i].tv.tv_usec,
           samp_mt[j][i].slot,
           samp_mt[j][i].x,
           samp_mt[j][i].y,
           samp_mt[j][i].pressure);
#endif
    }
    }
    }
    return data;
}

void tslibinput_init(void)
{
    char *tsdevice = NULL;
    int ret, i;
    
    ts = ts_setup(tsdevice, 0);
    if (!ts) {
            printf("ERROR: ts_setup failed.\n");
            return ;
    }
    
    samp_mt = malloc(SAMPLES * sizeof(struct ts_sample_mt *));
    if (!samp_mt) {
            ts_close(ts);
            printf("ERROR: ts failed to allocate samples.\n");
            return;
    }
    
    for (i = 0; i < SAMPLES; i++) {
        samp_mt[i] = calloc(SLOTS, sizeof(struct ts_sample_mt));
        if (!samp_mt[i]) {
            for (i--; i >= 0; i--)
                    free(samp_mt[i]);
            free(samp_mt);
            ts_close(ts);
            printf("ERROR: ts failed to allocate samples.\n");
            return;
        }
    }
    
    //start reading thread
    pthread_create(&thread1, NULL, read_input, NULL);
    printf("I: tslibinput_init OK\n");
}

/**
 * Get the current position and state of the mouse
 * @param indev_drv pointer to the related input device driver
 * @param data store the mouse data here
 */
void tslibinput_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    (void) indev_drv;      /*Unused*/
    if (!event) {
        return;
    }
    
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = event == 2 ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    event = 0;
}
