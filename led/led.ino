#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>


// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "Roda2";
const char* password = "804010sestO#";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "192.168.2.170";

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);

// DHT Sensor - GPIO 5 = D1 on ESP-12E NodeMCU board
const int ONE_WIRE_BUS = 5;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

int Status = 12;  // Digital pin D6
int sens = 13;  // Digital pin D7
int periodo = 3000;
int periodo1 = 1000;
unsigned long tiempoAnterior=0;

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("TEMP_ES8266")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {

  Serial.begin(115200);
  Serial.print("t (s)\t");
  Serial.println("T (grados C)");
  Serial.println("===================");
  pinMode(sens, INPUT);   // declare sensor as input
  pinMode(Status, OUTPUT);  // declare LED as output
  sensor.begin();
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

// For this project, you don't need to change anything in the loop function. Basically it ensures that you ESP is connected to your broker
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");

  now = millis();
  if (now - lastMeasure > 1000) {
    lastMeasure = now;
    sensor.requestTemperatures();
    float temperatura = sensor.getTempCByIndex(0);
    if (isnan(temperatura)) {
      Serial.println("Failed to read sensor!");
      return;
    }

    static char temperatureTemp[7];
    dtostrf(temperatura, 6, 2, temperatureTemp);
    client.publish("room/temperature", temperatureTemp);
    Serial.print(" %\t Temperature: ");
    Serial.print(temperatura);
    Serial.print(" *C ");

  }

  long state = digitalRead(sens);
    if(state == HIGH) {
      if(millis()- tiempoAnterior>=periodo){
      digitalWrite (Status, HIGH);
      Serial.println("Motion detected!");
      client.publish("calle/pir", "on");
      tiempoAnterior=millis();
      }
    }
    else {
      if(millis()- tiempoAnterior>=periodo1){
      digitalWrite (Status, LOW);
      Serial.println("Motion absent!");
      client.publish("calle/pir", "off");
      tiempoAnterior=millis();
      }
    }
} 
