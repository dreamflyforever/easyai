#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

static pthread_cond_t cond;
void * buzzer_run(void * arg);
char g_speech[100];
static pthread_mutex_t buzzer_mtx;

extern int alsa_play(char * path);
void put_buzzer(char * speech)
{
	printf("release signal.......\n");
	strncpy(g_speech, speech, 100);
	pthread_cond_signal(&cond);
}

void get_buzzer()
{
	pthread_cond_wait(&cond, &buzzer_mtx);/*等待*/
}
int buzzer_init(int gpio_pin)
{
#if 1
	//export gpio to user
	FILE * export_file = fopen("/sys/class/gpio/export","w");
	if (export_file == NULL) {
		perror("Failed to open GPIO export file");
		assert(0);
		return -1;
	}
	fprintf(export_file,"%d",gpio_pin);
	fclose(export_file);

	//get the direction_path
	char direction_path[50] = {0};
	snprintf(direction_path, sizeof(direction_path), "/sys/class/gpio/gpio%d/direction", gpio_pin);//get the format char
									              //specify the pin direction
	FILE *direction_file = fopen(direction_path,"w");
	if (direction_file == NULL) {
		printf("%s\n", direction_path);
		perror("Failed to open GPIO direction file");
		assert(0);
		return -1;
	}
	fprintf(direction_file,"out");
	fclose(direction_file);
#else
	system("echo 55 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio55/direction");	
#endif
	pthread_mutex_init(&buzzer_mtx, NULL);
	pthread_t buzzer_task;
	cond = PTHREAD_COND_INITIALIZER;

	int retval = pthread_create(&buzzer_task, NULL, buzzer_run, (int *)gpio_pin);
	//if (pthread_join(task, NULL) != 0) {
	//		fprintf(stderr, "Failed to join thread.\n");
	//}

	return 0;
}

void * buzzer_run(void * arg)
{
#if 1
	int gpio_pin = (int)(int *)arg;
	char value_path[50] = {0};
	snprintf(value_path,sizeof(value_path),
			"/sys/class/gpio/gpio%d/value", gpio_pin);
	FILE *value_file = fopen(value_path,"w");
	if(value_file == NULL){
		printf("Failed to open GPIO value file: %s\n", value_path);
		assert(0);
	}
	while (1) {
		printf("wait signal..........................\n");
		pthread_mutex_lock(&buzzer_mtx);
		get_buzzer();
		fprintf(value_file,"0");
		fflush(value_file);
		fprintf(value_file,"1");
		fflush(value_file);
		printf(">>>>>>>>>>>>>%s.......\n", g_speech);
		alsa_play(g_speech);
		fprintf(value_file,"0");
		pthread_mutex_unlock(&buzzer_mtx);
	}

	fclose(value_file);
	return 0;
#else
	while (1) {
		get_buzzer();
		system("echo 1 > /sys/class/gpio/gpio55/value");
		sleep(1);
		system("echo 0 > /sys/class/gpio/gpio55/value");
	}
#endif 
}
