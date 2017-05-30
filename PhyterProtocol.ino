
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

char dataBuffer[64];

void parseCommands() {
  if (!Serial.available()) return;
  int count = Serial.readBytesUntil(CMD_RESP_TERMINATOR, dataBuffer, 64);
  if (count == 0) return;
  switch (dataBuffer[0]) {
    case SET_SALINITY:
      setSalinity(*((double *) &dataBuffer[1]));
      sendSalResponse();
      break;
    case BACKGROUND:
      background();
      Serial.write(BACKGROUND_RESP);
      break;
    case MEASURE:
      measure();
      sendMeasureResponse();
      break;
    case LED_INTENSITY:
      ledIntensityCheck();
      Serial.write(LED_INTENSITY_RESP);
      break;
    default:
      Serial.write(ERROR_RESP);
      break;
  }
}

void sendSalResponse() {
  dataBuffer[0] = SET_SALINITY_RESP;
  *((double *) &dataBuffer[1]) = salinity;
  Serial.write(dataBuffer, 5);
}

void sendMeasureResponse() {
  dataBuffer[0] = MEASURE_RESP;
  *((double *) &dataBuffer[1]) = pH;
  *((double *) &dataBuffer[5]) = temp;
  Serial.write(dataBuffer, 9);
}






