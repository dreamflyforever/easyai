#include "core.h"

extern void put_buzzer(char * speech);
extern int buzzer_init(int gpio_pin);

int main(int argc, char **argv)
{
	os_printf("compile time : %s\n", __TIME__);
	int ret;
	if (argc != 2) {
		printf("%s <model_path>\n", argv[0]);
		return -1;
	}
	session_str * entity;
	buzzer_init(55);
	session_init(&entity, argv[1]);
	camera_init(entity);
	put_buzzer("/oem/ws/model/welcome.wav");
	is_night_thread();
	while (1) {
		camera_read(entity);
		preprocess(entity);
		inference(entity);
		ret = postprocess(entity);
		if (ret == 1) {
			put_buzzer("/oem/ws/model/check_people.wav");
			printf("detect people ring....\n");
		}
		updatefps();
#if 1
		char *str = (char *)malloc(20);
		memset(str, 0, 20);
		static int t = 0;
		sprintf(str, "%d_out.jpeg", t);
		write_image(str, &(entity->src_image));
		/* because the flash is only 24MB*/
		if (t >= 5) {
			os_printf("reach the amount of picutre, recover picture...\n");
			memset(str, 0, 20);
			sprintf(str, "%d_out.jpeg", (t-5));
			remove(str);
			t = -1;
		}
		t++;
		free(str);
#endif
	}
	camera_deinit(entity);
	session_deinit(entity);
	return ret;
}
