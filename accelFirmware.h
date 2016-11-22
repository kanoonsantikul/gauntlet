#define SENSITIVITY 16384.0

void initAccel ();
void getAccel (uint16_t *ax, uint16_t *ay, uint16_t *az);
void caribrate (uint16_t *axOff, uint16_t *ayOff, uint16_t *azOff);
