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
}

void caribrate (uint16_t *axOff, uint16_t *ayOff, uint16_t *azOff) {
    uint8_t count = 0;
    int16_t ax, ay, az;
    
    Serial.print("caribrating");
    while(count < 20) {
        Serial.print(".");
        getAccel(&ax, &ay, &az);
        *axOff += ax;
        *ayOff += ay;
        *azOff += az;
        
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

