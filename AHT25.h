#include <Wire.h>
#include <CRC8.h>

/** AHT25 setting value */
struct {
  int Status = 0;
  unsigned long NextTime = 0;
  CRC8 crc;
  double temperature;
  double humidity;
  int discomfortIndex;
} AHT25value;
const double AHT25_ERROR_VALUE = 999.0;
const byte AHT25_ADDR = 0x38;

void AHT25(){
  byte buf[7];
  uint32_t humidity_raw;
  uint32_t temperature_raw;
  byte state;

  unsigned long currentTime = millis();
  
  if(AHT25value.Status == 0){
    Wire.begin(AHT25_PIN_I2C_SDA, AHT25_PIN_I2C_SCL);

    AHT25value.NextTime = currentTime + 100;
    AHT25value.Status = 1;
  }else if(AHT25value.Status == 1 && currentTime >= AHT25value.NextTime){
    Wire.beginTransmission(AHT25_ADDR);
    Wire.write(0x71);
    Wire.endTransmission();

    AHT25value.NextTime = currentTime + 10;
    AHT25value.Status = 2;
  }else if(AHT25value.Status == 2 && currentTime >= AHT25value.NextTime){
    AHT25value.crc.setPolynome(0x31);
    AHT25value.crc.setStartXOR(0xFF);

    AHT25value.Status = 3;
  }else if(AHT25value.Status == 3 && currentTime >= AHT25value.NextTime){
    Wire.beginTransmission(AHT25_ADDR);
    Wire.write(0xAC);
    Wire.write(0x33);
    Wire.write(0x00);
    Wire.endTransmission();

    AHT25value.NextTime = currentTime + 80;
    AHT25value.Status = 4;
  }else if(AHT25value.Status == 4 && currentTime >= AHT25value.NextTime){
    Wire.requestFrom(AHT25_ADDR, 7);
    if (Wire.available() >= 7) {
      for(int i=0; i<7; i++) {
        buf[i] = Wire.read();
      }
    }

    if((buf[0] & 0x80) != 0){
      AHT25value.NextTime = currentTime + 80;
      AHT25value.Status = 4;
    }else{
      AHT25value.crc.restart();
      AHT25value.crc.add(buf, 6);

      if (buf[6] == AHT25value.crc.getCRC()) {
        state = buf[0];
        humidity_raw = ((uint32_t)buf[1] << 12)|((uint32_t)buf[2] << 4)|(((uint32_t)buf[3] >> 4) & 0x0F);
        temperature_raw = (((uint32_t)buf[3] & 0x0F) << 16)|((uint32_t)buf[4] << 8)|((uint32_t)buf[5]);

        AHT25value.humidity = humidity_raw / 1048576.0 * 100;
        AHT25value.temperature = temperature_raw / 1048576.0 * 200 - 50;

        /** discomfort index */
        AHT25value.discomfortIndex = AHT25value.temperature * 0.81 + 0.01 * AHT25value.humidity * (0.99 * AHT25value.temperature - 14.3) + 46.3;
      } else {
        // error
        AHT25value.humidity = AHT25_ERROR_VALUE;
        AHT25value.temperature = AHT25_ERROR_VALUE;
      }

      Serial.print("Aht25(");
      Serial.print("temperature: ");
      Serial.print(AHT25value.temperature);
      Serial.print(" humidity: ");
      Serial.print(AHT25value.humidity);
      Serial.print(" disconfort index: ");
      Serial.print(AHT25value.discomfortIndex);
      Serial.print(")");
      Serial.println("");

      //temp.NextTime = _currentTime;
      AHT25value.NextTime = currentTime + 1000 - 80;
      AHT25value.Status = 3;

    }
  }
}