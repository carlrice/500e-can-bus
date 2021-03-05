#include <FlexCAN_T4.h>

/*
 * Target: Fiat 500e 2013-2019, 360° camera system with active trajectory lines (configured for a BMW X1)
 * Use case: Expose 500e steering wheel angle to an external accessory that doesn't support Fiat
 * Hardware: Teensy 4.0, 2x Waveshare SN65HVD230 CAN trancievers, 12v to 5v buck converter, OBD2 pig tail
 * Instructions:
 *   1. Remove the 120Ω resistor from your CAN1 transciever (it's labeled 121 just before CANH/L headers)
 *   2. Wire and flash the Teensy with this script.
 *   3. Create an OBD2 pigtail that connects CAN1 H/L to OBD2 1/9 and connect CAN2 H/L to the external accessory
 *   4. Ensure 5v power is coming from your vehicle - a common ground was required.
 *         - You will need a USB data only if you plan on debugging while powered by the vehicle
 *         - Cut the USB power trace or use an adapter or modified data only USB cable
 */


FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
CAN_message_t msg;
CAN_message_t newMsg;

int ledPin = 13;

void setup(void) {
  // start up
  delay(1000);
  Serial.print("\nStarting..."); 
  pinMode(ledPin, OUTPUT);
  ledOff();
  
  // Fiat side - look at CAN-B (OBD2 pins 1/9) for SWA only
  can1.begin();
  can1.setBaudRate(50000);
  can1.enableFIFO();
  can1.enhanceFilter(FIFO);
  can1.setFIFOFilter(REJECT_ALL);
  can1.setFIFOFilter(0, 0x6264000, EXT);
  
  // 360° side (which is configured to look for BMW X1 messages)
  can2.begin();
  can2.setBaudRate(100000);
}

void loop() {
  translateData();
}

/*
 * Using live CAN data, find and convert Fiat SWA messages to BMW SWA. Call from `loop`.
 */
void translateData() {
  delay(10);
  if (can1.read(msg)) {
    ledOn();
    Serial.print("\nFound: "); 
    printMessage(msg);
     
    newMsg = convertToBMW(msg);
    Serial.print("  -> Sending: "); printMessage(newMsg);
    can2.write(newMsg);
  } else {
    ledOff();
  }
}

/*
 * Performs the conversion. Fiat has a +/- 540° range, but the BMW only +/- 500° 
 * so this is an approximate trajectory (like all active lines.)
 */
CAN_message_t convertToBMW(CAN_message_t msg) {
  // extract swa and adjust range
  float fiatDeg = ((msg.buf[1] * 256) + msg.buf[2])/10.0-720; // -540° to 540°
  float bmwDeg = (fiatDeg / 540.0) * 500.0; // -500° to 500°
  
  Serial.print("\n  -> Fiat Degrees:"); Serial.print(fiatDeg);
  Serial.print(" -> BMW Degrees:"); Serial.print(bmwDeg); 
  Serial.print(" -> ");
  
  // convert to BMW CAN message data
  int bmwDegDec = round(bmwDeg * 23.0);
  if (bmwDeg < 0) {
    bmwDegDec = 0xFFFF + bmwDegDec; 
  }
  Serial.print("BMW DEC:"); Serial.print(bmwDegDec); Serial.print("\n");
  
  // make new message
  newMsg.id = 0x0C4;
  newMsg.flags.extended = false;
  newMsg.len = 6;
  newMsg.buf[0] = bmwDegDec & 0xff;
  newMsg.buf[1] = bmwDegDec >> 8;
  newMsg.buf[2] = 0xFC;
  newMsg.buf[3] = 0x00;
  newMsg.buf[4] = 0x00;
  newMsg.buf[5] = 0xFF;
  return newMsg;
}

void ledOff() {
  digitalWrite(ledPin, LOW);
}

void ledOn() {
  digitalWrite(ledPin, HIGH);
}

void printMessage(CAN_message_t msg) {
  Serial.print("MB: "); Serial.print(msg.mb);
  Serial.print("  ID: 0x"); Serial.print(msg.id, HEX );
  Serial.print("  EXT: "); Serial.print(msg.flags.extended );
  Serial.print("  LEN: "); Serial.print(msg.len);
  Serial.print(" DATA: ");
  for ( uint8_t i = 0; i < 8; i++ ) {
    Serial.print(msg.buf[i]); Serial.print(" ");
  }
  Serial.print("  TS: "); Serial.println(msg.timestamp);
}

/// Testing ///

/*
 * Sends fake Fiat SWA messages for testing purposes. Call from `loop`.
 */
void fakeData() {
  // send fake message
  delay(10);
  newMsg = convertToBMW(newFakeFiatMessage());
  Serial.print("  -> Sending: "); printMessage(newMsg);
  can2.write(newMsg);
}

int curVal = 0x0708;
int minVal = 0x0708;
int maxVal = 0x3138;
int incr = 10;

/*
 * Generates a message like the Fiat would send, incrementing back and forth between
 * the min/max range.
 */
CAN_message_t newFakeFiatMessage() {
  int a = 0; int b = 0;
  if (curVal > 0xFF) {
    a = curVal >> 8;
    b = curVal & 0xff; 
  }else{
    a = 0x00;
    b = curVal;
  }
  Serial.print("\n0x"); Serial.print(a, HEX);
  Serial.print(" 0x"); Serial.print(b, HEX);
  if (curVal > maxVal || curVal < minVal) incr = incr*-1;
  curVal+=incr;


  msg.id = 0x6264000;
  msg.flags.extended = true;
  msg.len = 3;
  msg.buf[0] = 0x0;
  msg.buf[1] = a;
  msg.buf[2] = b;
  
  return msg;
}
