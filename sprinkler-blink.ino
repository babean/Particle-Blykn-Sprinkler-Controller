// This #include statement was automatically added by the Particle IDE.
#include "blynk/blynk.h"

#define minute_slider V0
#define seconds_cntr  V1
#define RELAY1 D0
#define RELAY2 D1
#define RELAY3 D2
#define RELAY4 D3
#define led D7

//App dashboard setup:
//   Slider widget (0...10) on V0 Watering Minutes hand
//   Dial widget on V1... Watering Seconds hand
//   Button widget on V10... Relay 1
//   Button widget on V11... Relay 2
//   Button widget on V12... Relay 3
//   Button widget on V13... Relay 4
//   Button widget on V31... Push widget settings to hardware
//


//STARTUP(WiFi.selectAntenna(ANT_EXTERNAL)); // selects the u.FL antenna

char auth[] = "<your_blynk_app_auth_key_here>";
int relay[] = {RELAY1, RELAY2, RELAY3, RELAY4};

bool isFirstConnect = true;
unsigned int minutes;
unsigned int seconds;

byte mac[6];
String mac_addr;

//Initialize a Software timer
Timer sec_timer(1000, update_every_second);

WidgetLCD lcd(V3);

BLYNK_WRITE(V31)
{ 
    Blynk.syncVirtual(V10);
    Blynk.syncVirtual(V11);
    Blynk.syncVirtual(V12);
    Blynk.syncVirtual(V13);
}

BLYNK_CONNECTED() {
  if (isFirstConnect) {
    Blynk.syncAll();
    isFirstConnect=false;
  }
}

BLYNK_WRITE_DEFAULT()
{ 
    digitalWrite(relay[request.pin-10], param.asInt());
    lcd.clear();
    lcd.print(0,0,request.pin);
}

void stop_timers()
{    sec_timer.stop();
}

void heads_off()
{
    stop_timers();
    Blynk.virtualWrite(V10, 0);
    Blynk.virtualWrite(V11, 0);
    Blynk.virtualWrite(V12, 0);
    Blynk.virtualWrite(V13, 0);
    pinResetFast(RELAY1);
    pinResetFast(RELAY2);
    pinResetFast(RELAY3);
    pinResetFast(RELAY4);
}

void update_every_second()
{
    switch (seconds)
    {
    case 0:
        Blynk.virtualWrite(minute_slider, --minutes);
        seconds=59;
        Blynk.virtualWrite(seconds_cntr, 59);
        break;
    case 1:
        if (minutes==0) {
             heads_off();
        }
    default: Blynk.virtualWrite(seconds_cntr, --seconds);
    }
}

void setStationTime(int min_in)
{
    seconds=0;
    Blynk.virtualWrite(seconds_cntr, seconds); // Reset seconds display to 60 seconds
    sec_timer.reset();
}

BLYNK_WRITE(V0)
{ 
  minutes = param.asInt(); // Get value as integer
  if (minutes>0) {
      setStationTime(minutes);
  }
  else {
      Blynk.virtualWrite(seconds_cntr, 0); // Reset seconds display to 0 seconds
      heads_off();
  }
}


void setup()
{
    Blynk.begin(auth);

    //Initilize  relay control pins as output
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    pinMode(RELAY3, OUTPUT);
    pinMode(RELAY4, OUTPUT);
    pinMode(led, OUTPUT);

    // Initialize relays to an OFF state
    pinResetFast(RELAY1);
    pinResetFast(RELAY2);
    pinResetFast(RELAY3);
    pinResetFast(RELAY4);
    pinResetFast(led);
 
    WiFi.macAddress(mac);
    mac_addr=String(mac[0],HEX);
    
    for (int i=1;i<6;i++){
        mac_addr.concat(":");
        mac_addr.concat(String(mac[i],HEX));
    }
    Particle.variable("mac_addr", mac_addr);
}

void loop()
{ 
  Blynk.run();
}
