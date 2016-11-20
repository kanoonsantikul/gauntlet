#include "accelFirmware.h"

int16_t *ax, *ay, *az;
int16_t *axOff, *ayOff, *azOff;

void setup () {
    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    initAccel();
    Serial.println("caribating");
    getOffset (axOff, ayOff, azOff);
}

void loop () {
    getAccel(ax, ay, az);

    static float fax;
    static float fay;
    static float faz;

    fax = (*ax - *axOff) / SENSITIVITY;
    fay = (*ay - *ayOff) / SENSITIVITY;
    faz = (*az - *azOff - SENSITIVITY) / SENSITIVITY;

    Serial.print(fax); Serial.print("\t");
    Serial.print(fay); Serial.print("\t");
    Serial.println(faz);
}
