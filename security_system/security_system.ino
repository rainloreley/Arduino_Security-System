#include <IRremote.h>
#include <NewTone.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
 
#define SS_PIN 10
#define RST_PIN 9

#include <SoftwareSerial.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal_I2C Display(0x27, 16, 2);

int movementStatus = 0;
int bluetooth = 0;
int systemActive = 0;

//Pins
int movementSensor = 8;
int statusLED = 3;
int IR_PIN = 4;
int speaker = 5;
int backlightActive = 1;

// Card UID: 24 22 71 A7


String enteredPassword = "1";
unsigned int entPasswordLength = enteredPassword.length();

IRrecv irDetect(IR_PIN);

decode_results irIn;

void setup() {

  // Blynk will work through Serial
  // Do not read or write this serial manually in your sketch
  Serial.begin(9600);

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  Display.init();
  Display.begin(20, 2);
  Display.backlight();
  backlightActive = 1;
  
  pinMode(movementSensor, INPUT);
  pinMode(statusLED, OUTPUT);
  pinMode(speaker, OUTPUT);

  irDetect.enableIRIn();
}

void loop() {

  if (systemActive == 1) {
    digitalWrite(statusLED, HIGH);
    Display.clear();
    Display.print("Active");
    Serial.println("Check enabled");
    movementStatus = digitalRead(movementSensor);
    if (movementStatus == HIGH) {
      while (systemActive == 1) {
        Serial.println("alarm");
        Display.clear();
        Display.print("Alarm!");
        alertSound();
        noNewTone(speaker);
        checkAlert();
      }
    }
    //Serial.println(movementStatus);
  }
  else if (systemActive == 0) {
    digitalWrite(statusLED, LOW);
    Serial.println("Check disabled");
    Display.clear();
    Display.print("Not active");
  }

  if (backlightActive == 1) {
      Display.backlight();
    }
    else if (backlightActive == 0) {
      Display.noBacklight();
    }
    
  checkAlert();
}

void alertSound() {

 for (int i=50; i<1000; i=i+1) {
  if (systemActive == 1) {
    NewTone(speaker, i);
    delay(1);
  }
  } 

  checkAlert();
  for (int y=1000; y>50; y=y-1) {
    if (systemActive == 1) {
      NewTone(speaker, y);
    delay(1);
    }
  }
}

void checkAlert() {
  
// Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "24 22 71 A7") //change here the UID of the card/cards that you want to give access
  {
    Serial.print("spy");
    if (backlightActive == 1) {
      Display.noBacklight();
      backlightActive = 0;
      delay(1000);
    }
    else if (backlightActive == 0) {
      Display.backlight();
      backlightActive = 1;
      delay(1000);
    }
  }
  else if (content.substring(1) == "33 5E 14 DB") {
    rfidAccess();
  }
  else if (content.substring(1) == "92 9F 50 8C") {
    rfidAccess();
  }
 
 else   {
    Serial.println(" Access denied");
    delay(3000);
  }
 
/*  //Get Remote Value
if (irDetect.decode(&irIn)) {
  String hexvalue = String(irIn.value, HEX);
  //(irIn.value, HEX);
    Serial.println(hexvalue);
    if (hexvalue == "ff22dd" || hexvalue == "52a3d41f") { //Left: System active
      systemActive = 1;
    }
    if (hexvalue == "ffc23d" || hexvalue == "20fe4dbb") { //Right: System not active
      systemActive = 0;
      noNewTone(speaker);
    }
    irDetect.resume(); // Receive the next value
}*/
}

void rfidAccess() {
  
  Serial.println("Authorized access");
    if (systemActive == 1) {
      noNewTone(speaker);
      Serial.println("disabled");
      Display.clear();
      Display.println("Remove your tag");
      delay(500);
      NewTone(speaker, 349);
      delay(200);
      NewTone(speaker, 311);
      delay(200);
      NewTone(speaker, 277);
      delay(200);
      noNewTone();
      Display.clear();
      for (int z=1; z<21; z=z+1) {
        Display.print("-");
      delay(250);
      }
      NewTone(speaker, 277);
      delay(200);
      noNewTone();
      Display.clear();
      systemActive = 0;
    }
    else if (systemActive == 0) {
      Serial.println("enabled");
      Display.clear();
      Display.println("Remove your tag");
      delay(500);
      NewTone(speaker, 277);
      delay(200);
      NewTone(speaker, 311);
      delay(200);
      NewTone(speaker, 349);
      delay(200);
      noNewTone();
      Display.clear();
      for (int z=1; z<21; z=z+1) {
        Display.print("-");
      delay(250);
      }
      Display.clear();
      NewTone(speaker, 349);
      delay(200);
      noNewTone();
      systemActive = 1;
    }
}

