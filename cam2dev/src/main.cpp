#include <Arduino.h>

// includes for environment
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_ADS1X15.h>

// includes for two readers wiegand and system(protothreads)
#include <trw.h>
#include <pt.h>

// i2c peripheral includes
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

// includes for servers and wifi connections
#include <WiFi.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Preferences.h>


#include <page1.h>
#include <page2.h>
#include <page3.h>
#include <style.h>

// access point dredentials
const char *ssid = "SECUREPASS";
const char *password = "57832023";

//********************************
//********************************
//****Defines*********************
//********************************

// define  heart beat
#define LED_BH 25
// define  buzzer
#define BUZZ 27
// define  relay
#define REL 26
// define  wifi (blink=conected , on = access póint mode)
#define LED_WIFI 33
// define  blue tooth
#define LED_BLE 32
// define   magnetic reed switch
#define LED_S1 4
// define  request to exit
#define LED_S2 5

#define MAX_DEVICES 32

//*********************************
//*********STRUCS*************
//*********************************
struct I2CDeviceInfo
{
  byte address;
  int type;
  int order;
};
//*********************************

//**********************************
//****global variables**************
//**********************************

I2CDeviceInfo deviceInfo[MAX_DEVICES];          // configuracion de dispositivos i2c.
I2CDeviceInfo recoveredDeviceINfo[MAX_DEVICES]; // configuracion recuperada.
byte deviceCount = 0;

bool lcdDefautl = false;
bool rtcDefault = false;
bool mem0Default = false;

//*********************************
//**************const**************
//*********************************

const char *tabletypes[] = {
    "LCD",
    "RTC-13-32",
    "MEM",
    "KBD",
    "GPIO-PCF",
    "GPIO-PCFA",
    "MAG-5803",
    "GYRO-6500-09250",
    "BME280",
    "DISP-SSD1306",
    "A2D-AD1115",
};

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//********************************
//*****Objet Declarations*******
//********************************

// Display i2c Instance
LiquidCrystal_I2C lcd(0x3f, 16, 2);
RTC_DS1307 rtc;
WIEGAND wg;

// put function declarations here:

void scanI2CAndSaveData();
void configRecovery();
void storeI2cconfig();
void autoConfig();

void setup()
{
  // put your setup code here, to run once:

  if (!SPIFFS.begin(true))
  {
    Serial.println("Error al montar el sistema de archivos SPIFFS.");
    return;
  }
  // scanneri2c();
  Serial.begin(115200);
  Serial.println("Serial port open 115200");
  Serial.println("i2c Iniciated");
  Wire.begin();
  Serial.println("Scanning I2C Addreses");
  scanI2CAndSaveData();
  Serial.println("Scanning I2C ended ");
  configRecovery();
  autoConfig();
  storeI2cconfig();
  configRecovery();

  if (lcdDefautl)
  {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    delay(1000);
    lcd.print("Display OK");
    delay(1000);
    lcd.setCursor(0, 1);
  }

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      delay(10);
  }

  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  lcd.print("RTC OK");

  // inputs optputs and signaling configuration
  pinMode(LED_BH, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(REL, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_BLE, OUTPUT);
  pinMode(LED_S1, INPUT);
  pinMode(LED_S2, INPUT);
  // testing of outputs
  digitalWrite(LED_BH, HIGH);
  digitalWrite(BUZZ, HIGH);
  digitalWrite(REL, HIGH);
  digitalWrite(LED_WIFI, HIGH);
  digitalWrite(LED_BLE, HIGH);
  // waiting (block task)
  delay(500);
  // end test
  digitalWrite(BUZZ, LOW);
  digitalWrite(REL, LOW);
  digitalWrite(LED_WIFI, LOW);
  digitalWrite(LED_BLE, LOW);

  // reader one init
  wg.begin_R1();
  // reader 2 init
  wg.begin_R2();
  

}

void loop()
{

  DateTime now = rtc.now();
  lcd.setCursor(0, 0);
  lcd.print(now.year());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.day());
  lcd.setCursor(0, 1);
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());
 
  
  delay(500);

  // put your main code here, to run repeatedly:
}

// put function definitions here:

//*************************Setup system functions//**********************************************
//**********************************************************************************

// i2cscanner
void scanI2CAndSaveData()
{
  Serial.println("Scaning I2C bus..");

  for (byte address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("Dispositivo encontrado en dirección 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);

      // Asignar los números asociados a esta dirección
      deviceInfo[deviceCount].address = address;
      deviceInfo[deviceCount].type = 0;  // esto se procesa por los defaults o se deja en blanco 0 indica no hay tipo asignado
      deviceInfo[deviceCount].order = 0; // esto se procesa por los defaults o se deja en blanco 0 indica esta el dispositivo pero no funcional
      deviceCount++;
    }
  }

  // Store data in memory
  File file = SPIFFS.open("/i2c_data.txt", "w");
  if (!file)
  {
    Serial.println("Error in opening the file.");
    return;
  }

  for (byte i = 0; i < deviceCount; i++)
  {
    file.printf("Address: 0x%02X, type: %d, order: %d\n", deviceInfo[i].address, deviceInfo[i].type, deviceInfo[i].order);
  }
  file.close();

  Serial.println("I2C scanning completed data stored in SPIFFS.");
}
//**********************************************************************************

// i2c configuration recovery

void configRecovery()
{
  // put your main code here, to run repeatedly:
  // Leer los datos desde SPIFFS y reconstruir el vector
  if (SPIFFS.exists("/i2c_data.txt"))
  {
    File file = SPIFFS.open("/i2c_data.txt", "r");
    if (file)
    {
      deviceCount = 0; // Restablece el contador de dispositivos
      while (file.available())
      {
        String line = file.readStringUntil('\n');
        if (line.length() > 0)
        {
          // Analizar la línea y extraer los datos
          sscanf(line.c_str(), "Address: 0x%02X, type: %d, order: %d",
                 &deviceInfo[deviceCount].address, &deviceInfo[deviceCount].type, &deviceInfo[deviceCount].order);
          deviceCount++;
        }
      }
      file.close();
    }
    // Ahora el vector "deviceInfo" contiene los datos recuperados de SPIFFS
    // Puedes acceder a los datos de cada dispositivo mediante deviceInfo[i].
  }
}
//**********************************************************************************

// setting up i2c.

void storeI2cconfig()
{
  // Store data in memory
  File file = SPIFFS.open("/i2c_data.txt", "w");
  if (!file)
  {
    Serial.println("Error in opening the file.");
    return;
  }

  for (byte i = 0; i < deviceCount; i++)
  {
    file.printf("Address: 0x%02X, type: %d, order: %d\n", deviceInfo[i].address, deviceInfo[i].type, deviceInfo[i].order);
  }
  file.close();
}
//**********************************************************************************

// i2c autoconfig
void autoConfig()
{

  for (byte i = 0; i < deviceCount; i++)
  {
    if (deviceInfo[i].address == 0x3F)
    {
      deviceInfo[i].type = 0;
      deviceInfo[i].order = 1;
      lcdDefautl = true;
    }

    if (deviceInfo[i].address == 0x68)
    {
      deviceInfo[i].type = 1;
      deviceInfo[i].order = 1;
      rtcDefault = true;
    }

    if (deviceInfo[i].address == 0x50)
    {
      deviceInfo[i].type = 2;
      deviceInfo[i].order = 1;
      mem0Default = true;
    }
  }
}
