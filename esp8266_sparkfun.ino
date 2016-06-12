#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <DallasTemperature.h>
#include <ESP8266HTTPClient.h>

#define ONE_WIRE_BUS D4

#define MAXTEMP 85

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

char temperatureString[6];

/*
 * FILL IN THESE 4 Variables 
 *  SSID and PASSWORD of your wifi access point
 *  Goto data.sparkfun.com and register for a free "endpoint", then take the endpoint device id and private key 
 *  and put them into the respective variables  below.
 */
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

const char* sparkfun_device = "SPARKFUN_DEVICE";
const char* sparkfun_private_key ="SPARKFUN_KEY";

HTTPClient http;

void setup(void)
{
  Serial.begin(9600);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("DS18B20 Temperature Demo");
  DS18B20.begin();
  pinMode(D7,OUTPUT);
  // Blink LED a few times to show things are starting up.
  for (int i=0; i < 15 ; i++) {
    digitalWrite(D7,HIGH);
    delay (100);
    digitalWrite(D7,LOW);
    delay (100);
  }
}

float getTemperature() 
{
  float temp;
  do {
    // Serial.println("Reading Temperature...");
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempFByIndex(0);
    // Serial.println(temp);
    delay(100);
  } while (temp == 85.0 || temp == (-127.0));
  return temp;
}

void loop() 
{
  HTTPClient http;
  char f_str[10];
  static char url[100];
  float temperature = getTemperature();
  static int counter=0;
  
  dtostrf(temperature, 2, 2, temperatureString);
  Serial.println(temperatureString);
  digitalWrite(D7,HIGH);
  delay(1000);
  // Turn off LED when temp < MAXTEMP (all ok). I.e. Blinking effect
  // If temp >= MAXTEMP then LED will stay on to signify threshold reached
  if (temperature < MAXTEMP) {
    digitalWrite(D7,LOW);
  }

  if ((counter % 60) == 0) {
    dtostrf(temperature, 4, 2, f_str);
    sprintf(url, "http://data.sparkfun.com/input/%s?private_key=%s&temp=%s", sparkfun_device, sparkfun_private_key, f_str);
    Serial.println(url);
    http.begin(url);
    int httpcode = http.GET();
    Serial.println(httpcode);
    http.end();
  }
  counter++;
  
}
