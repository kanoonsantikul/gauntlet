<<<<<<< HEAD
#include "MPU6050.h"

#define SENSITIVITY 16384.0

void initAccel ();
void getOffset (float *axOff, float *ayOff, float *azOff);
=======
#define SENSITIVITY 16384.0

void initAccel ();
>>>>>>> 7a6d7373a2cf6b3bdca70135c89a2d7cf63ca39b
void getAccel (uint16_t *ax, uint16_t *ay, uint16_t *az);
void caribrate (uint16_t *axOff, uint16_t *ayOff, uint16_t *azOff);
