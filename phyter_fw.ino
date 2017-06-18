  #include <Wire.h>
  #include <Adafruit_ADS1015.h>
  #include <SoftwareSerial.h>
  #define BT_SUCCESS          0
  #define BT_NOT_RESPONDING   1
  #define BT_SET_NAME_FAILED  2
   
  Adafruit_ADS1115 ads1115;
  
  //reset function (resets PC to address 0)
  void (* RESET)(void) = 0;
  
  //global absorbance ratio
  double R = 0;
  
  //detector signal pin
  int dPin = 0;
  int dPos = 4;
  
  //time the LED is on
  double dlay = 250;
  
  //LED output pins (434nm and 578nm)
  int gPin = 2;
  int bPin = 3;

  //UART for HM10
  int HM10_tx = 7;
  int HM10_rx = 8;
  SoftwareSerial hm10_UART(HM10_tx, HM10_rx);
  
  //thermistor positive pin
  int thermP = 10;
  //thermistor signal pin
  int thermistorPin = 2;
  
  //on off LED
  int LED = 10;
  
  void setup() {
    //Begin Serial Connections
    Serial.begin(9600);
    ads1115.begin();
    hm10_UART.begin(9600);
    hm10_UART.setTimeout(1000); //1 sec timeout for buffered reads
    
    //Define output pins
    pinMode(gPin,OUTPUT);
    pinMode(bPin,OUTPUT);   
    pinMode(thermP,OUTPUT);
    pinMode(dPos,OUTPUT);
    pinMode(LED,OUTPUT);
    
    //Initializing outputs low    
    digitalWrite(gPin, LOW);
    digitalWrite(bPin, LOW);
    digitalWrite(thermP,LOW);
    digitalWrite(dPos,LOW);
    digitalWrite(LED,HIGH);
    
    //bluetooth
    int btResult = configureHM10();
    if (btResult != BT_SUCCESS) {
      Serial.println("Error initializing bluetooth module");
      fatalError(btResult);
    }
    
  }
  
  
  
  //variable holds pH
  double pH = 0;
      
  //global salinity value
  double temp = 0;
  
  //global salinity value
  double salinity = 35;
  
  //global light intensity signals from detector
  double greenI0 = 0;
  double blueI0 = 0;
  double greenI = 0;
  double blueI = 0;
  double dark = 0;
  
  //Absorbance 
  double A578=0,A434=0;
  void loop() {
    parseCommands();
  }
   
  //reads the blue LED/////////////////////
  double blue()
  {
    //local vairiables
    int i;            //counter         
    double blue = 0;  //blue intensity
    dark = 0;  //dark noise
  
    digitalWrite(dPos,HIGH);
    delay(dlay);
    //Measuring dark noise
    for(i=0;i<10;++i){
      dark+=ads1115.readADC_SingleEnded(dPin);
    }
    //Averaging 10 measurments
    dark/=10;
  
    //turn blue LED on     
    digitalWrite(bPin,HIGH);   
    delay(dlay);
  
    //taking measurement
    for(i=0;i<10;++i){
    blue+=ads1115.readADC_SingleEnded(dPin);
    }
    //Averaging 10 measurments
    blue/=10; 
  
    //Subtracting dark noise
    blue = blue-dark;
  
    //turn blue LED off
    digitalWrite(bPin,LOW);  
    digitalWrite(dPos,LOW);
  
    return blue;
  }
  
  
  //reads the green LED//////////////////////
  double green()
  {
    //local vairiables
    int i;            //counter    
    double green = 0; //green intensity
    dark = 0;  //dark noise
  
    digitalWrite(dPos,HIGH);
    delay(dlay);
    //Measuring dark noise (10 measurements)
    for(i=0;i<10;++i){
      dark+=ads1115.readADC_SingleEnded(dPin);
    }
    //Averaging 10 measurments
    dark/=10;
  
    //turn green LED on     
    digitalWrite(gPin,HIGH);   
    delay(dlay);
  
    //taking measurement
    for(i=0;i<10;++i){
      green+=ads1115.readADC_SingleEnded(dPin);
    }
    //Averaging 10 measurments
    green/=10; 
  
    //Subtracting dark noise
    green = green-dark;
  
    //turn green LED off
    digitalWrite(gPin,LOW);
    digitalWrite(dPos,LOW);
  
    delay(1000);
  
  
    return green;
  }
  
  //Reads thermistor for internal temp [C]///////////////////////
  double getTemp()
  {  
    digitalWrite(thermP,HIGH);
    delay(100);
    //internal temperature measurement (thermistor)
    //+5------\\\\\\---Vx----\\\\\\-------Ground
    //           R             Rt            
    
    //calculates thermistor resistance [ohm]
    int bits = ads1115.readADC_SingleEnded(thermistorPin);         //reading Vx pin
    double Vx = bits*0.000188;            //converting bits to voltage
    double Rt = (14000*Vx)/(5-Vx);                //calculate Rt
  
    //temperature from Steinhart-Hart equation
    //constants
    double a = 0.001018318;
    double b = 0.000240866;
    double c = 0.000000150;
    
    //equation
    double temp = (1/(a+(b*(log(Rt)))+(c*(pow(log(Rt),3)))))-273.15;
    return temp;
  }
  
  
  //calculates pH//////////////////////////////////////////////////////
  double calculate(double inTemp,double inSal,double greenI0,double blueI0,double greenI,double blueI)
  {
    //instance variables 
    double pka,Ea434,Ea578,Eb434,Eb578,e1,e2,e3,pH;
  
    //pKa Calculation
    pka = -241.46+(7085.7/(inTemp+273.15))+43.833*log(inTemp+273.15)-0.08064*(inTemp+273.15)+0.00211*(35-inSal);
  
    //A1 is 578nm
    //Absorption Coeficients
    Ea434 = 17372+20.162*(24.8-inTemp);
  
    Ea578 = 94.1-1.0177*(24.8-inTemp);
  
    Eb434 = 2284.1-6.3863*(24.86-inTemp);
  
    Eb578 = 38676.5+66.808*(24.86-inTemp);
  
    e1 = Ea578/Ea434;
  
    e2 = Eb578/Ea434;
  
    e3 = Eb434/Ea434;
  
    //Absorbance
    A578 = log10(greenI0/greenI);
  
    A434 = log10(blueI0/blueI);
  
    //Ratio(578/434)
    R = A578/A434;
  
    //pH Calculation
    pH = pka+log10((R-e1)/(e2-R*e3));
    
    return pH;
  }

  void setSalinity(double val) {
    salinity = val;
  }

  void background() {
    //Turn detector on
    digitalWrite(dPos,HIGH);
    delay(100);
    //measure LEDs
    greenI0 = green();
    blueI0 = blue();
    digitalWrite(dPos,LOW);
  }

  void measure() {
    //Turn detector on
    digitalWrite(dPos,HIGH);
    delay(100);
    //measure LEDs
    greenI = green();
    blueI = blue();
    digitalWrite(dPos,LOW);
    //get internal temperature [C]
    temp = getTemp();
    //calculates pH
    pH = calculate(temp,salinity,greenI0,blueI0,greenI,blueI);
  }

  void ledIntensityCheck() {
    double testgI0 = green();
    double testbI0 = blue();
  }
 
  /**
   * Error handler. Pulses LEDs <code> times and resets the application.
   * For now, this function pulses the sensor LED's.
   * @param code The error code
   */
  void fatalError(int code) {
    int pulseCount = 0;
    do {
      digitalWrite(gPin, HIGH);
      digitalWrite(bPin, HIGH);
      delay(500);
      digitalWrite(gPin, LOW);
      digitalWrite(bPin, LOW);
      delay(500);
    } while (++pulseCount < code);
    RESET();
  }
  
  
  

