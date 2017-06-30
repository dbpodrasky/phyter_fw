
/** 
 *  *IMPORTANT*
 *  If using an HC-05 based module (typically labled ZS-040) this should be set to true.
 *  Otherwise, leave it false.
 */
const bool USE_CR_NL = true;
typedef enum {
  DBM_NEG_23  = 0,
  DBM_NEG_6   = 1,
  DBM_0       = 2,
  DBM_6       = 3
} HM10_OutputPower;

int configureHM10() {
  //some modules output some status info on start up, this clears the buffer
  hm10_UART.readString();
  Serial.println("Checking bluetooth module connectivity...");
  if (!checkBluetooth()) {
    return BT_NOT_RESPONDING;
  }
  Serial.println("Setting bluetooth name...");
  if (!setBluetoothName("PHYTER")) {
    return BT_SET_NAME_FAILED;
  }
  return BT_SUCCESS;
}

/**
   Checks if the bluetooth module is responding to AT commands.
   @return true if successful
*/
bool checkBluetooth() {
  String at = "AT";
  if (USE_CR_NL) {
    at += "\r\n";
  }
  hm10_UART.print(at);
  String resp = hm10_UART.readString();
  return resp.indexOf("OK") >= 0;
}

/**
   Sets the bluetooth name visible to external devices.
   @return true if successful
*/
bool setBluetoothName(String name) {
  return sendATCommand("NAME", name);
}

/**
   Sends an AT command to the HM-10 using the following format: 'AT+<paramName><value>'
   @param paramName The name of the HM-10 parameter to read or modify
   @param value The value to set the paramter to. Use "" if you wish to read only.
   @return true if successful
*/
bool sendATCommand(String paramName, String value) {
  String command = "AT+";
  command += paramName;
  command += value;
  if (USE_CR_NL) {
    command += "\r\n";    
  }
  Serial.println("Sending AT Command: " + command);
  hm10_UART.print(command);
  String resp = hm10_UART.readString();
  Serial.println("AT Command Response: " + resp);
  return resp.indexOf("OK") >= 0;
}



