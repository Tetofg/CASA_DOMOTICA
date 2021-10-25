#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Roda2";
const char* password = "804010sestO#";

const char* mqtt_server = "192.168.2.170";

WiFiClient espClient;
PubSubClient client(espClient);
int periodo = 1000;
String strFloat;
char envio[6];
int analog = A0;
int rele1 = D5;
int rele2 = D6;
int rele3 = D7;
int rele4 = D8;
int led1 = 5;
int led2 = 4;
int led3 = 0;
int led4 = 2;


unsigned long tiempoAnterior=0;

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

// Recibir
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

  if(topic=="casa/cuarto1"){
      if(messageTemp == "on"){
        digitalWrite(rele1, HIGH);
        Serial.print("Rele 1 On");
      }
      else if(messageTemp == "off"){
        digitalWrite(rele1, LOW);
        Serial.print("Rele 1 Off");
      }
  }
    if(topic=="casa/cuarto2"){
      if(messageTemp == "on"){
        digitalWrite(rele4, HIGH);
        Serial.print("Rele 4 On");
      }
      else if(messageTemp == "off"){
        digitalWrite(rele4, LOW);
        Serial.print("Rele 4 Off");
      }
  }

    if(topic=="porton/rfig"){
      if(messageTemp == "on"){
        digitalWrite(rele3, LOW);
        delay(500);
        digitalWrite(rele3,HIGH);
        delay(2500);
        Serial.print("Rele 3 On");
        digitalWrite(rele2, LOW);
        delay(500);
        digitalWrite(rele2,HIGH);
        Serial.print("Rele 2 On");
      }
  }



    if(topic=="casa/entrada"){
      Serial.println("Encendiendo luz casa");
      if(messageTemp == "on"){
        digitalWrite(led1, HIGH);
        Serial.print("Led 1 On");
      }
      else if(messageTemp == "off"){
        digitalWrite(led1, LOW);
        Serial.print("Led 1 Off");
      }
  }

if(topic=="casa/cuarto5"){
      Serial.println("Encendiendo luz casa");
      if(messageTemp == "on"){
        digitalWrite(led2, HIGH);
        Serial.print("Led 2 On");
      }
      else if(messageTemp == "off"){
        digitalWrite(led2, LOW);
        Serial.print("Led 2 Off");
      }
  }

if(topic=="casa/cuarto3"){
      Serial.println("Encendiendo luz casa");
      if(messageTemp == "on"){
        digitalWrite(led3, HIGH);
        Serial.print("Led 3 On");
      }
      else if(messageTemp == "off"){
        digitalWrite(led3, LOW);
        Serial.print("Led 3 Off");
      }
  }

  if(topic=="casa/vestibulo"){
      Serial.println("Encendiendo luz casa");
      if(messageTemp == "on"){
        digitalWrite(led4, HIGH);
        Serial.print("Led 4 On");
      }
      else if(messageTemp == "off"){
        digitalWrite(led4, LOW);
        Serial.print("Led 4 Off");
      }
  }



}


// This functions reconnects your ESP8266 to your MQTT broker 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("RELE_ES8266")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example);
      client.subscribe("casa/entrada");
      client.subscribe("casa/cuarto5");
      client.subscribe("casa/cuarto3");
      client.subscribe("casa/vestibulo");
      client.subscribe("casa/cuarto4");
      client.subscribe("casa/cuarto1");
      client.subscribe("casa/cuarto2");
      client.subscribe("porton/rfig");
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
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
  pinMode(rele4, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  digitalWrite(rele1,HIGH);
  digitalWrite(rele2,HIGH);
  digitalWrite(rele1,HIGH);
  digitalWrite(rele4,HIGH);
  Serial.begin(115200);  
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
    client.connect("ESP8266");

  if(millis()- tiempoAnterior>=periodo){
      int valor = analogRead(A0);
      float voltaje = valor*(5.0/1023.0);
      float porcentaje = voltaje*(100/5);
      strFloat += porcentaje;
      Serial.println(voltaje);
      Serial.println(porcentaje);
      strFloat.toCharArray (envio, 6);
      client.publish("calle/luz", envio);
      envio[0] = 0;
    envio[1] = 0;
    envio[2] = 0;
    envio[3] = 0;
    envio[4] = 0;
    envio[5] = 0;
    envio[6] = 0;
    strFloat = "0";
      tiempoAnterior=millis();
      }
 
} 
