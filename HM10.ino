
  
  typedef enum {
    DBM_NEG_23  = 0,
    DBM_NEG_6   = 1,
    DBM_0       = 2,
    DBM_6       = 3
  } HM10_OutputPower;

  int configureHM10() {
    //some modules output some status info on start up, this clears the buffer
    Serial.readString();
    if (!checkBluetooth()) {
      return BT_NOT_RESPONDING;
    }
    if (!setBluetoothName("PHYTER-DEV-KIT")) {
      return BT_SET_NAME_FAILED;
    }
    return BT_SUCCESS;
  }

  /**
   * Checks if the bluetooth module is responding to AT commands.
   * @return true if successful
   */
  bool checkBluetooth() {
    Serial.print("AT\r\n");
    String resp = Serial.readString();
    return resp.indexOf("OK") > 0;
  }
  
  /**
   * Sets the bluetooth name visible to external devices.
   * @return true if successful
   */
  bool setBluetoothName(String name) {
    return sendATCommand("NAME", name); 
  }
  
  /**
   * Sends an AT command to the HM-10 using the following format: 'AT+<paramName><value>'
   * @param paramName The name of the HM-10 parameter to read or modify
   * @param value The value to set the paramter to. Use "" if you wish to read only.
   * @return true if successful
   */
  bool sendATCommand(String paramName, String value) {
    String command = "AT+";
    command += paramName;
    command += value;
    command += "\r\n";
    Serial.print(command);
    String resp = Serial.readString();
    return resp.indexOf("OK") > 0;
  }


