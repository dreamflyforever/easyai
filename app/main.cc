#include "core.h"

#if 1
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

	system("echo 55 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio55/direction");

	while (1) {
		camera_read(entity);
		preprocess(entity);
		inference(entity);
		ret = postprocess(entity);
		if (ret == 1) {
			system("echo 1 > /sys/class/gpio/gpio55/value");
			sleep(1);
			system("echo 0 > /sys/class/gpio/gpio55/value");
			printf("detect people ring....\n");
		}
		updatefps();
#if 0
		char *str = (char *)malloc(20);
		memset(str, 0, 20);
		static int t = 0;
		sprintf(str, "%d_out.jpeg", t);
		write_image(str, &(entity->src_image));
		free(str);
		t++;
		/* because the flash is only 24MB*/
		if (t == 50) {
			os_printf("reach the amount of picutre, exit...\n");
			break;
		}
#endif
	}
	camera_deinit(entity);
	session_deinit(entity);
	return ret;
}
#endif
