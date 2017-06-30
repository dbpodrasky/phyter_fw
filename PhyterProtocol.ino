
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
  MEASURE_RESP1       = 0x83,
  MEASURE_RESP2       = 0x84,
  LED_INTENSITY_RESP  = 0x85,
  ERROR_RESP          = 0xFF
} PhyterResponse;

char dataBuffer[64];

void parseCommands() {
  if (!hm10_UART.available()) return;
  int count = hm10_UART.readBytesUntil(CMD_RESP_TERMINATOR, dataBuffer, 64);
  if (count == 0) return;
  switch (dataBuffer[0]) {
    case SET_SALINITY:
      setSalinity(*((double *) &dataBuffer[1]));
      sendSalResponse();
      break;
    case BACKGROUND:
      background();
      hm10_UART.write(BACKGROUND_RESP);
      break;
    case MEASURE:
      measure();
      sendMeasureResponse();
      break;
    case LED_INTENSITY:
      ledIntensityCheck();
      hm10_UART.write(LED_INTENSITY_RESP);
      break;
    default:
      hm10_UART.write(ERROR_RESP);
      break;
  }
}

void sendSalResponse() {
  dataBuffer[0] = SET_SALINITY_RESP;
  *((double *) &dataBuffer[1]) = salinity;
  hm10_UART.write(dataBuffer, 5);
}

void sendMeasureResponse() {
  dataBuffer[0] = MEASURE_RESP1;
  *((double *) &dataBuffer[1]) = pH;
  *((double *) &dataBuffer[5]) = temp;
  hm10_UART.write(dataBuffer, 9);
  delay(500);
  dataBuffer[0] = MEASURE_RESP2;
  *((double *) &dataBuffer[1]) = A578;
  *((double *) &dataBuffer[5]) = A434;
  *((double *) &dataBuffer[9]) = dark;
  hm10_UART.write(dataBuffer, 13); 
}






