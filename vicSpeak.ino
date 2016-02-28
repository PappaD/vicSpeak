#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>


ESP8266WiFiMulti WiFiMulti;

#define APIKEY "xxx" /* Insert API key from thingspeak here */
#define WIFI "xxx" /* Your SSID */
#define PASSWORD "xxx" /* Your WiFi password */

#define SLEEPTIME 3600


#define PID 0
#define V 1
#define VS 2
#define I 3
#define P 4
#define CE 5
#define SOC 6
#define TTG 7
#define Alarm 8
#define Relay 9
#define AR 10
#define BMV 11
#define FW 12
#define Checksum 13
#define H1 14
#define H2 15
#define H3 16
#define H4 17
#define H5 18
#define H6 19
#define H7 20
#define H8 21
#define H9 22
#define H10 23
#define H11 24
#define H12 25
#define H15 26
#define H16 27
#define H17 28
#define H18 29


#define KEYWORDS 30

String keywords[KEYWORDS] = {
  "PID ", 
  "V ", 
  "VS ", 
  "I ", 
  "P ", 
  "CE ", 
  "SOC ", 
  "TTG ", 
  "Alarm ", 
  "Relay ", 
  "AR ", 
  "BMV ", 
  "FW ", 
  "Checksum ", 
  "H1 ", 
  "H2 ", 
  "H3 ", 
  "H4 ", 
  "H5 ", 
  "H6 ", 
  "H7 ", 
  "H8 ", 
  "H9 ", 
  "H10 ", 
  "H11 ", 
  "H12 ", 
  "H15 ", 
  "H16 ", 
  "H17 ", 
  "H18 "
};

String values[KEYWORDS];
int timeouts = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);

  // Initialize with empty strings
  for(int i=0;i<KEYWORDS;i++)
  {
    values[i] = "";
  }

}


/*
V = Main bank millivolt
VS = Secondary bank millivolt
I = Milliampere currently consuming
P = Watts consuming
CE = mAh that has been consumed
SOC = Percent of main bank left
TTG = Time to go = minutes
*/

void deepsleep()
{
  Serial.println("Deep sleeping");
  
  ESP.deepSleep(SLEEPTIME * 1000000L);   
}



void callURL(String host, String uri)
{
  WiFiMulti.addAP(WIFI, PASSWORD);
  int tries = 0;
  while(tries++ < 10) /* If we cannot connect in 10 seconds, just abort and sleep */
  {
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;
        
        Serial.print("http begin... URI is ");
        Serial.println(uri);
        http.begin(host, 80, uri); 

        int httpCode = http.GET();
        if(httpCode) {
            Serial.printf("http GET... code: %d\n", httpCode);

            if(httpCode == 200) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.print("http GET... failed, no connection or no HTTP server\n");
        }
        return;
    }
    Serial.println("Waiting for wifi");

    delay(1000);
  }
  Serial.println("Wanted to report, but got no WiFi connection");
}


void loop() {



  // If we have all data, send it to thingspeak
  if(values[V].length() && 
     values[VS].length() && 
     values[CE].length() &&
     values[SOC].length())
  {
    // Send info to thingspeak
    Serial.println("Sending to thingspeak, then deep sleep");

    double v = values[V].toInt() / 1000.0;
    double vs = values[VS].toInt() / 1000.0;
    double ce = values[CE].toInt() / 1000.0;
    double soc = values[SOC].toInt() / 10.0;

    String uri = String("/update?api_key=") + APIKEY + "&field1=" + String(v) + "&field2=" + String(vs) + "&field3=" + String(ce) + "&field4=" + String(soc);

    callURL("api.thingspeak.com", uri);
    deepsleep();
  }


  // If we dont have all the data, keep looking for it
  String s = Serial.readString();
  if(s.length()) {
    for(int i=0;i<KEYWORDS;i++)
    {
      String k = keywords[i];

      if(s.substring(0,k.length()) == k)
      {
        String t = s.substring(k.length()+1);
        t.trim();
        
        Serial.println("Got " + t + " as value for " + k + ". Saving it!");
        values[i] = t;
        timeouts = 0;
      }
      

    }
  } else {
    Serial.println("Timeout, strange!");

    // If we have more than 10 timeouts, just go to bed...
    if(timeouts++ > 10)
    {
      Serial.println("Going to bed, too many timeouts. Are we even connected to the victron monitor?");
      deepsleep();
    } 
  }
}
