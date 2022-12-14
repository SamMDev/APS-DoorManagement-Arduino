#include <MFRC522.h>
#include <deprecated.h>
#include <require_cpp11.h>
#include <MFRC522Extended.h>

#include <Servo.h>

#define THIS_GATE_CODE "test"
#define RST_PIN         8          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

Servo servo;
MFRC522 mfrc522(SS_PIN, RST_PIN);

const int buzzerPin = 8;
const int refuseRedLedPin = 7;
const int approveGreenLedPin = 6;

const int servoOpenAngle = 90;
const int servoCloseAngle = 0;

int readBooleanFromSerial();
void openLock();
void closeLock();
void letPersonPass();
void rejectPersonPass();
void lightApproveLight();
void turnOffApproveLight();
void makeApproveSound();
void makeRejectSound();
void turnOffBuzzer();
void array_to_string(byte array[], unsigned int len, char buffer[]);


void setup() {
  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT);
  pinMode(refuseRedLedPin, OUTPUT);
  pinMode(approveGreenLedPin, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();
  
  servo.attach(3);
  servo.write(0);
  delay(2000); // wait for servo to reposition
}



void loop() {

  // not doing anything until card is detected
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if ( ! mfrc522.PICC_ReadCardSerial()) return;

  char str[32] = "";
  array_to_string(mfrc522.uid.uidByte, 4, str);

  
  Serial.println(String(str) + " " + THIS_GATE_CODE);
  delay(500); // wait for response

  // read response
  if (Serial.available() > 0) {
    int canPass = readBooleanFromSerial();
    
    if (canPass > 0) letPersonPass();
    else rejectPersonPass();
  }

  delay(5000);
  
}

int readBooleanFromSerial() {
  if (Serial.available() == 0) return 0;

  char value = Serial.read();
  while (Serial.available() > 0) Serial.read();

  return value > '0';
}

void openLock() {
  servo.write(servoOpenAngle);
}

void closeLock() {
  servo.write(servoCloseAngle);
}

void makeApproveSound() {
   tone(buzzerPin, 1000);
}

void makeRejectSound() {
  tone(buzzerPin, 500);
}

void lightApproveLight() {
  digitalWrite(approveGreenLedPin, HIGH);
}

void turnOffApproveLight() {
  digitalWrite(approveGreenLedPin, LOW);
}


void lightRejectLight() {
  digitalWrite(refuseRedLedPin, HIGH);
}

void turnOffRejectLight() {
  digitalWrite(refuseRedLedPin, LOW);
}

void turnOffBuzzer() {
  noTone(buzzerPin);
}

void letPersonPass() {
  // buzzer make approve sound
  makeApproveSound();
  lightApproveLight();
  
  openLock();
  delay(3000);
  closeLock();

  turnOffBuzzer();
  turnOffApproveLight();
}

void rejectPersonPass() {
  makeRejectSound();
  lightRejectLight();

  delay(1000);

  turnOffBuzzer();
  turnOffRejectLight();
}


void array_to_string(byte arr[], unsigned int len, char buffer[]) {
   for (unsigned int i = 0; i < len; i++) {
      byte nib1 = (arr[i] >> 4) & 0x0F;
      byte nib2 = (arr[i] >> 0) & 0x0F;
      buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
      buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
   }
   buffer[len*2] = '\0';
}

