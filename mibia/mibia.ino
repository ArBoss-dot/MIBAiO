#include "BluetoothSerial.h" 
BluetoothSerial SerialBT;
#include "EEPROM.h"
#include <WiFi.h>
WiFiClient client;
#include <PubSubClient.h>
#include <AceButton.h>
using namespace ace_button;
PubSubClient Mqtt_Client(client);
#include "BT_SetWifiCreden.h"
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

int lastTtoggleState_1 = 1;
int lastTtoggleState_2 = 1; 
int lastTtoggleState_3 = 1; 
int lastTtoggleState_4 = 1;

//================================================================================//
                            //TLS SSL Certificate//
const char* test_root_ca= \
     "-----BEGIN CERTIFICATE-----\n" \
     "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
     "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
     "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
     "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
     "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
     "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
     "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
     "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
     "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
     "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
     "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
     "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
     "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
     "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
     "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
     "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
     "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
     "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
     "-----END CERTIFICATE-----\n";

//=================================================================================//
/*                      || Class for GPIO Handle ||                               */
//=================================================================================//
class GPIOHandle
{   public:
      int toggleState_1 = 1; //Define integer to remember the toggle state for relay 1
      int toggleState_2 = 1; //Define integer to remember the toggle state for relay 2
      int toggleState_3 = 1; //Define integer to remember the toggle state for relay 3
      int toggleState_4 = 1; //Define integer to remember the toggle state for relay 4

    public:
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
    void bluetoothCommand(char cmd)
    {
      switch(cmd)
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
//        case 'M' : connectToBroker();
//                  break;
        case 'Z': GPIOHandle::all_Switch_ON(); break;  // if 'Z' received Turn on all Relays
        case 'z': GPIOHandle::all_Switch_OFF(); break; // if 'z' received Turn off all Relays
        default : break;
      } 
    }
    void MqttCommand(char cmd)
    {
      switch(cmd)
      {
        case 'A': digitalWrite(RelayPin1, LOW);  toggleState_1 = 0; break; // if 'A' received Turn on Relay1
        case 'a': digitalWrite(RelayPin1, HIGH); toggleState_1 = 1; break; // if 'a' received Turn off Relay1
        case 'B': digitalWrite(RelayPin2, LOW);  toggleState_2 = 0; break; // if 'B' received Turn on Relay2
        case 'b': digitalWrite(RelayPin2, HIGH); toggleState_2 = 1; break; // if 'b' received Turn off Relay2
        case 'C': digitalWrite(RelayPin3, LOW);  toggleState_3 = 0; break; // if 'C' received Turn on Relay3
        case 'c': digitalWrite(RelayPin3, HIGH); toggleState_3 = 1; break; // if 'c' received Turn off Relay3
        case 'D': digitalWrite(RelayPin4, LOW);  toggleState_4 = 0; break; // if 'D' received Turn on Relay4
        case 'd': digitalWrite(RelayPin4, HIGH); toggleState_4 = 1; break; // if 'd' received Turn off Relay4
        case 'Z': GPIOHandle::all_Switch_ON(); break;  // if 'Z' received Turn on all Relays
        case 'z': GPIOHandle::all_Switch_OFF(); break; // if 'z' received Turn off all Relays
        default : break;
      } 
    }

    
};
GPIOHandle IOHandle;
//=============================================================================================================//

/* --------------------------------------- CLASS for MQTT --------------------------------- */
class MqttHandel {
  private:
    const char* DeviceId= "abcd" ;
    const char*  server = "31.170.165.62";
    int port = 8883;
    String PublishTopic="outTopic";
    const char* SubscribeTopic = "inTopic";
    const char* BrokerUserName = "mibaio" ;
    const char* BrokerPassword = "password";
    friend class GPIOHandle;
  public:
   //----------------------Constructor----------------------//
    MqttHandel()
    {
      Mqtt_Client.setServer(server,port);
//      Mqtt_Client.setClient(client);
      Mqtt_Client.setCallback(handleRecivedMessage);
    }
    //------------------------------------------------------//
    
    void setMqttServer(char* serverID) //setServer;
    {
      this-> server = serverID;
    }
    
    void setMqttPort(int port)  //setServerPort
    {
      this->port = port;
    }

    void setPublishTopic(char* topic)
    {
      this->PublishTopic = topic;
    }
    
    void setSubscribeTopic(char* topic)
    {
      this->SubscribeTopic = topic; 
    }
    
    void connectToBroker()  //connect to Broker
    {
      if (Mqtt_Client.connect(DeviceId,"mibaio","password")) {
        Serial.println("Connected to mqtt");
        Mqtt_Client.subscribe("inTopic");
      }
      else {
        Serial.println("Not Able to Connect");
      }   
    }
    
    static void handleRecivedMessage(char* topic, byte* payload, unsigned int length)
    {  
    char  ch;
    for (int i=0;i<length;i++) 
      {
        Serial.print((char)payload[i]);
        ch = (char)payload[i];
      }
      IOHandle.MqttCommand(ch);
      Serial.println();
    }
    void publishState(const char* state)
    {
      Mqtt_Client.publish("outTopic",state);
    }
};
  MqttHandel Mqtt;
/*----------------------------------------------------------------------------------------*/


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run
`make menuconfig` to and enable it
#endif



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
void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) 
{
  Serial.println("EVENT1");
  switch (eventType) 
  {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      IOHandle.toggleState_1 = 0;
      digitalWrite(RelayPin1, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      IOHandle.toggleState_1 = 1;
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
      IOHandle.toggleState_2 = 0;
      digitalWrite(RelayPin2, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      IOHandle.toggleState_2 = 1;
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
      IOHandle.toggleState_3 = 0;
      digitalWrite(RelayPin3, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      IOHandle.toggleState_3 = 1;
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
      IOHandle.toggleState_4 = 0;
      digitalWrite(RelayPin4, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      IOHandle.toggleState_4 = 1;
      digitalWrite(RelayPin4, HIGH);
      break;
  }
}


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


void Bluetooth_handle()
{
  bt_data = SerialBT.read();
  Serial.println(bt_data);
  delay(20);
  IOHandle.bluetoothCommand(bt_data);
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
    creden.ConnectToWifi();
  }
  
//  client.setCACert(test_root_ca);
  Mqtt.connectToBroker();
  
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
  if(lastTtoggleState_1 != IOHandle.toggleState_1)
  {
    String st = "Device-1:";
    st += (String)IOHandle.toggleState_1;

    Mqtt.publishState(st.c_str());
    lastTtoggleState_1=IOHandle.toggleState_1;
  }
  if(lastTtoggleState_2 != IOHandle.toggleState_2)
  {
    String st = "Device-2:";
    st += (String)IOHandle.toggleState_2;
    Mqtt.publishState(st.c_str());
    lastTtoggleState_2=IOHandle.toggleState_2;
  }
  if(lastTtoggleState_3 != IOHandle.toggleState_3)
  {
    String st = "Device-3:";
    st += (String)IOHandle.toggleState_3;
    Mqtt.publishState(st.c_str());
    lastTtoggleState_3=IOHandle.toggleState_3;
  }
  if(lastTtoggleState_4 != IOHandle.toggleState_4)
  {
    String st = "Device-4:";
    st += (String)IOHandle.toggleState_4;
    Mqtt.publishState(st.c_str());
    lastTtoggleState_4=IOHandle.toggleState_4;
  }
  button1.check();
  button2.check();
  button3.check();
  button4.check();
  if (!Mqtt_Client.connected()) {
    Mqtt.connectToBroker(); 
  }
  Mqtt_Client.loop();
}
