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
float realAx, realAy, realAz;

float Vx0, Vy0, Vx, Vy;
int8_t previousMs;
int8_t deltaMs;

uint8_t xState = 0;
uint8_t yState = 0;

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
  static float accelNoise = 0.02;
  static float velocityNeighborRange = 0.7;
  static uint8_t zeroCountX = 0;
  static uint8_t zeroCountY = 0;
  PT_BEGIN(pt);
    for (;;) {
      //find delta time
        deltaMs = millis() - previousMs;
        previousMs = millis();
      
        getAccel(&rawAx, &rawAy, &rawAz);
  
        realAx = ((float)rawAx - axOff) / SENSITIVITY;
        realAy = ((float)rawAy - ayOff) / SENSITIVITY;
        realAz = ((float)rawAz - azOff - SENSITIVITY) / SENSITIVITY;
  
        //find velocity
        Vx = Vx0 + (realAx * (float)deltaMs);
        Vy = Vy0 + (realAy * (float)deltaMs);

        // set V = 0 if have noise
        if(absolute(realAx) <= accelNoise) {
          Vx = 0;
        }
        if(absolute(realAy) <= accelNoise) {
          Vy = 0;
        }

        if (Vx != 0) {
          zeroCountX = 0;
        } else {
          zeroCountX++;
        }
        if (Vy != 0) {
          zeroCountY = 0;
        } else {
          zeroCountY++;
        }

        if (zeroCountX > 10) {
          Vx0 = 0;
        }
        if (zeroCountY > 10) {
          Vy0 = 0;
        }
        
        //set new V0 = V in possible range
        if(isNeighbor(Vx0, Vx, velocityNeighborRange)) {
         Vx0 = Vx;
        }
        if(isNeighbor(Vy0, Vy, velocityNeighborRange)) {
         Vy0 = Vy;
        }
        
        //graph
        //Serial.print(deltaMs);
        // Serial.print(",");
        Serial.print((int8_t)Vx0);
        Serial.print(",");
        Serial.println((int8_t)Vy0);
        //Serial.print(",");
        //Serial.println(realAz);
  
        PT_DELAY(pt,1,ts);
    }
  PT_END(pt);
}

PT_THREAD(mouse_task(struct pt *pt)) {
  PT_BEGIN(pt); 
    for (;;) {
      PT_WAIT_UNTIL(pt,usbInterruptIsReady());
      sendMouse((int8_t)Vx0, -(int8_t)Vy0, 0);
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
  deltaMs = 0;

  Vx0 = 0;
  Vy0 = 0;

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

