/*
Testing button module
 */
#include <PID_v1.h>
#include <Timer.h>
#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <SPI.h>
#include <PlayingWithFusion_MAX31865.h>              // core library
#include <PlayingWithFusion_MAX31865_STRUCT.h>       // struct library
#include <Servo.h>

Servo exhaustServo;
Servo intakeServo;
Timer t;

int exhaustPosition_percent = 90;
int exhaustPosition_raw = 90;
int intakePosition_percent = 90;
int intakePosition_raw = 90; 
int fanSpeed = 30;
// CS pin used for the connection with the sensor
// other connections are controlled by the SPI library)
const int CS0_PIN = 9;
const int CS1_PIN = 10;
const int fanPin = 6;

PWFusion_MAX31865_RTD rtd_ch0(CS0_PIN);
PWFusion_MAX31865_RTD rtd_ch1(CS1_PIN);


Adafruit_LiquidCrystal lcd(15, 2, 4);

const int backButtonPin = 19;
const int upButtonPin = 17;
const int downButtonPin = 18;
const int enterButtonPin = 16;
const int buttonLightsPin = 5;


int brightness = 20;    // how bright the LED is
int backButtonState = 0;
int upButtonState = 0;
int downButtonState = 0;
int enterButtonState = 0;
int menuSelected = 1;
int airFlow_percent;
int manualServo_percent = 50;
int exhaustBias = 35;
int fanBias = 25;

unsigned long previousMillis = 0;
const long interval = 1000;

double smokeTemp;
double meatTemp;
double Setpoint =  250; 
double Output;                  // From PID Example

double Kp=2, Ki=5, Kd=1;
PID myPID(&smokeTemp, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup() {

  t.every(1500, readRTDs);
  lcd.begin(20, 4);
  
  myPID.SetMode(AUTOMATIC);                    //   From PID Example
  
  exhaustServo.attach(8,700,2200);
  intakeServo.attach(7,700,2200);
  
  // setup for the the SPI library:
  SPI.begin();                            // begin SPI
  SPI.setClockDivider(SPI_CLOCK_DIV16);   // SPI speed to SPI_CLOCK_DIV16 (1MHz)
  SPI.setDataMode(SPI_MODE3);             // MAX31865 works in MODE1 or MODE3
  
  // initalize the chip select pin
  pinMode(CS0_PIN, OUTPUT);
  pinMode(CS1_PIN, OUTPUT);
  pinMode(fanPin, OUTPUT);
  rtd_ch0.MAX31865_config();
  rtd_ch1.MAX31865_config();
  
  // give the sensor time to set up
  delay(100);

pinMode(backButtonPin, INPUT);
pinMode(upButtonPin, INPUT);
pinMode(downButtonPin, INPUT);
pinMode(enterButtonPin, INPUT);
pinMode(buttonLightsPin, OUTPUT);

}


//                                    LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP
void loop() {

  t.update(); 
 
  myPID.Compute();
//  analogWrite(PIN_OUTPUT, Output);                               FROM PID EXAMPLE
  
  exhaustServo.write(exhaustPosition_raw);
  intakeServo.write(intakePosition_raw);
  analogWrite(buttonLightsPin, brightness);
  analogWrite(fanPin, fanSpeed);
  
 
  airFlow_percent = map (Output, 0, 255, 0, 100);  

  
  intakePosition_raw = map(intakePosition_percent, 0, 100, 160, 61);
  exhaustPosition_raw = map(exhaustPosition_percent, 0, 100, 160, 61);
  

  backButtonState = digitalRead(backButtonPin);
  upButtonState = digitalRead(upButtonPin);
  downButtonState = digitalRead(downButtonPin);
  enterButtonState = digitalRead(enterButtonPin); 
  
  switch (menuSelected) {
    case 1:
        homeScreen();
        break;

    case 2:
        pidScreen();
        break;
        
    case 3:
        setpointScreen();
        break;
        
    case 4:
        propgainScreen();
        break;
        
    case 5:
        integralScreen();
        break;
    
    case 6:
        exhaustbiasScreen();
        break;
        
     case 7:
        fanbiasScreen();
        break;

     case 8:
        servoScreen();
        break;        
        
  }

}


void homeScreen(){            // homeScreen      homeScreen      homeScreen      homeScreen      homeScreen      homeScreen      homeScreen

    calculateValvePosition();

  if (backButtonState == 1){      
        menuSelected = 6;
        }
  if (upButtonState == 1){
        brightness++;
        }
  if (downButtonState == 1){
        brightness--;
        }
  if (enterButtonState == 1){
        menuSelected = 2;
        }
        
    lcd.setCursor(0,0);
    lcd.print(" Meat Temp: ");
    lcd.setCursor(12,0);
    lcd.print(meatTemp);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print("Smoke Temp: ");
    lcd.setCursor(12,1);
    lcd.print(smokeTemp);
    lcd.print(" ");
  
    lcd.setCursor(0,2);
    lcd.print(" i:");
    lcd.setCursor(3,2);
    lcd.print(intakePosition_percent);
    lcd.print("     ");
    lcd.setCursor(8,2);
    lcd.print("e:");
    lcd.setCursor(10,2);
    lcd.print(exhaustPosition_percent);
    lcd.print("       ");
    
    lcd.setCursor(0,3);
    lcd.print("af:");
    lcd.setCursor(3,3);
    lcd.print(airFlow_percent);
    lcd.print("     ");
    lcd.setCursor(8,3);
    lcd.print("f:");
    lcd.setCursor(10,3);
    lcd.print(fanSpeed);
    lcd.print("       ");
   

}

void pidScreen(){                      // PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID  

     calculateValvePosition();
  
  if (backButtonState == 1){ 
        menuSelected = 1;
        }
  if (upButtonState == 1){
        intakePosition_percent = intakePosition_percent + 1;
        }
  if (downButtonState == 1){
        intakePosition_percent = intakePosition_percent - 1;
        }
  if (enterButtonState == 1){       
        menuSelected = 3;
        
}  
        
    lcd.setCursor(0,0);
    lcd.print("      pv:");
    lcd.setCursor(9,0);
    lcd.print(smokeTemp);
    lcd.print("    ");
 
    lcd.setCursor(0,1);
    lcd.print("  cv:");
    lcd.setCursor(5,1);  
    lcd.print(Output);
    lcd.print("      ");
    lcd.setCursor(12,1);
    lcd.print("af:");
    lcd.setCursor(15,1);  
    lcd.print(airFlow_percent);
    lcd.print("    ");
    
    lcd.setCursor(0,2);
    lcd.print("  Kp:");
    lcd.setCursor(5,2);  
    lcd.print(Kp);
    lcd.print("    ");
    lcd.setCursor(10,2);
    lcd.print("  Ki:");
    lcd.setCursor(15,2);  
    lcd.print(Ki);
    lcd.print("   ");
    
    lcd.setCursor(0,3);
    lcd.print("      sp:");
    lcd.setCursor(9,3);
    lcd.print(Setpoint);
    lcd.print("      ");
}

void setpointScreen(){ 

      calculateValvePosition();  
  
  if (backButtonState == 1){       // setpointScreen    setpointScreen    setpointScreen    setpointScreen    setpointScreen    setpointScreen    setpointScreen        
        menuSelected =2;
        }
  if (upButtonState == 1){
        if( Setpoint >= 0){
        Setpoint++;}
        }
  if (downButtonState == 1){
        if( Setpoint < 500){
        Setpoint--;}
        }
  if (enterButtonState == 1){   
        menuSelected =4;
        }
 
    lcd.setCursor(0,0);
    lcd.print("      SETPOINT      ");
    lcd.setCursor(0,1);
    lcd.print("      sp:");
    lcd.setCursor(9,1);  
    lcd.print(Setpoint);
    lcd.print("             ");
    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
}

void propgainScreen(){ 

      calculateValvePosition(); 
  
  if (backButtonState == 1){       // propgainScreen    propgainScreen    propgainScreen    propgainScreen    propgainScreen    propgainScreen        
        menuSelected =3;
        }
  if (upButtonState == 1){
        if( Kp >= 1){
        Kp = Kp + .05;}
        }
  if (downButtonState == 1){
        if( Kp < 10){
        Kp = Kp - .05;}
        }
  if (enterButtonState == 1){   
         menuSelected =5;
        }
 
    lcd.setCursor(0,0);
    lcd.print("      SET GAIN     ");
    lcd.setCursor(0,1);
    lcd.print("      Kp:");
    lcd.setCursor(9,1);  
    lcd.print(Kp);
    lcd.print("             ");



}

void integralScreen(){ 
    
     calculateValvePosition(); 
  
  if (backButtonState == 1){       // integralScreen    integralScreen    integralScreen    integralScreen    integralScreen    integralScreen        
        menuSelected =4;
        }
  if (upButtonState == 1){
        if( Ki >= 1){
        Ki = Ki + .05;}
        }
  if (downButtonState == 1){
        if( Ki < 10){
        Ki = Ki - .05;}
        }
  if (enterButtonState == 1){   
         menuSelected =6;
        }
 
    lcd.setCursor(0,0);
    lcd.print("    SET INTEGRAL    ");
    lcd.setCursor(0,1);
    lcd.print("      Ki:");
    lcd.setCursor(9,1);  
    lcd.print(Ki);
    lcd.print("        ");
    lcd.setCursor(0, 2);
    lcd.print("                    ");  
    lcd.setCursor(0,3);
    lcd.print("                    "); 

}
void exhaustbiasScreen(){ 
    
     calculateValvePosition();
  
  if (backButtonState == 1){       // exhaustbiasScreen    exhaustbiasScreen   exhaustbiasScreen   exhaustbiasScreen   exhaustbiasScreen      
        menuSelected =5;
        }
  if (upButtonState == 1){
        if( exhaustBias <= 95){
        exhaustBias = exhaustBias + 1;}
        }
  if (downButtonState == 1){
        if( exhaustBias > 0){
        exhaustBias = exhaustBias - 1;}
        }
  if (enterButtonState == 1){   
         menuSelected =7;
        }
 
    lcd.setCursor(0,0);
    lcd.print("    EXHAUST BIAS    ");
    lcd.setCursor(0,1);
    lcd.print("     eBias:");
    lcd.setCursor(11,1);  
    lcd.print(exhaustBias);
    lcd.print("      ");
    lcd.print("     ");    
    lcd.setCursor(0, 2);
    lcd.print("                    ");  
    lcd.setCursor(0,3);
    lcd.print("                    "); 

}

void fanbiasScreen(){ 
    
     calculateValvePosition();
  
  if (backButtonState == 1){       // exhaustbiasScreen    exhaustbiasScreen   exhaustbiasScreen   exhaustbiasScreen   exhaustbiasScreen      
        menuSelected =6;
        }
  if (upButtonState == 1){
        if( fanBias <= 95){
        fanBias = fanBias + 1;}
        }
  if (downButtonState == 1){
        if( fanBias > 0){
        fanBias = fanBias - 1;}
        }
  if (enterButtonState == 1){   
         menuSelected =8;
        }
 
    lcd.setCursor(0,0);
    lcd.print("    FAN BIAS    ");
    lcd.setCursor(0,1);
    lcd.print("     fBias:");
    lcd.setCursor(11,1);  
    lcd.print(fanBias);
    lcd.print("      ");
    lcd.print("     ");    
    lcd.setCursor(0, 2);
    lcd.print("                    ");  
    lcd.setCursor(0,3);
    lcd.print("                    "); 

}
void servoScreen(){ 

  intakePosition_percent = manualServo_percent;
  exhaustPosition_percent = manualServo_percent;  
  
  if (backButtonState == 1){       // Servos   Servos   Servos    Servos   Servos   Servos    Servos   Servos   Servos    
        menuSelected =7;
        }
  if (upButtonState == 1){
        if( manualServo_percent >= 1){
        manualServo_percent++;}
        }
  if (downButtonState == 1){
        if( manualServo_percent < 100){
        manualServo_percent--;}
        }
  if (enterButtonState == 1){   
         menuSelected =1;
        }
 
    lcd.setCursor(0,0);
    lcd.print("     MANUAL MODE    ");
    lcd.setCursor(0,1);
    lcd.print("     man%");
    lcd.setCursor(9,1);  
    lcd.print(manualServo_percent);
    lcd.print("             ");
    lcd.setCursor(0,2);
    lcd.print("iraw:");
    lcd.setCursor(4,2);  
    lcd.print(intakePosition_raw);
    lcd.print("    ");
    lcd.setCursor(10,2);
    lcd.print("eraw:");
    lcd.setCursor(14,2);  
    lcd.print(exhaustPosition_raw);
    lcd.print("    "); 

}
  
void readRTDs(){

  static struct var_max31865 RTD_CH0;
  static struct var_max31865 RTD_CH1;
  double tmp;
  
  RTD_CH0.RTD_type = 1;                         // un-comment for PT100 RTD

  RTD_CH1.RTD_type = 1;                         // un-comment for PT100 RTD

  
  struct var_max31865 *rtd_ptr;
  rtd_ptr = &RTD_CH0;
  rtd_ch0.MAX31865_full_read(rtd_ptr);          // Update MAX31855 readings 
  
  rtd_ptr = &RTD_CH1;
  rtd_ch1.MAX31865_full_read(rtd_ptr);          // Update MAX31855 readings 

   smokeTemp = ((double)RTD_CH0.rtd_res_raw / 32) - 256;                                                                                              // smokeTemp Calculated
   smokeTemp = smokeTemp * 29/16 + 32;                          

    meatTemp = ((double)RTD_CH1.rtd_res_raw / 32) - 256;                                                                                              //MeatTemp Calculated
    meatTemp = meatTemp * 29/16 + 32;
}  

void calculateValvePosition(){
  intakePosition_percent = airFlow_percent;
  if (smokeTemp <= 175){
        exhaustPosition_percent = airFlow_percent; 
        
     if (intakePosition_percent >= 25){
         fanSpeed = intakePosition_percent + 25;}
     else {
         fanSpeed = 0;}
                
  }
  else {
        exhaustPosition_percent = airFlow_percent - exhaustBias;
       
       if (intakePosition_percent >= 25){
           fanSpeed = intakePosition_percent + fanBias;}   /*changed Sun Nov 20, 2016; fanBias adds not subracts speed*/ 
       else {
         fanSpeed = 0;}
       }
       
  if (exhaustPosition_percent <= 0){
      exhaustPosition_percent = 0;}
  if (fanSpeed <= 0){
      fanSpeed = 0;}
} 
