//#include <EEPROM.h>
#include "EEPROM.h"
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
EEPROMClass ssId("eeprom0",0x25);
EEPROMClass Pass("eeprom1",0x15);
int address=0;
  
class WifiCredential{
  private:
    String SsID,Password;
    int address;

  public:
    WifiCredential(int adds)
    {
        address = adds;
    }
    bool isWifiConfig()
    {
//       return EEPROM.readBool(address);
         return address;
       
    }
    String getSsid()
    {
      ssId.get(0,SsID);
      return SsID;
    }
    String getPass()
    {
      Pass.get(0,Password);
      return Password;
    }
    void configWifiCreden()
    {
      if(!ssId.begin(ssId.length()) || !Pass.begin(Pass.length()))
      {
        Serial.println("failed to intialise");
        Serial.println("Restarting....");
      }
      long long int pervMil = millis();
      while((pervMil+30000)>millis()) //Wait for 30 sec to get input via BLT;
      {
//        if(SerialBT.available())
          if(Serial.available())
        {
//          SsID = SerialBT.readStringUntil('\n');
          SsID = Serial.readStringUntil(' ');
//          Password = SerialBT.readStringUntil('\n');
          Password = SerialBT.readStringUntil(' ');
          ssId.put(0,SsID);
          Pass.put(0,Password);
//          EEPROM.writeBool(address,true);
          address = 1;
        }
      }
      
      if(EEPROM.readBool(address))
      {
        Serial.println("SSID & PASSWORD Updated");
        Serial.print("SSID : ");Serial.println(SsID);
        Serial.print("Password : ");Serial.println(Password);
      }
      else{
        Serial.print("SSID & PASSWORD Not Set Try again");
      }
    }
};

void setup()
{
  Serial.begin(115200);
//  EEPROM.begin(1);
//  EEPROM.writeBool(1,false);


  
}
void loop()
{
  
  WifiCredential wiFi(address);
  if(!wiFi.isWifiConfig())
  {
    wiFi.configWifiCreden();
  }
  else
  {
    Serial.println("Credentials are Available");
    Serial.print("SSID : ");Serial.println(wiFi.getSsid());
    Serial.print("Password : ");Serial.println(wiFi.getPass());
  }
  delay(1000);
  
}
