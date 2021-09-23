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
      for (int i = 25; i < (int)EEPROM.read(57); ++i)
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
        if(SerialBT.available())
        {
          SSId = SerialBT.readStringUntil(' ');
          Password = SerialBT.readStringUntil(' ');
          Serial.print("Recived SSID = ");Serial.println(SSId);
          Serial.print("Recived password = ");Serial.println(Password);
          SerialBT.print("Recived SSID = ");SerialBT.println(SSId);
          SerialBT.print("Recived password = ");SerialBT.println(Password);
          for (int i = 0; i < 96; ++i)
          {
            EEPROM.write(i, 0);
          }
          EEPROM.write(56,SSId.length());
          EEPROM.write(57,Password.length());
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
        Serial.println("Wifi Credentials are avilable");
        Serial.print("SSID : ");Serial.println(getSsid());
        Serial.print("Password : ");Serial.println(getPass());
      }    
      int count = 3;
      while ((WiFi.status() != WL_CONNECTED) && count)
      {
        Serial.print(".");
        delay(5000);
        Serial.println("WIfi not connected");
        WiFi.begin(SSId.c_str(), Password.c_str());
        count--;
      }
      if(WiFi.status() != WL_CONNECTED) 
      {
        Serial.print("Not able to connect to Wifi");
      }
      else {
        Serial.print("Connected to Wifi");
      }
      
    }
};
