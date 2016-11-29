#define SENSITIVITY 16384.0000

void initAccel ();
void getAccel (int16_t *ax, int16_t *ay, int16_t *az);
void caribrate (float *axOff, float *ayOff, float *azOff, uint16_t numberOffTest);
