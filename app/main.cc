#include "core.h"

extern void put_buzzer(char * speech);
extern int buzzer_init(int gpio_pin);

int main(int argc, char **argv)
{
	int ret;
	if (argc != 2) {
		printf("%s <model_path>\n", argv[0]);
		return -1;
	}
	session_str * entity;
	session_init(&entity, argv[1]);
	camera_init(entity);

	buzzer_init(55);
	while (1) {
		camera_read(entity);
		preprocess(entity);
		inference(entity);
		ret = postprocess(entity);
		if (ret == 1) {
			put_buzzer(NULL);
			printf("detect people ring....\n");
		}
		updatefps();
#if 1
		char *str = (char *)malloc(20);
		memset(str, 0, 20);
		static int t = 0;
		sprintf(str, "%d_out.jpeg", t);
		write_image(str, &(entity->src_image));
		free(str);
		t++;
		/* because the flash is only 24MB*/
		if (t == 50) {
			os_printf("reach the amount of picutre, recover picture...\n");
			t = 0;
		}
#endif
	}
	camera_deinit(entity);
	session_deinit(entity);
	return ret;
}
