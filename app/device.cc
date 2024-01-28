#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void * buzzer_run(void * arg);

int buzzer_init(int gpio_pin)
{
	//export gpio to user
	FILE * export_file = fopen("/sys/class/gpio/export","w");
	if (export_file == NULL) {
		perror("Failed to open GPIO export file");
		return -1;
	}
	fprintf(export_file,"%d",gpio_pin);
	fclose(export_file);

	//get the direction_path
	char direction_path[50];
	snprintf(direction_path, sizeof(direction_path), "sys/class/gpio/gpio%d/direction", gpio_pin);//get the format char
									              //specify the pin direction
	FILE *direction_file = fopen(direction_path,"w");
	if (direction_file == NULL) {
		perror("Failed to open GPUI direction file");
		return -1;
	}
	fprintf(direction_file,"out");
	fclose(direction_file);

	pthread_t buzzer_task;
	int retval = pthread_create(&buzzer_task, NULL, buzzer_run, (void *)&gpio_pin);
	//if (pthread_join(task, NULL) != 0) {
	//		fprintf(stderr, "Failed to join thread.\n");
	//}

	return 0;
}

void * buzzer_run(void * arg)
{
	int gpio_pin = *(int *)arg;
	char value_path[50];
	snprintf(value_path,sizeof(value_path),
			"sys/class/gpio/gpio%d/value", gpio_pin);
	FILE *value_file = fopen(value_path,"w");
	if(value_file == NULL){
		perror("Failed to open GPIO value file: %d\n");
	}
	while (1) {
		fprintf(value_file,"1");
		fflush(value_file);
		sleep(1);

		fprintf(value_file,"0");
		fflush(value_file);
		sleep(1);
	}

	fclose(value_file);
	return 0;
}
