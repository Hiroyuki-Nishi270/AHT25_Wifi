void initialSerial(){
  /** Serial initializing */
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("-------------------");
  Serial.println("AHT25_Wifi");
  Serial.println("Created by H.N");
  Serial.println("-------------------");
  Serial.print("TIME_LIMIT   : ");Serial.println(TIME_LIMIT);
}