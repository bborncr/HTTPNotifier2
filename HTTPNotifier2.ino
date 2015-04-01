/*HTTPNotifier version 2.0
* iBoard + Buck Converter + NeoPixel Ring + Buzzer
 */

#include <SPI.h>        
#include <Ethernet.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#define PIN A0
#define BUZZER_PIN A1
#define NUM_PIXELS 16

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

int ledState = 0;
int red_value = 0;
int green_value = 0;
int blue_value = 0;
long previousMillis = 0;
long previousMillis_BUZ = 0;
boolean buzzerOn = false;
long interval = 500; // sets blink speed
long buzzer_interval = 2000; // buzzer time
int currentColor = 5;
int currentState = 0;
String commandString = "null";

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x86 };//make sure this doens't conflict with anything in your network
byte ip[] = { 
  192, 168, 1, 1 };

EthernetServer server(80);

void setup()
{
  
  pinMode(BUZZER_PIN,OUTPUT);
  Serial.begin(9600);
  Serial.println("Serial connection ready...");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  if(Ethernet.begin(mac) == 0) { //change to Ethernet.begin(mac,ip) for static ip
    Serial.println("Failed to configure Ethernet using DHCP");  
    while(true)   // no point in carrying on, so stay in endless loop:
      ;
  }
  server.begin();
  delay(1000);
  Serial.print("IP Address: ");
  Serial.print((Ethernet.localIP()));
  Serial.println("...ready");
  
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue

}

void loop()
{
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        if( client.find("GET /") ) {  // search for 'GET'
          // find tokens starting with "cmd" and stop on the first blank line
          while(client.findUntil("cmd", "\n\r")){  // search to the end of line for 'cmd'
            int cmd = client.parseInt(); // receive command number

            switch (cmd) {
            case 0 : 
              currentColor=0; 
              currentState=0; 
              commandString = ("Red On"); 
              break;
            case 1 : 
              currentColor=1; 
              currentState=0; 
              commandString = ("Green On"); 
              break;
            case 2 : 
              currentColor=2; 
              currentState=0; 
              commandString = ("Blue On"); 
              break;
            case 3 : 
              currentColor=3; 
              currentState=0; 
              commandString = ("Yellow On"); 
              break;
            case 4 : 
              currentColor=4; 
              currentState=0; 
              commandString = ("White On"); 
              break;
            case 5 : 
              currentColor=5; 
              currentState=0; 
              commandString = ("LEDs Off"); 
              break;
              // case 6 : tone(BUZZER_PIN, 2000, 3000); commandString = ("Buzzer On"); digitalWrite(BUZZER_PIN,HIGH); break;
            case 6 : 
              buzzerOn = true; 
              commandString = ("Buzzer On"); 
              break;
            case 10 : 
              commandString = ("Gridshield HTTP Notifier v1.1"); 
              break;
            default: 
              commandString = ("Unknown"); 
              Serial.println("Unknown command"); 
              break;
            }
          }
        }

        // the findUntil has detected the blank line (a lf followed by cr)
        // so the http request has ended and we can send a reply
        // send a standard http response header
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();

        // Respond with the handled request
        client.print("Command received: "); 
        client.print(commandString);
        client.println("<br />");
        client.println("<br />");

        break;
      }
    }
    // give the web browser time to receive the data
    delay(1);
    client.stop();
    commandString="null";
  }

  updateLED();
  updateBUZZER();

}

void updateLED(){

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == 0)
      ledState = 1;
    else
      ledState = 0;

    // set the LED with the ledState of the variable:
    setLED(currentColor, ledState);


  }
}
void updateBUZZER(){

  unsigned long currentMillis = millis();

  if (!buzzerOn){
    previousMillis_BUZ = currentMillis; // if buzzer is not on reset timer
  } 
  else if(currentMillis - previousMillis_BUZ < buzzer_interval) {
    tone(BUZZER_PIN, 2000, 3000);
  } 
  else {
    buzzerOn=false;
    noTone(BUZZER_PIN);
  }

}

void setLED(int color, int led_state){
  if (led_state==1){

    switch (color) {
    case 0 : 
      red_value=255; 
      green_value=0; 
      blue_value=0; 
      break;
    case 1 : 
      red_value=0; 
      green_value=255; 
      blue_value=0; 
      break;
    case 2 : 
      red_value=0; 
      green_value=0; 
      blue_value=255; 
      break;
    case 3 : 
      red_value=255; 
      green_value=255; 
      blue_value=0; 
      break;
    case 4 : 
      red_value=255; 
      green_value=255; 
      blue_value=255; 
      break;
    case 5 : 
      red_value=0; 
      green_value=0; 
      blue_value=0; 
      break;
    default: 
      Serial.println("Unknown color value"); 
      break;
    }
  }
  else{
    red_value=0; 
    green_value=0; 
    blue_value=0;
  }
  for(int i=0;i<NUM_PIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(i, strip.Color(red_value,green_value,blue_value));

    strip.show();

  }
 
}


