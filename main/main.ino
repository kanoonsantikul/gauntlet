#include "accelFirmware.h"

<<<<<<< HEAD
uint16_t ax;
uint16_t ay;
uint16_t az;

float axOff;
float ayOff; 
float azOff;

float fax;
float fay;
float faz;
=======
int16_t ax, ay, az;
int16_t axOff, ayOff, azOff;
float fax, fay, faz;
>>>>>>> 7a6d7373a2cf6b3bdca70135c89a2d7cf63ca39b

void setup () {
    Serial.begin(38400);
    initAccel();
<<<<<<< HEAD
    getOffset (&axOff, &ayOff, &azOff);
=======
    caribrate(&axOff, &ayOff, &azOff);
>>>>>>> 7a6d7373a2cf6b3bdca70135c89a2d7cf63ca39b
}

void loop () {
    getAccel(&ax, &ay, &az);

<<<<<<< HEAD
    fax = (ax / SENSITIVITY - axOff);
    fay = (ay / SENSITIVITY - ayOff);
    faz = (az / SENSITIVITY - azOff - 1);

    Serial.println(fax); Serial.print("\t");
    //Serial.println(ay); Serial.print("\t");
    //Serial.println(faz);

    delay(100);
=======
    fax = (ax - axOff) / SENSITIVITY;
    fay = (ay - ayOff) / SENSITIVITY;
    faz = (az - azOff - SENSITIVITY) / SENSITIVITY;

//    Serial.print(fax); Serial.print("\t");
//    Serial.print(fay); Serial.print("\t");
//    Serial.println(faz);

    //graph
    Serial.println(fax);
    delay(25);
>>>>>>> 7a6d7373a2cf6b3bdca70135c89a2d7cf63ca39b
}
