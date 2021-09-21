#include "EEPROM.h"
#include "BluetoothSerial.h"
#include "WiFi.h"
BluetoothSerial SerialBT;
class WifiCerden{
  private:
    String SSId,Password;
    bool credAvailStatus;

  public:
    bool isCredAvil()
    {
      credAvailStatus = EEPROM.readBool(1);
      return credAvailStatus;
    }
    String getSsid()
    {
      SSId ="";
      for (int i = 2; i < 34; ++i)
      {
         SSId += char(EEPROM.read(i));
      }
      return SSId;
    }
    String getPass()
    {
      Password="";
      for (int i = 34; i <98; ++i)
      {
         Password += char(EEPROM.read(i));
      }
      return Password;
      
    }
    void displayAvailSSID()
    {
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(50);
      int ssCount = WiFi.scanNetworks();
      Serial.println("Scannig Done");
      if(ssCount == 0)
      {
        Serial.println("No Networks Found");
      }
      else
      {
        Serial.print(ssCount);Serial.println(" Networks Fount");
        for (int i = 0; i < ssCount; ++i)
        {
          // Print SSID and RSSI for each network found
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.print(WiFi.SSID(i));
          Serial.print(" (");
          Serial.print(WiFi.RSSI(i));
          Serial.print(")");
//          Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
          delay(10);
        }
      }
    }
    void configWifiCredentials()
    {
      displayAvailSSID();
      Serial.println("Please Enter SSID and Passwords with in a min in separete line");
      long long int prev = millis();
      while(prev + 60000 > millis())
      {
        if(Serial.available())
        {
          SSId = Serial.readStringUntil('\n');
          Password = Serial.readStringUntil('\n');
          Serial.print("Recived SSID = ");Serial.println(SSId);
          Serial.print("Recived password = ");Serial.println(Password);
          for (int i = 0; i < 96; ++i)
          {
            EEPROM.write(i, 0);
          }
          Serial.println("writing eeprom ssid:");
          for (int i = 2; i < SSId.length(); ++i)
          {
            EEPROM.write(i, SSId[i]);
            Serial.print("Wrote: ");
            Serial.println(SSId[i]);
          }
          Serial.println("writing eeprom pass:");
          for (int i = 2; i < Password.length(); ++i)
          {
            EEPROM.write(32 + i, Password[i]);
            Serial.print("Wrote: ");
            Serial.println(Password[i]);
          }
          EEPROM.writeBool(1,true);
          EEPROM.commit();
          break;
        }
      }
    }    
};



void setup() {
  Serial.begin(115200);
  EEPROM.begin(100);
  delay(10);
  WifiCerden creden;
  if(!creden.isCredAvil())
  {
    Serial.println("Wifi Credentials are not avilable");
    creden.configWifiCredentials();
  }
  if(creden.isCredAvil())
  {
    Serial.println("Wifi Credentials are avilable");
    Serial.print("SSID : ");Serial.println(creden.getSsid());
    Serial.print("Password : ");Serial.println(creden.getPass());
  }
  
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
