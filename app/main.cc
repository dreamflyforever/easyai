#include <sys/time.h>
#include "core.h"

extern void put_buzzer(char * speech);
extern int buzzer_init(int gpio_pin);
extern int get_device_status();
extern int is_device_run();

int sleep_times;
int noface_times;
int lr_times;
int phone_times;
int normal_times;

int main(int argc, char **argv)
{
	os_printf("compile time : %s\n", __TIME__);
	int ret;
	int first_check = 1;
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
	is_device_run();
#if MQTT
	mqtt_init();
	pub("hello", "12344", 5);
#endif
	while (1) {
		camera_read(entity);
		preprocess(entity);
		inference(entity);
		ret = postprocess(entity);
		printf(">>>>>>>>>>>%d\n", ret);
		((ret == 2) || (ret == 4) || (ret == 5)) ? sleep_times++ : sleep_times = 0;
		(ret == -1)? noface_times++ : noface_times = 0;
		(ret == 0) ? lr_times++ : lr_times = 0;
		(ret == 3) ? phone_times++ : phone_times = 0;
		(ret == 1) ? normal_times++ : normal_times = 0;
		switch (ret) {
		case 0:
			if (lr_times>=15)
				put_buzzer("/oem/ws/model/leftright.wav");
			break;
		case 1:
			if (first_check == 1) {
				put_buzzer("/oem/ws/model/first_check.wav");
				first_check = 0;
			}
			if (normal_times>=(5 * 10 * 60)) {
				int a;
				srand((unsigned)time(NULL));
				a = rand();
				char tmp[100] = {0};
				snprintf(tmp, 100, "/oem/ws/model/normal%d.wav", a%3);
				put_buzzer(tmp);
				normal_times = 0;
			}
			break;
		case 2:
		case 4:
		case 5:
			if ((sleep_times>=4) && (sleep_times < 7)) {
					struct timeval tv;
					gettimeofday(&tv, NULL);
					printf("second: %ld\n", tv.tv_sec);
					int select = (tv.tv_sec % 3);
					if (select == 1)
						put_buzzer("/oem/ws/model/openmouth.wav");
					else if (select == 2)
						put_buzzer("/oem/ws/model/headdown.wav");
					else
						put_buzzer("/oem/ws/model/check_sleep.wav");
			}
			if (sleep_times>=7)
				put_buzzer("/oem/ws/model/bb.wav");

			break;
		case 3:
			if (phone_times>=10)
				put_buzzer("/oem/ws/model/mobile.wav");

			break;

		default:
			if (first_check || get_device_status()) {
				if (noface_times>=10) {
					system("echo 0 > /sys/class/gpio/gpio54/value");
				}
				if (noface_times>=20) {
					put_buzzer("/oem/ws/model/verify.wav");
					/*open IR led*/
					system("echo 0 > /sys/class/gpio/gpio54/value");
				}
			}
			break;
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
