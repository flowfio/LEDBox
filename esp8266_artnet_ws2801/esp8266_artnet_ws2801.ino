#define  FASTLED_ESP8266_RAW_PIN_ORDER
#include "FastLED.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define ARTNET_ARTDMX 0x5000 //20480 as a decimal ArtNet OP Code
#define DATA_PIN 0
#define CLOCK_PIN 2 
#define NUM_LEDS 10
#define BRIGHTNESS 255
CRGB leds[NUM_LEDS];

//WiFi Setup
char ssid[] = "Kontor Hamburg";
char pass[] = "WikipediA";

IPAddress local_ip(192, 168, 1, 20);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
#define ART_NET_PORT 6454
WiFiUDP Audp; //UDP socket for ArtNet
#define BUFFER_SIZE 640
unsigned char PacketBuffer[BUFFER_SIZE]; //buffer to hold incoming packet data

void setup() {

  // init LEDs
  //FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );

  // init wifi
  WiFi.begin(ssid, pass); //Connect to Network
  WiFi.config(local_ip, gateway, subnet); //Set static IP information
  
  while (WiFi.status() != WL_CONNECTED)  //wait until we are connected
  {
     // Turn the LED on, then pause
    leds[0] = CRGB::Blue;
    leds[1] = CRGB::Blue;
    leds[2] = CRGB::Blue;
    leds[3] = CRGB::Blue;
    FastLED.show();
    delay(500);
    
    // Now turn the LED off, then pause
    leds[0] = CRGB::Black;
    leds[1] = CRGB::Black;
    leds[2] = CRGB::Black;
    leds[3] = CRGB::Black;
    FastLED.show();
    delay(500);
  }

  // init artnet port
  Audp.begin(ART_NET_PORT); //Open ArtNet port

  // signal ready
  leds[0] = CRGB::Green;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Green;
  leds[3] = CRGB::Green;
  FastLED.show();
}

// the loop function runs over and over again forever
void loop() {
    int packetSize = Audp.parsePacket(); //Parse Packet
    
    if ( packetSize ) //ArtNet packet size is 530
    {

      Audp.read(PacketBuffer, BUFFER_SIZE); //Read packet into buffer
      int opcode = artNetOpCode(PacketBuffer); //Check to see if packet is ArtNet
      if ( opcode == ARTNET_ARTDMX ) //If code returned is correct
      {
        for(int i=0; i<NUM_LEDS; i++) {
          int d = i*3+18;
          leds[i].r = PacketBuffer[d];
          leds[i].g = PacketBuffer[d+1];
          leds[i].b = PacketBuffer[d+2];
        }

        FastLED.show();
      }
    }
}
int artNetOpCode(unsigned char* pbuff) {
  String test = String((char*)pbuff);
  if ( test.equals("Art-Net") )
  {
    if ( pbuff[11] >= 14 ) 
    {            //protocol version [10] hi byte [11] lo byte
      return pbuff[9] *256 + pbuff[8];  //opcode lo byte first
    }
  }
  return 0;
}
