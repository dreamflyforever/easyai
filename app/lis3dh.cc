#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define I2C_DEVICE "/dev/i2c-3" // I2C 
#define LIS3DHTR_ADDR 0x19      // LIS3DHTR I2C 
#define WHO_AM_I     0x0F       // ID 
#define CTRL_REG1    0x20       // 1
#define CTRL_REG4    0x23       // 4
#define OUT_X_L      0x28       // X 
#define OUT_X_H      0x29       // X 
#define OUT_Y_L      0x2A       // Y 
#define OUT_Y_H      0x2B       // Y 
#define OUT_Z_L      0x2C       // Z 
#define OUT_Z_H      0x2D       // Z 

#define QUEUE_SIZE 100
#define THRESHOLD 0.07
#define RUN 1
#define STOP 0


typedef struct {
    float data[QUEUE_SIZE];
    int front;
    int rear;
    int count;
} FIFOQueue;

void initQueue(FIFOQueue* queue) {
    queue->front = 0;
    queue->rear = -1;
    queue->count = 0;
}

int isFull(FIFOQueue* queue) {
    return queue->count == QUEUE_SIZE;
}

int isEmpty(FIFOQueue* queue) {
    return queue->count == 0;
}

void enqueue(FIFOQueue* queue, float value) {
    if (isFull(queue)) {
        queue->front = (queue->front + 1) % QUEUE_SIZE;
    } else {
        queue->count++;
    }
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->data[queue->rear] = value;
}

float dequeue(FIFOQueue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty\n");
        exit(1);
    }
    float value = queue->data[queue->front];
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->count--;
    return value;
}

float getQueueElement(FIFOQueue* queue, int index) {
    if (index < 0 || index >= queue->count) {
        printf("Index out of bounds\n");
        exit(1);
    }
    return queue->data[(queue->front + index) % QUEUE_SIZE];
}

float computeAbsoluteValue(float x, float y, float z) {
    return sqrt(x * x + y * y + z * z);
}

void updateChangeRateAndMean(FIFOQueue* absQueue, FIFOQueue* changeRateQueue, float newValue, float* meanChangeRate) {
    if (absQueue->count > 0) {
        float lastValue = getQueueElement(absQueue, absQueue->count - 1);
        float changeRate = fabs(newValue - lastValue);
        float oldMean = *meanChangeRate;

        if (isFull(changeRateQueue)) {
            float oldestChangeRate = dequeue(changeRateQueue);
            *meanChangeRate = oldMean + (changeRate - oldestChangeRate) / QUEUE_SIZE;
        } else {
            *meanChangeRate = (oldMean * changeRateQueue->count + changeRate) / (changeRateQueue->count + 1);
        }

        enqueue(changeRateQueue, changeRate);
    }

    enqueue(absQueue, newValue);
}

void i2c_write(int file, uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;
    if (write(file, buf, 2) != 2) {
        perror("Failed to write to the i2c bus");
    }
}

uint8_t i2c_read_byte(int file, uint8_t reg) {
    uint8_t buf[1];
    if (write(file, &reg, 1) != 1) {
        perror("Failed to write to the i2c bus");
    }
    if (read(file, buf, 1) != 1) {
        perror("Failed to read from the i2c bus");
    }
    return buf[0];
}

void i2c_read_bytes(int file, uint8_t reg, uint8_t *data, size_t length) {
    if (write(file, &reg, 1) != 1) {
        perror("Failed to write to the i2c bus");
    }
    if (read(file, data, length) != length) {
        perror("Failed to read from the i2c bus");
    }
}
static float g_meanChangeRate;
// return
// 0-stop, 1-run
int get_device_status() {
    if (g_meanChangeRate > THRESHOLD)
        return RUN;
    return STOP;
}

void * lis3hd_main(void * arg)
{
    int i2c_file = open(I2C_DEVICE, O_RDWR);
    if (i2c_file < 0) {
        perror("Failed to open the i2c bus");
        exit(1);
    }

    if (ioctl(i2c_file, I2C_SLAVE, LIS3DHTR_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        close(i2c_file);
        exit(1);
    }

    uint8_t who_am_i = i2c_read_byte(i2c_file, WHO_AM_I);
    if (who_am_i != 0x33) {
        fprintf(stderr, "Failed to connect to LIS3DHTR: WHO_AM_I = 0x%02X\n", who_am_i);
        close(i2c_file);
        exit(1);
    }

    i2c_write(i2c_file, CTRL_REG1, 0x57); // 0x57 = 0b01010111

    i2c_write(i2c_file, CTRL_REG4, 0x00); // 0x00 = 0b00000000, 设置 FS1 和 FS0 为 00

    FIFOQueue xQueue, yQueue, zQueue;
    FIFOQueue absQueue, changeRateQueue;
    float meanChangeRate = 0;

    initQueue(&xQueue);
    initQueue(&yQueue);
    initQueue(&zQueue);
    initQueue(&absQueue);
    initQueue(&changeRateQueue);

    float dt = 0.1; // 10ms
    struct timespec sleep_time = {0, dt * 1000000000L}; // 
    int cnt = 0;

    while (1) {
        uint8_t accel_data[6];
        i2c_read_bytes(i2c_file, OUT_X_L | 0x80, accel_data, 6); // 0x80 

        int16_t accel_x = (accel_data[1] << 8) | accel_data[0];
        int16_t accel_y = (accel_data[3] << 8) | accel_data[2];
        int16_t accel_z = (accel_data[5] << 8) | accel_data[4];

        float accel_x_g = accel_x / 16384.0; //  +-2g
        float accel_y_g = accel_y / 16384.0;
        float accel_z_g = accel_z / 16384.0;

        enqueue(&xQueue, accel_x_g);
        enqueue(&yQueue, accel_y_g);
        enqueue(&zQueue, accel_z_g);

        float absValue = computeAbsoluteValue(accel_x_g, accel_y_g, accel_z_g);

        updateChangeRateAndMean(&absQueue, &changeRateQueue, absValue, &meanChangeRate);
	    g_meanChangeRate = meanChangeRate;

        // send STOP to upstream task before first 10s(100ms * 100)
        if (cnt < 100) {
            cnt++;
            g_meanChangeRate = 0;       
        }

        printf("Acceleration: X=%.4f g, Y=%.4f g, Z=%.4f g, Mean Change Rate: %.4f\n", accel_x_g, accel_y_g, accel_z_g, meanChangeRate);

        nanosleep(&sleep_time, NULL);
    }

    close(i2c_file);
    return 0;
}

int is_device_run()
{
	pthread_t run_task;
	int retval = pthread_create(&run_task, NULL, lis3hd_main, NULL);
	return 0;
}
