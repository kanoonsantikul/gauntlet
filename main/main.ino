#include "accelFirmware.h"

#define NOISE_Y 0.35

int16_t ax, ay, az;
int16_t axOff, ayOff, azOff;
float fax, fay, faz;
float yMax;

void setup () {
    Serial.begin(38400);
    initAccel();
    caribrate(&axOff, &ayOff, &azOff);
}

void loop () {
    getAccel(&ax, &ay, &az);

    fax = (ax - axOff) / SENSITIVITY;
    fay = (ay - ayOff) / SENSITIVITY;
    faz = (az - azOff - SENSITIVITY) / SENSITIVITY;

    //graph
    Serial.println(fay);
    //getMax();
}


//void getMax() {
//    //Serial.println(yMax);
//    //Serial.print("fay: "); Serial.println(fay);
//    if (fay > NOISE_Y && fay > yMax) {
//        yMax = fay;
//    } else if (fay < -NOISE_Y && fay < yMax) {
//        yMax = fay;
//    } else if (fay < NOISE_Y && fay > -NOISE_Y) {
//        if(yMax > NOISE_Y || yMax < -NOISE_Y) {
//           //Serial.print("yMAX: ");
//           //Serial.println(yMax);
//           delay(10);
//        }
//        yMax = 0;
//        Serial.println(yMax);
//    }
//}
