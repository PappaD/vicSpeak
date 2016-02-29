# vicSpeak
Report the status of the battery bank from the Victron battery monitor to thingspeak. Hardware needed is ESP8266 module and 3.3 volt regulator.
Nothing fancy in schematics, just get a ESP8266 running, connect GPIO16 to RESET so it can wake up from deep sleep, connect Victron TX+GND to RX+GND on ESP.

Im using BMV-702 and a simple 3g/wifi router to make my yacht report battery status.

Example thingspeak page: https://thingspeak.com/channels/91575

Connector on BMV-702 (VE.Direct) is JST-PH: https://www.sparkfun.com/products/9916
Pinout on VE.Direct:
1. 3.3 volt (I dont dare using this to power the ESP8266, tell me if you tried! :-)
2. TX
3. RX
4. GND

