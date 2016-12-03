#include <pt.h>
#include <usbdrv.h>
#include "keycodes.h"
#include "accelFirmware.h"
#include <math.h>

#define REPORT_ID_MOUSE 2

#define PT_DELAY(pt,ms,tsVar) \
  tsVar = millis(); \
  PT_WAIT_UNTIL(pt, millis()-tsVar >= (ms));

int16_t rawAx, rawAy, rawAz;

float axOff, ayOff, azOff;
float Ax, Ay, Az;
float AxMax = 0;
float AyMax = 0;
float zxcount = 0;
float zycount = 0;

int8_t previousMs;
int8_t t;

static uint32_t ts = 0;

float absolute(float f);
bool isNeighbor(float a, float b, float d);

PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
  ////////////////////////////////////
  // โครงสร้าง HID report สำหรับคียบอร์ด
  ////////////////////////////////////
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
  0x09, 0x06,                    // USAGE (Keyboard)
  0xa1, 0x01,                    // COLLECTION (Application)
  0x85, 0x01,                    //   REPORT_ID (1)
  0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
  0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
  0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
  0x75, 0x01,                    //   REPORT_SIZE (1)
  0x95, 0x08,                    //   REPORT_COUNT (8)
  0x81, 0x02,                    //   INPUT (Data,Var,Abs)
  0x95, 0x01,                    //   REPORT_COUNT (1)
  0x75, 0x08,                    //   REPORT_SIZE (8)
  0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
  0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
  0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
  0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
  0xc0,                          // END_COLLECTION
  //////////////////////////////////////
  // โครงสร้าง HID report สำหรับเมาส์ 3 ปุ่ม
  //////////////////////////////////////
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
  0x0b, 0x02, 0x00, 0x01, 0x00,  // USAGE (Generic Desktop:Mouse)
  0xa1, 0x01,                    // COLLECTION (Application)
  0x0b, 0x01, 0x00, 0x01, 0x00,  //   USAGE (Generic Desktop:Pointer)
  0xa1, 0x00,                    //   COLLECTION (Physical)
  0x85, 0x02,                    //     REPORT_ID (2)
  0x05, 0x09,                    //     USAGE_PAGE (Button)
  0x1b, 0x01, 0x00, 0x09, 0x00,  //     USAGE_MINIMUM (Button:Button 1)
  0x2b, 0x03, 0x00, 0x09, 0x00,  //     USAGE_MAXIMUM (Button:Button 3)
  0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
  0x75, 0x01,                    //     REPORT_SIZE (1)
  0x95, 0x03,                    //     REPORT_COUNT (3)
  0x81, 0x02,                    //     INPUT (Data,Var,Abs)
  0x75, 0x05,                    //     REPORT_SIZE (5)
  0x95, 0x01,                    //     REPORT_COUNT (1)
  0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
  0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
  0x0b, 0x30, 0x00, 0x01, 0x00,  //     USAGE (Generic Desktop:X)
  0x0b, 0x31, 0x00, 0x01, 0x00,  //     USAGE (Generic Desktop:Y)
  0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
  0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
  0x75, 0x08,                    //     REPORT_SIZE (8)
  0x95, 0x02,                    //     REPORT_COUNT (2)
  0x81, 0x06,                    //     INPUT (Data,Var,Rel)
  0xc0,                          //     END_COLLECTION
  0xc0,                          // END_COLLECTION
  ////////////////////////////////////////////////////////////
  // โครงสร้าง HID report สำหรับเกมแพดแบบหนึ่งปุ่มกดและหนึ่งก้านแอนะล็อก
  ////////////////////////////////////////////////////////////
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
  0x09, 0x04,                    // USAGE (Joystick)
  0xa1, 0x01,                    // COLLECTION (Application)
  0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
  0x09, 0x01,                    //   USAGE (Pointer)
  0xa1, 0x00,                    //   COLLECTION (Physical)
  0x85, 0x03,                    //     REPORT_ID (3)
  0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
  0x09, 0x32,                    //     USAGE (Z)
  0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
  0x26, 0xff, 0x03,              //     LOGICAL_MAXIMUM (1023)
  0x75, 0x0a,                    //     REPORT_SIZE (10)
  0x95, 0x01,                    //     REPORT_COUNT (1)
  0x81, 0x02,                    //     INPUT (Data,Var,Abs)
  0x05, 0x09,                    //     USAGE_PAGE (Button)
  0x09, 0x01,                    //     USAGE (Button 1)
  0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
  0x75, 0x01,                    //     REPORT_SIZE (1)
  0x95, 0x01,                    //     REPORT_COUNT (1)
  0x81, 0x02,                    //     INPUT (Data,Var,Abs)
  0x75, 0x01,                    //     REPORT_SIZE (1)
  0x95, 0x05,                    //     REPORT_COUNT (5)
  0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
  0xc0,                          //   END_COLLECTION
  0xc0                           // END_COLLECTION
};

struct ReportMouse {
  uint8_t  report_id;
  uint8_t  buttons;
  int8_t   dx;
  int8_t   dy;
};

ReportMouse reportMouse;

// Protothread states
struct pt main_pt;
struct pt mouse_pt;
struct pt compute_pt;

////////////////////////////////////////////////////////////////
// Automatically called by usbpoll() when host makes a request
////////////////////////////////////////////////////////////////
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
  return 0;  /* Return nothing to host for now */
}

void sendMouse(int8_t dx, int8_t dy, uint8_t buttons) {
  reportMouse.dx = dx;
  reportMouse.dy = dy;
  reportMouse.buttons = buttons;
  usbSetInterrupt((uchar*)&reportMouse, sizeof(reportMouse));
}

PT_THREAD(compute_task(struct pt *pt)) {
  static float ZERO_NOISE = 3.5;
  static float ACCEL_NOISE = 25;
  static float ZERO = 35;
  static float LOW_SPEED = 10;
  static float HEIGHT_SPEED = 15; 
  PT_BEGIN(pt);
    for (;;) {
      //find delta time
        t = millis() - previousMs;
        previousMs = millis();
      
        getAccel(&rawAx, &rawAy, &rawAz);

        Ax = ((float)rawAx - axOff) / SENSITIVITY;
        Ay = ((float)rawAy - ayOff) / SENSITIVITY;
        Az = ((float)rawAz - azOff - SENSITIVITY) / SENSITIVITY;

        Ax *= 50;
        if (Ax <= ZERO_NOISE && Ax >= -ZERO_NOISE) {
          Ax = 0;          
          
          zxcount++;
          if(zxcount > ZERO) {
            zxcount = 0;
            AxMax = 0;
          }
        } else if (Ax <= ACCEL_NOISE && Ax >= -ACCEL_NOISE) {
          zxcount = 0;
        }

        if(AxMax == 0) {
            if(Ax > 0) {
              if(Ax < LOW_SPEED) {
                Ax = LOW_SPEED;
              } else if(Ax > HEIGHT_SPEED) {
                Ax = HEIGHT_SPEED;
              }
            } else if(Ax < 0) {
              if(Ax > -LOW_SPEED) {
                Ax = -LOW_SPEED;
              } else if(Ax < -HEIGHT_SPEED) {
                Ax = -HEIGHT_SPEED;
              }
            }
            AxMax = Ax;
        }
        //////////////////////////////////////////
        Ay *= 50;
        if (Ay <= ZERO_NOISE && Ay >= -ZERO_NOISE) {
          Ay = 0;          
          
          zycount++;
          if(zycount > ZERO) {
            zycount = 0;
            AyMax = 0;
          }
        } else if (Ay <= ACCEL_NOISE && Ay >= -ACCEL_NOISE) {
          zycount = 0;
        }

        if(AyMax == 0) {
            if(Ay > 0) {
              if(Ay < LOW_SPEED) {
                Ay = LOW_SPEED;
              } else if(Ay > HEIGHT_SPEED) {
                Ay = HEIGHT_SPEED;
              }
            } else if(Ay < 0) {
              if(Ay > -LOW_SPEED) {
                Ay = -LOW_SPEED;
              } else if(Ay < -HEIGHT_SPEED) {
                Ay = -HEIGHT_SPEED;
              }
            }
            AyMax = Ay;
        }

        Serial.print(AyMax);
        Serial.print(",");
        Serial.println(Ay);
  
        PT_DELAY(pt,1,ts);
    }
  PT_END(pt);
}

PT_THREAD(mouse_task(struct pt *pt)) {
  PT_BEGIN(pt); 
    for (;;) {
      PT_WAIT_UNTIL(pt,usbInterruptIsReady());
      sendMouse(AxMax, -AyMax, 0);
      PT_DELAY(pt,1,ts);
    }
  PT_END(pt);
}

PT_THREAD(main_task(struct pt *pt)) {
  PT_BEGIN(pt);
    for (;;) {
      mouse_task(&mouse_pt);
      compute_task(&compute_pt);
      PT_DELAY(pt,1,ts);
    }
  PT_END(pt);
}

void setup() {
  previousMs = millis();
  t = 0;
  
  pinMode(PIN_PD3, OUTPUT);
  Serial.begin(115200);
   
  initAccel();
  caribrate(&axOff, &ayOff, &azOff, 200);
  
  // Initialize USB subsystem
  usbInit();
  usbDeviceDisconnect();
  delay(300);
  usbDeviceConnect();
  
  // Initialize USB reports
  reportMouse.report_id = REPORT_ID_MOUSE;
  reportMouse.dx = 0;
  reportMouse.dy = 0;
  reportMouse.buttons = 0;

  // Initialize tasks
  PT_INIT(&main_pt);
  PT_INIT(&mouse_pt);
  PT_INIT(&compute_pt);
}

void loop() {
  PORTD ^= (1<<PD3);  // ใช้ตัวดำเนินการ XOR เพื่อสลับลอจิกของขา
  
  usbPoll();
  main_task(&main_pt);
}

bool isNeighbor(float a, float b, float d) {
  if (a > b) {
    return a - b <= d;
  } else {
    return b - a <= d;
  }
}

float absolute(float f) {
  if (f > 0) {
    return f;
  } else if (f < 0) {
    return -f;
  } else {
    return 0;
  }
}

