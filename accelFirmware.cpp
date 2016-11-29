#include <util/delay.h>
#include "accelFirmware.h"

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
static MPU6050 accelgyro;

void initAccel () {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    accelgyro.initialize();
    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
}

void caribrate (float *axOff, float *ayOff, float *azOff, uint16_t numberOfTest) {
    int16_t ax, ay, az;
    uint16_t count = 0;

    Serial.print("caribrating");
    for (count = 0; count < numberOfTest; count++) {
        Serial.print(count / (float)numberOfTest * 100);
        Serial.println("%");
        getAccel(&ax, &ay, &az);
        *axOff += ax;
        *ayOff += ay;
        *azOff += az;
        
        delay(25);
    }
    
    *axOff /= numberOfTest;
    *ayOff /= numberOfTest;
    *azOff /= numberOfTest;
}

void getAccel (int16_t *ax, int16_t *ay, int16_t *az) {
    accelgyro.getAcceleration(ax, ay, az);
}
