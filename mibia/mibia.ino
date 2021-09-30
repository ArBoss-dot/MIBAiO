  /**********************************************************************************
 *  TITLE: Manual-Internet-Bluetooth All-in-One home automation using ESP-32
 *  Download Board ESP32: https://github.com/espressif/arduino-esp32
 *  Download the libraries
 *  BluetoothSerial: https://github.com/espressif/arduino-esp32/tree/master/libraries/BluetoothSerial
 *  AceButton Library: https://github.com/bxparks/AceButton
 *  Blynk library  : https://github.com/blynkkk/blynk-library/releases/latest
 **********************************************************************************/


#include "BluetoothSerial.h" 
BluetoothSerial SerialBT;
#include "EEPROM.h"
#include "WiFi.h"

#include "BT_SetWifiCreden.h"
#include <AceButton.h>
using namespace ace_button;


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run
`make menuconfig` to and enable it
#endif

WifiCerden creden;
// define the GPIO connected with Relays
#define RelayPin1 15  //23  //D23
#define RelayPin2 2   //22  //D22
#define RelayPin3 4   //21  //D21
#define RelayPin4 22  //19  //D19

// define the GPIO connected with switches
#define SwitchPin1 32  //13  //D13
#define SwitchPin2 35  //12  //D12
#define SwitchPin3 34  //14  //D14
#define SwitchPin4 39  //27  //D27

//Manual Internet Bluetooth LEDs
#define LED1 26
#define LED2 25
#define LED3 27

//Virtual pins for Blynk 
//#define VPIN_BUTTON_1    V1 
//#define VPIN_BUTTON_2    V2
//#define VPIN_BUTTON_3    V3 
//#define VPIN_BUTTON_4    V4

int toggleState_1 = 1; //Define integer to remember the toggle state for relay 1
int toggleState_2 = 1; //Define integer to remember the toggle state for relay 2
int toggleState_3 = 1; //Define integer to remember the toggle state for relay 3
int toggleState_4 = 1; //Define integer to remember the toggle state for relay 4
int wifiFlag = 0; //Mode of device manual-internet or manua-bluetooth
char bt_data; // variable for storing bluetooth data 

ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);
ButtonConfig config4;
AceButton button4(&config4);

void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);
void handleEvent4(AceButton*, uint8_t, uint8_t);


//=============================================BluetoothAuth=======================================================//
boolean confirmRequestPending = false;

void BTConfirmRequestCallback(uint32_t numVal)
{
  confirmRequestPending = true;
  Serial.println(numVal);
}

void BTAuthCompleteCallback(boolean success)
{
  confirmRequestPending = false;
  if (success)
  {
    Serial.println("Pairing success!!");
  }
  else
  {
    Serial.println("Pairing failed, rejected by user!!");
  }
}
//===============================================================================================================//


void all_Switch_ON()
{
  digitalWrite(RelayPin1, LOW); toggleState_1 = 0; delay(100);
  digitalWrite(RelayPin2, LOW); toggleState_2 = 0; delay(100);
  digitalWrite(RelayPin3, LOW); toggleState_3 = 0; delay(100);
  digitalWrite(RelayPin4, LOW); toggleState_4 = 0; delay(100);
}

void all_Switch_OFF()
{
  digitalWrite(RelayPin1, HIGH); toggleState_1 = 1; delay(100);
  digitalWrite(RelayPin2, HIGH); toggleState_2 = 1; delay(100);
  digitalWrite(RelayPin3, HIGH); toggleState_3 = 1; delay(100);
  digitalWrite(RelayPin4, HIGH); toggleState_4 = 1; delay(100);
}

void Bluetooth_handle()
{
  bt_data = SerialBT.read();
  Serial.println(bt_data);
  delay(20);

  switch(bt_data)
      {
        case 'A': digitalWrite(RelayPin1, LOW);  toggleState_1 = 0; break; // if 'A' received Turn on Relay1
        case 'a': digitalWrite(RelayPin1, HIGH); toggleState_1 = 1; break; // if 'a' received Turn off Relay1
        case 'B': digitalWrite(RelayPin2, LOW);  toggleState_2 = 0; break; // if 'B' received Turn on Relay2
        case 'b': digitalWrite(RelayPin2, HIGH); toggleState_2 = 1; break; // if 'b' received Turn off Relay2
        case 'C': digitalWrite(RelayPin3, LOW);  toggleState_3 = 0; break; // if 'C' received Turn on Relay3
        case 'c': digitalWrite(RelayPin3, HIGH); toggleState_3 = 1; break; // if 'c' received Turn off Relay3
        case 'D': digitalWrite(RelayPin4, LOW);  toggleState_4 = 0; break; // if 'D' received Turn on Relay4
        case 'd': digitalWrite(RelayPin4, HIGH); toggleState_4 = 1; break; // if 'd' received Turn off Relay4
        case 'E':
                  creden.configWifiCredentials(); //For configuring Wifi Credentials
                  break;
        case 'F': creden.ConnectToWifi(); //To make attempt to connect Wifi
                  break;
        case 'Z': all_Switch_ON(); break;  // if 'Z' received Turn on all Relays
        case 'z': all_Switch_OFF(); break; // if 'z' received Turn off all Relays
        default : break;
      }
}

void setup()
{
  Serial.begin(115200);
  
  EEPROM.begin(100);  
  btStart();  //Serial.println("Bluetooth On");
  
  if(!creden.isCredAvil())
  {
    Serial.println("Wifi Credentials are not avilable");
  }
  if(creden.isCredAvil())
  {
    Serial.println("Wifi Credentials are avilable");
    Serial.print("SSID : ");Serial.println(creden.getSsid());
    Serial.print("Password : ");Serial.println(creden.getPass());
  }
  
  SerialBT.enableSSP();
  SerialBT.onConfirmRequest(BTConfirmRequestCallback);
  SerialBT.onAuthComplete(BTAuthCompleteCallback);
  SerialBT.begin("MIBAiO_BLE3"); //Bluetooth device name
  
  Serial.println("The device started, now you can pair it with bluetooth!");
  delay(500);

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  
//  pinMode(Buzzer, OUTPUT);
  digitalWrite(LED1, HIGH);
  delay(200);
  digitalWrite(LED2, HIGH);
  delay(200);
  digitalWrite(LED3, HIGH);
  delay(200);
  
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  delay(500);

  digitalWrite(LED1, HIGH);
  delay(200);
  digitalWrite(LED2, HIGH);
  delay(200);
  digitalWrite(LED3, HIGH);
  delay(200);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);  
  
  //At the beginning all the relays are turned on/off according to their respective manual switch state
  digitalWrite(RelayPin1, digitalRead(SwitchPin1));
  digitalWrite(RelayPin2, digitalRead(SwitchPin2));
  digitalWrite(RelayPin3, digitalRead(SwitchPin3));
  digitalWrite(RelayPin4, digitalRead(SwitchPin4));
  
  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);
  config3.setEventHandler(button3Handler);
  config4.setEventHandler(button4Handler);
  
  button1.init(SwitchPin1);
  button2.init(SwitchPin2);
  button3.init(SwitchPin3);
  button4.init(SwitchPin4);
  delay(1000);
}

void loop()
{  
  if (confirmRequestPending)
  {
    if (touchRead(T0)<50)
    {
      SerialBT.confirmReply(true);
    }
    else
    {
      SerialBT.confirmReply(false);
    }
    
  }
  if (SerialBT.available())
  {
    Bluetooth_handle();
  }
  button1.check();
  button2.check();
  button3.check();
  button4.check();
}

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) 
{
  Serial.println("EVENT1");
  switch (eventType) 
  {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      toggleState_1 = 0;
      digitalWrite(RelayPin1, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      toggleState_1 = 1;
      digitalWrite(RelayPin1, HIGH);
      break;
  }
}

void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) 
{
  Serial.println("EVENT2");
  switch (eventType) 
  {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      toggleState_2 = 0;
      digitalWrite(RelayPin2, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      toggleState_2 = 1;
      digitalWrite(RelayPin2, HIGH);
      break;
  }
}

void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) 
{
  Serial.println("EVENT3");
  switch (eventType) 
  {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      toggleState_3 = 0;
      digitalWrite(RelayPin3, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      toggleState_3 = 1;
      digitalWrite(RelayPin3, HIGH);
      break;
  }
}

void button4Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) 
{
  Serial.println("EVENT4");
  switch (eventType) 
  {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      toggleState_4 = 0;
      digitalWrite(RelayPin4, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      toggleState_4 = 1;
      digitalWrite(RelayPin4, HIGH);
      break;
  }
}
