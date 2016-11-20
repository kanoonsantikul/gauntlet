#define SENSITIVITY 16384

void initAccel ();
void getOffset (uint16_t *axOff, uint16_t *ayOff, uint16_t *azOff);
void getAccel (uint16_t *ax, uint16_t *ay, uint16_t *az);
