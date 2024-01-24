#include "core.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("%s <model_path>\n", argv[0]);
		return -1;
	}
	session_str * entity;
	session_init(&entity, argv[1]);
	camera_init(entity);
	while (1) {
		camera_read(entity);
		preprocess(entity);
		inference(entity);
		postprocess(entity);
		updatefps();
	}
	session_deinit(entity);
}
