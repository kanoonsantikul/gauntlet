#include "accelFirmware.h"

int16_t ax, ay, az;
int16_t axOff, ayOff, azOff;
float fax, fay, faz;

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

//    Serial.print(fax); Serial.print("\t");
//    Serial.print(fay); Serial.print("\t");
//    Serial.println(faz);

    //graph
    Serial.println(fax);
    delay(25);
}
