#include <util/delay.h>
#include "accelFirmware.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

MPU6050 accelgyro;

void initAccel () {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    accelgyro.initialize();
}

void getOffset (float *axOff, float *ayOff, float *azOff) {
    uint8_t count = 0;

    uint16_t ax;
    uint16_t ay;
    uint16_t az;

    Serial.println("caribating");
    while(count < 20) {
        Serial.println(".");
        getAccel(&ax, &ay, &az);
        
        *axOff += ax / SENSITIVITY;
        *ayOff += ay / SENSITIVITY;
        *azOff += az / SENSITIVITY;
        
        count++;
        delay(500);
    }

    *axOff /= 20;
    *ayOff /= 20;
    *azOff /= 20;
}

void getAccel (uint16_t *ax, uint16_t *ay, uint16_t *az) {
    accelgyro.getAcceleration(ax, ay, az);
}
