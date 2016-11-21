#include "MPU6050.h"

#define SENSITIVITY 16384.0

void initAccel ();
void getOffset (float *axOff, float *ayOff, float *azOff);
void getAccel (uint16_t *ax, uint16_t *ay, uint16_t *az);
