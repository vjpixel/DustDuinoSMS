// DustDuinoSMS 0.1.3
// Last Updated 4 November 2015
//
// Written VJ pixel, based on code by Matthew Schroyer and others
//
//888888888888888888888888888888888888888888888888888888888888888888888888888888888888

// CONFIGURE HERE
unsigned long sampletime_ms = 3600000; // = 60m * 60s * 1000ms | time interval for posting
char remoteNum[] = "000000000000"; // number of the FrontlineCloud phone

//888888888888888888888888888888888888888888888888888888888888888888888888888888888888

// Include the GSM library
#include <GSM.h>

// initialize the library instance
GSM gsmAccess;
GSM_SMS sms;

String txtMsg = ""; // data send by SMS
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

float ratioP1 = 0;
float ratioP2 = 0;
float countP1;
float countP2;


void setup(){

  // connected in UART
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
  txtMsg = "DustDuinoGSM #01 on";
  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS();

}


void loop()
{

  digitalWrite(7, HIGH);

  valP1 = digitalRead(1); // reads PM 10
  valP2 = digitalRead(0); // reads PM 2.5

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

    // Generates PM10 and PM2.5 count from LPO. Derived from code created by Chris Nafis
    // http://www.howmuchsnow.com/arduino/airquality/grovedust/

    ratioP1 = durationP1/(sampletime_ms*10.0);
    ratioP2 = durationP2/(sampletime_ms*10.0);
    countP1 = 1.1*pow(ratioP1,3)-3.8*pow(ratioP1,2)+520*ratioP1+0.62;
    countP2 = 1.1*pow(ratioP2,3)-3.8*pow(ratioP2,2)+520*ratioP2+0.62;
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
    float concLarge = (PM10count)*K*mass10;

    // next, PM2.5 mass concentration algorithm
    double r25 = 0.44*pow(10,-6);
    double vol25 = (4/3)*pi*pow(r25,3);
    double mass25 = density*vol25;
    float concSmall = (PM25count)*K*mass25;

    // convert numbers to string
    String sPM10count = String(PM10count);
    String sPM25count = String(PM25count);
    String sConcLarge = String(concLarge);
    String sConcSmall = String(concSmall);

    txtMsg = "PM10count "+ sPM10count + " PM10 " + sConcLarge + " PM25count " + sPM25count + " PM25 " + sConcSmall;
    sms.beginSMS(remoteNum);
    sms.print(txtMsg);
    sms.endSMS();

    durationP1 = 0;
    durationP2 = 0;
    starttime = millis();
  }

}


