// DustDuinoSMS 0.1.4
// Last Updated 13 November 2015
//
// Written VJ pixel, based on code by Matthew Schroyer and others
//
//888888888888888888888888888888888888888888888888888888888888888888888888888888888888

// CONFIGURE HERE

//  time interval for posting
unsigned long sampletime_ms = 3600000; // = 60m * 60s * 1000ms

// id of the device
String id = "redbull"; 

// number of the FrontlineCloud phone
char remoteNum[] = "011941876615"; 

//888888888888888888888888888888888888888888888888888888888888888888888888888888888888

// Include the GSM library
#include <GSM.h>

// initialize the library instance
GSM gsmAccess;
GSM_SMS sms;

String txtMsg = ""; // data sent by SMS
unsigned long starttime;

unsigned long triggerOnP1;
unsigned long triggerOffP1;
unsigned long pulseLengthP1;
unsigned long durationP1;
boolean valP1 = HIGH;
boolean triggerP1 = false;

unsigned long triggerOnP2;
unsigned long triggerOffP2;
unsigned long pulseLengthP2;
unsigned long durationP2;
boolean valP2 = HIGH;
boolean triggerP2 = false;

// ratio is now inside calcCount
//float ratioP1 = 0;
//float ratioP2 = 0;

float countP1;
float countP2;


void setup()
{

  // turn on GSM shield
  powerUpOrDown();

  // dust sensor connected in UART
  pinMode(1, INPUT);
  pinMode(0, INPUT);

  starttime = millis();

  // connection state
  boolean notConnected = true;

  // Start GSM shield
  while (notConnected)
  {
    if (gsmAccess.begin() == GSM_READY)
      notConnected = false;
    else
    {
      delay(1000);
    }
  }

  // send "I'm on" message
  txtMsg = id + " on";
  sendSMS(txtMsg);

}


void loop()
{

  // just for the DustDuino board
  //digitalWrite(7, HIGH);
  
  // the pins below are inverted
  valP1 = digitalRead(0); // reads PM 10
  valP2 = digitalRead(1); // reads PM 2.5

  if(valP1 == LOW && triggerP1 == false)
  {
    triggerP1 = true;
    triggerOnP1 = micros();
  }

  if (valP1 == HIGH && triggerP1 == true)
  {
    triggerOffP1 = micros();
    pulseLengthP1 = triggerOffP1 - triggerOnP1;
    durationP1 = durationP1 + pulseLengthP1;
    triggerP1 = false;
  }

  if(valP2 == LOW && triggerP2 == false)
  {
    triggerP2 = true;
    triggerOnP2 = micros();
  }

  if (valP2 == HIGH && triggerP2 == true)
  {
    triggerOffP2 = micros();
    pulseLengthP2 = triggerOffP2 - triggerOnP2;
    durationP2 = durationP2 + pulseLengthP2;
    triggerP2 = false;
  }

  // Function creates particle count and mass concentration from PPD-42 low pulse occupancy (LPO)
  if ((millis() - starttime) > sampletime_ms)
  {

    countP1 = calcCount(durationP1);
    countP2 = calcCount(durationP2);
    float PM10count = countP2;
    float PM25count = countP1 - countP2;

    // Assues density, shape, and size of dust to estimate mass concentration from particle
    // count. This method was described in a 2009 paper by Uva, M., Falcone, R., McClellan,
    // A., and Ostapowicz, E.
    // http://wireless.ece.drexel.edu/research/sd_air_quality.pdf

    // begins PM10 mass concentration algorithm
    double r10 = 2.6*pow(10,-6);
    double pi = 3.14159;
    double vol10 = (4/3)*pi*pow(r10,3);
    double density = 1.65*pow(10,12);
    double mass10 = density*vol10;
    double K = 3531.5;
    float PM10conc = (PM10count)*K*mass10;

    // next, PM2.5 mass concentration algorithm
    double r25 = 0.44*pow(10,-6);
    double vol25 = (4/3)*pi*pow(r25,3);
    double mass25 = density*vol25;
    float PM25conc = (PM25count)*K*mass25;

    // convert numbers to string
    String sPM10count = String(PM10count);
    String sPM25count = String(PM25count);
    String sPM10conc = String(PM10conc);
    String sPM25conc = String(PM25conc);

    txtMsg = "PM10count "+ sPM10count + " PM10 " + sPM10conc + " PM25count " + sPM25count + " PM25 " + sPM25conc;
    sendSMS(txtMsg);

    durationP1 = 0;
    durationP2 = 0;
    starttime = millis();
  }
  
}

float calcCount (unsigned long duration)
{

  float ratio = 0;
  float count = 0;

  // Generates PM10 and PM2.5 count from LPO. Derived from code created by Chris Nafis
  // http://www.howmuchsnow.com/arduino/airquality/grovedust/

  ratio = duration/(sampletime_ms*10.0);
  count = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;
  return count;
    
}


void sendSMS(String msg)
{
  
  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS();  

}


void powerUpOrDown()
{
  
  pinMode(6, OUTPUT); 
  digitalWrite(6,LOW);
  delay(1000);
  digitalWrite(6,HIGH);
  delay(2000);
  digitalWrite(6,LOW);
  delay(3000);
  
}

