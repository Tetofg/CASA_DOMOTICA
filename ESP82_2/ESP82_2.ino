#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN D8
#define RST_PIN D0
const int PIN_TRIG = 4; // S2
const int PIN_ECHO = 5; // s3
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
// Init array that will store new NUID
byte nuidPICC[4];

const char* ssid = "Roda2";
const char* password = "804010sestO#";

const char* mqtt_server = "192.168.2.170";
WiFiClient espClient;
PubSubClient client(espClient);
long now = millis();
long lastMeasure = 0;

float tiempo;
float distancia;
String strFloat;
char envio[6];
char rf[12];

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


void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("RFIG_ESP8266")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("entrada/ultra");
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



void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  Serial.begin(115200);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  SPI.begin(); // Init SPI bus
 rfid.PCD_Init(); // Init MFRC522
 Serial.println();
 Serial.print(F("Reader :"));
 rfid.PCD_DumpVersionToSerial();
 for (byte i = 0; i < 6; i++) {
   key.keyByte[i] = 0xFF;
 }
 Serial.println();
 Serial.println(F("This code scan the MIFARE Classic NUID."));
 Serial.print(F("Using the following key:"));
 printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop() {
if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Cn");

  now = millis();


  digitalWrite(PIN_TRIG,LOW);
  delayMicroseconds(4);
  if (now - lastMeasure > 1000) {
    lastMeasure = now;
    digitalWrite(PIN_TRIG,HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG,LOW);
  
    tiempo = pulseIn(PIN_ECHO,HIGH);
    distancia = tiempo/58.3;
  
    // Serial.println(distancia);
    strFloat += distancia ;
    strFloat.toCharArray (envio, 6);
    //Serial.println(strFloat);
    Serial.println(envio);
    //Serial.println("\n");   
    //Serial.println(strFloat);
    //Serial.println("\n");
  
    client.publish("entrada/ultra0", envio);
    envio[0] = 0;
    envio[1] = 0;
    envio[2] = 0;
    envio[3] = 0;
    envio[4] = 0;
    envio[5] = 0;
    strFloat = "0";
     // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
 if ( ! rfid.PICC_IsNewCardPresent())
   return;
 // Verify if the NUID has been readed
 if ( ! rfid.PICC_ReadCardSerial())
   return;
 Serial.print(F("PICC type: "));
 MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
 Serial.println(rfid.PICC_GetTypeName(piccType));
 // Check is the PICC of Classic MIFARE type
 if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
     piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
     piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
   Serial.println(F("Your tag is not of type MIFARE Classic."));
   return;
 }
 if (rfid.uid.uidByte[0] != nuidPICC[0] ||
     rfid.uid.uidByte[1] != nuidPICC[1] ||
     rfid.uid.uidByte[2] != nuidPICC[2] ||
     rfid.uid.uidByte[3] != nuidPICC[3] ) {
   Serial.println(F("A new card has been detected."));
   // Store NUID into nuidPICC array
   for (byte i = 0; i < 4; i++) {
     nuidPICC[i] = rfid.uid.uidByte[i];
   }
   Serial.println(F("The NUID tag is:"));
   Serial.print(F("In hex: "));
   printHex(rfid.uid.uidByte, rfid.uid.size);
    String serial = "";
      for (int x = 0; x < rfid.uid.size; x++)
      {
        // If it is less than 10, we add zero
        if (rfid.uid.uidByte[x] < 0x10)
        {
          serial += "0";
        }
        // Transform the byte to hex
        serial += String(rfid.uid.uidByte[x], HEX);
        // Add a hypen
        if (x + 1 != rfid.uid.size)
        {
          serial += "-";
        }
      }
   
   Serial.println();
   Serial.println("Read serial is: " + serial);

    // Serial.println(distancia);
    serial.toCharArray (rf, 12);
    //Serial.println(strFloat);
    Serial.println(rf);
    //Serial.println("\n");   
    //Serial.println(strFloat);
    //Serial.println("\n");
  
   client.publish("control/rfid", rf);
    serial[0] = 0;
    serial[1] = 0;
    serial[2] = 0;
    serial[3] = 0;
    serial[4] = 0;
    serial[5] = 0;
    serial[6] = 0;
    serial[7] = 0;
    serial[8] = 0;
    serial[9] = 0;
    serial[10] = 0;
    serial[11] = 0;
    serial[12] = 0; 
    serial = "0";

   Serial.print(F("In dec: "));
   printDec(rfid.uid.uidByte, rfid.uid.size);
   Serial.println();
 }
 else Serial.println(F("Card read previously."));
 // Halt PICC
 rfid.PICC_HaltA();
 // Stop encryption on PCD
 rfid.PCD_StopCrypto1();
  }
}

void printHex(byte *buffer, byte bufferSize) {
 for (byte i = 0; i < bufferSize; i++) {
   Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   Serial.print(buffer[i], HEX);
 }
}
/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
 for (byte i = 0; i < bufferSize; i++) {
   Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   Serial.print(buffer[i], DEC);
 }
}
