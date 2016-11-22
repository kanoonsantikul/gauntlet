#include <pt.h>
#include <usbdrv.h>
#include "keycodes.h"

#define REPORT_ID_MOUSE 2

// มาโครสำหรับจำลองการหน่วงเวลาใน protothread
#define PT_DELAY(pt,ms,tsVar) \
  tsVar = millis(); \
  PT_WAIT_UNTIL(pt, millis()-tsVar >= (ms));

#include "accelFirmware.h"

int16_t ax, ay, az;
int16_t axOff, ayOff, azOff;
uint8_t fax, fay, faz;
uint8_t Vx0, Vy0, Vx, Vy;
uint32_t previousMs;
uint32_t deltaMs;

/////////////////////////////////////////////////////////////////////
// USB report descriptor, สร้างขึ้นจาก HID Descriptor Tool ซึ่ง
// ดาวน์โหลดได้จาก
//    http://www.usb.org/developers/hidpage/dt2_4.zip
//
// หรือใช้ HIDEdit ซึ่งให้บริการฟรีผ่านเว็บที่ http://hidedit.org/
//
// *** ไม่แนะนำให้สร้างเองด้วยมือ ***
/////////////////////////////////////////////////////////////////////
PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = 
{
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

struct ReportMouse
{
  /* +----\------+-----+-----+-----+-----+-----+-----+-----+-----+
   * |Byte \ Bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
   * +------\----+-----+-----+-----+-----+-----+-----+-----+-----+
   * |  0        |               Report ID = 2                   |
   * +-----------+-----+-----+-----+-----+-----+-----+-----+-----+
   * |  1        |              Buttons' statuses                |
   * +-----------+-----+-----+-----+-----+-----+-----+-----+-----+
   * |  2        |                  Delta X                      |
   * +-----------+-----+-----+-----+-----+-----+-----+-----+-----+
   * |  3        |                  Delta Y                      |
   * +-----------+-----+-----+-----+-----+-----+-----+-----+-----+
   */
  uint8_t  report_id;
  uint8_t  buttons;
  int8_t   dx;
  int8_t   dy;
};

ReportMouse reportMouse;

uint16_t light;  // Updated by Light-Task; to be shared among threads

// Protothread states
struct pt main_pt;
struct pt mouse_pt;

////////////////////////////////////////////////////////////////
// Automatically called by usbpoll() when host makes a request
////////////////////////////////////////////////////////////////
usbMsgLen_t usbFunctionSetup(uchar data[8])
{
  return 0;  /* Return nothing to host for now */
}

//////////////////////////////////////////////////////////////
void sendMouse(int8_t dx, int8_t dy, uint8_t buttons)
{
  reportMouse.dx = dx;
  reportMouse.dy = dy;
  reportMouse.buttons = buttons;
  usbSetInterrupt((uchar*)&reportMouse, sizeof(reportMouse));
}

//////////////////////////////////////////////////////////////
PT_THREAD(mouse_task(struct pt *pt))
{
  static uint32_t ts = 0;

  PT_BEGIN(pt);

  for (;;)
  {
    //find delta time
    deltaMs = millis() - previousMs;
    previousMs = millis();
    
    getAccel(&ax, &ay, &az);
    

     //test acceleration
//    ax = 1;
//    ay = 1;

    fax = (ax - axOff) / SENSITIVITY;
    fay = (ay - ayOff) / SENSITIVITY;
    faz = (az - azOff - SENSITIVITY) / SENSITIVITY;

//    if(fax < 0.5 && fax > -0.5) {
//      fax = 0;
//    }
//    if(fay < 0.5 && fay > -0.5) {
//      fay = 0;
//    }

    //find velocity
    Vx = Vx0 + (fax * deltaMs);
    Vy = Vy0 + (fay * deltaMs);

    //set new V0
    Vx0 = Vx;
    Vy0 = Vy;

    //graph
    Serial.print("ax : ");
    Serial.print(ax);
    Serial.print("       ay : ");
    Serial.println(ay);e
       
    sendMouse(-Vx, Vy, 0);
    PT_DELAY(pt,100,ts);
  }
  
  PT_END(pt);
}

//////////////////////////////////////////////////////////////
PT_THREAD(main_task(struct pt *pt))
{
  PT_BEGIN(pt);

  PT_WAIT_UNTIL(pt,usbInterruptIsReady());
  mouse_task(&mouse_pt);

  PT_END(pt);
}

//////////////////////////////////////////////////////////////
void setup()
{
  previousMs = millis();
  deltaMs = 0;

  Vx0 = 0;
  Vy0 = 0;

  pinMode(PIN_PD3, OUTPUT);
  Serial.begin(9600);
  
  //Serial.begin(38400);
  
  initAccel();
  caribrate(&axOff, &ayOff, &azOff);
  
  // Initialize USB subsystem
  usbInit();
  usbDeviceDisconnect();
  delay(300);
  usbDeviceConnect();
  
  //init_peripheral();
  
  // Initialize USB reports
  reportMouse.report_id = REPORT_ID_MOUSE;
  reportMouse.dx = 0;
  reportMouse.dy = 0;
  reportMouse.buttons = 0;

  // Initialize tasks
  PT_INIT(&main_pt);
  PT_INIT(&mouse_pt);
}

//////////////////////////////////////////////////////////////
void loop()
{

 // Serial.println("Hello, Serial");
  PORTD ^= (1<<PD3);  // ใช้ตัวดำเนินการ XOR เพื่อสลับลอจิกของขา
  delay(5);
  
  usbPoll();
  main_task(&main_pt);
}

