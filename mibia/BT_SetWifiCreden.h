class WifiCerden{
  private:
    String SSId,Password;
    bool credAvailStatus;

  public:
    bool isCredAvil()
    {
      credAvailStatus = EEPROM.readBool(55);
      return credAvailStatus;
    }
    String getSsid()
    {
      SSId ="";
      for (int i = 0; i < (int)EEPROM.read(56); ++i)
      {
         SSId += char(EEPROM.read(i));
      }
      return SSId;
    }
    String getPass()
    {
      Password="";
      for (int i = 25; i < 25 +(int)EEPROM.read(58); ++i)
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
      SerialBT.println("Scannig Done");
      if(ssCount == 0)
      {
        SerialBT.println("No Networks Found");
      }
      else
      {
        SerialBT.print(ssCount);Serial.println(" Networks Fount");
        for (int i = 0; i < ssCount; ++i)
        {
          // Print SSID and RSSI for each network found
          SerialBT.print(i + 1);
          SerialBT.print(": ");
          SerialBT.print(WiFi.SSID(i));
          SerialBT.print(" (");
          SerialBT.print(WiFi.RSSI(i));
          SerialBT.print(")\n");
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
      while(prev + 600000 > millis())
      {
        if(SerialBT.available())
        {
          SSId = SerialBT.readStringUntil('^');
          Password = SerialBT.readStringUntil('^');
          Serial.print("Recived SSID = ");Serial.println(SSId);
          Serial.print("Recived password = ");Serial.println(Password);
          SerialBT.print("Recived SSID = ");SerialBT.println(SSId);
          SerialBT.print("Recived password = ");SerialBT.println(Password);
          for (int i = 0; i < 96; ++i)
          {
            EEPROM.write(i, 0);
          }
          EEPROM.write(56,SSId.length());
          EEPROM.write(58,Password.length());
          Serial.println("writing eeprom ssid:");
          for (int i = 0; i < SSId.length(); ++i)
          {
            EEPROM.write(i, SSId[i]);
            Serial.print("Wrote: ");
            Serial.println(SSId[i]);
          }
          Serial.println("writing eeprom pass:");
          for (int i =0 ; i < Password.length(); ++i)
          {
            EEPROM.write(25 + i, Password[i]);
            Serial.print("Wrote: ");
            Serial.println(Password[i]);
          }
          EEPROM.writeBool(55,true);
          EEPROM.commit();
          break;
        }
      }
    }
    void ConnectToWifi()
    {
      if(!isCredAvil())
      {
        Serial.println("Wifi Credentials are not avilable");
        return;
      }
      if(isCredAvil())
      {
        SerialBT.println("Wifi Credentials are avilable");
        SerialBT.print("SSID : ");SerialBT.println(getSsid());
        SerialBT.print("Password : ");SerialBT.println(getPass());
      }    
      int count = 3;
      while ((WiFi.status() != WL_CONNECTED) && count)
      {
        delay(5000);
        SerialBT.print(count+" : ");
        SerialBT.println("Retrying to connect to wifi network");
        WiFi.begin(getSsid().c_str(), getPass().c_str());
        count--;
      }
      if(WiFi.status() != WL_CONNECTED) 
      {
        SerialBT.print("Not able to connect to Wifi make sure given SSID and Password are Correct");
      }
      else {
        SerialBT.print("Connected to Wifi");
      }
      
    }
};
