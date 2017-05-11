
const char CMD_RESP_TERMINATOR = '\r';

typedef enum {
  SET_SALINITY  = 0x01,
  BACKGROUND    = 0x02,
  MEASURE       = 0x03,
  LED_INTENSITY = 0x04
} PhyterCommand;

typedef enum {
  SET_SALINITY_RESP   = 0x81,
  BACKGROUND_RESP     = 0x82,
  MEASURE_RESP        = 0x83,
  LED_INTENSITY_RESP  = 0x84,
  ERROR_RESP          = 0xFF
} PhyterResponse;

char dataBuffer[255];

void parseCommands() {
  if (!Serial.available()) return;
  int count = Serial.readBytesUntil(CMD_RESP_TERMINATOR, dataBuffer, 255);
  if (count == 0) return;
  switch (dataBuffer[0]) {
    case SET_SALINITY:
      setSalinity(dataBuffer[1]);
      break;
    case BACKGROUND:
      background();
      break;
    case MEASURE:
      measure();
      break;
    case LED_INTENSITY:
      ledIntensityCheck();
      break;  
  }
}






