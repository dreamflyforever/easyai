#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void * buzzer_run(void * arg);

static pthread_mutex_t buzzer_mtx;

void put_buzzer()
{
	pthread_mutex_unlock(&buzzer_mtx);
}

void get_buzzer()
{
	pthread_mutex_lock(&buzzer_mtx);
}

int buzzer_init(int gpio_pin)
{
#if 0
	//export gpio to user
	FILE * export_file = fopen("/sys/class/gpio/export","w");
	if (export_file == NULL) {
		perror("Failed to open GPIO export file");
		while (1);
		return -1;
	}
	fprintf(export_file,"%d",gpio_pin);
	fclose(export_file);

	printf(">>>>>%d\n", gpio_pin);
	//get the direction_path
	char direction_path[50] = {0};
	snprintf(direction_path, sizeof(direction_path), "sys/class/gpio/gpio%d/direction", gpio_pin);//get the format char
									              //specify the pin direction
	FILE *direction_file = fopen(direction_path,"w");
	if (direction_file == NULL) {
		printf("%s\n", direction_path);
		perror("Failed to open GPUI direction file");
		while (1);
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
	int retval = pthread_create(&buzzer_task, NULL, buzzer_run, (void *)&gpio_pin);
	//if (pthread_join(task, NULL) != 0) {
	//		fprintf(stderr, "Failed to join thread.\n");
	//}

	return 0;
}

void * buzzer_run(void * arg)
{
#if 0
	int gpio_pin = *(int *)arg;
	printf(">>>>>%d\n", gpio_pin);
	char value_path[50];
	snprintf(value_path,sizeof(value_path),
			"sys/class/gpio/gpio%d/value", gpio_pin);
	FILE *value_file = fopen(value_path,"w");
	if(value_file == NULL){
		perror("Failed to open GPIO value file: %d\n");
		while (1);
	}
	while (1) {
		get_buzzer();
		printf(">>>>>>>>>>>>>ring.......\n");
		fprintf(value_file,"1");
		fflush(value_file);
		sleep(1);

		fprintf(value_file,"0");
		fflush(value_file);
		sleep(1);
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
