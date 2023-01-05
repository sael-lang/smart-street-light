#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "tobby";
const char* password = "ahmad121";
const char* mqtt_server = "91.121.93.94";  // test.mosquitto.org

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
char manual_light;
int Status = D6;  // Digital pin D6
int sensor = D7;  // Digital pin D7
const int trigPin = D4; 
const int echoPin = D5; 
const int buzzer = D8;
short sensorPin = A0;
long duration;
int distance;
int light=1;
 double Temp;
char msg_out[20];
char msg1[20];
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
if ((char)payload[0] == '2') {
  manual_light = '0';
  }
  if ((char)payload[0] == '3') {
  manual_light = '1';
  }
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '0') {
    digitalWrite(Status, LOW);
    light=0;
          sprintf(msg_out, "%d",light);
 client.publish("test/light", msg_out );
  } if ((char)payload[0] == '1') {
    digitalWrite(Status, HIGH); 
    light=1;
          sprintf(msg_out, "%d",light);
 client.publish("test/light", msg_out );
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("device/temp", "MQTT Server is Connected");
      // ... and resubscribe
      client.subscribe("device/led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
double Thermistor(int RawADC) {
   
    Temp = log(10000.0*((1024.0/RawADC-1))); 
    Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
    Temp = Temp - 273.15;            // Convert Kelvin to Celcius
    return Temp;
}

void data(){
    if(manual_light == '0')
  {

  long state = digitalRead(sensor);
    if(state == HIGH) {
      digitalWrite (Status, HIGH);
      Serial.println("Motion detected!");
      delay(1000);
      light=1;
      sprintf(msg_out, "%d",light);
 client.publish("test/light", msg_out );
    }
    else {
      digitalWrite (Status, LOW);
      Serial.println("Motion absent!");
      delay(1000);
      light=0;
      sprintf(msg_out, "%d",light);
 client.publish("test/light", msg_out );
      }
  }

// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

if(distance<5)
{
digitalWrite(buzzer, HIGH);
delay(100);
digitalWrite(buzzer, LOW);
  }
    snprintf (msg, MSG_BUFFER_SIZE, "%d", distance);

    client.publish("device/event", msg);
  }

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(sensor, INPUT);   // declare sensor as input
  pinMode(Status, OUTPUT);  // declare LED as output
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
pinMode(buzzer,OUTPUT);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
   sprintf(msg_out, "%d",light);
 client.publish("test/light", msg_out );
 data();
   int readVal=analogRead(sensorPin);
     char temp = Thermistor(readVal);
      sprintf(msg1, "%d",temp);
 client.publish("test/Temp", msg1);
    delay(1000);
  }
