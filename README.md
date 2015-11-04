# DustDuino

An open-source, project to develop and deploy low-cost air quality monitors worldwide. The DustDuino reads 
particulate matter 10 micrometers or less in diameter (PM10), and particulate matter 2.5 micrometers or less in 
diameter (PM2.5), returning both.

Read more about DustDuino: http://publiclab.org/wiki/dustduino

You can find code for other versions of DustDuino on http://dustduino.org/

This is the Arduino code for sending SMS with the readings of the equipment.

IMPORTANT: This code don't run in Arduino IDE 1.0.5. It was compiled using IDE 1.6.5.

Hardware used:
- Arduino UNO
- SD Card Shield Seeed Studio: http://www.seeedstudio.com/wiki/SD_Card_Shield 
- GSM Shield: http://www.elecfreaks.com/wiki/index.php?title=EFCom_GPRS/GSM_Shield
- Dust sensor Shinyei PPD42: http://www.seeedstudio.com/wiki/Grove_-_Dust_Sensor
