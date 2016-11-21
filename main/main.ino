#include "accelFirmware.h"

uint16_t ax;
uint16_t ay;
uint16_t az;

float axOff;
float ayOff; 
float azOff;

float fax;
float fay;
float faz;

void setup () {
    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    initAccel();
    getOffset (&axOff, &ayOff, &azOff);
}

void loop () {
    getAccel(&ax, &ay, &az);

    fax = (ax / SENSITIVITY - axOff);
    fay = (ay / SENSITIVITY - ayOff);
    faz = (az / SENSITIVITY - azOff - 1);

    Serial.println(fax); Serial.print("\t");
    //Serial.println(ay); Serial.print("\t");
    //Serial.println(faz);

    delay(100);
}
