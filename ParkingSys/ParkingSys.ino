#include <ESP8266WiFi.h>                  //libraries 
#include "Adafruit_MQTT.h"                //used 
#include "Adafruit_MQTT_Client.h"         //in 
#include <Servo.h>                        //code

Servo myservo; 


byte a = 15;                             //select pin A of MUX
byte b = 13;                             //Select pin B of MUX
byte c = 12;                             //select pin C of MUX
byte com = 14;                           
byte IR = 0;
unsigned long echo = 0;
unsigned long ultrasoundValue = 0;

#define TRIGGER_PIN 5
#define ECHO_PIN 14


#define WLAN_SSID       ""
#define WLAN_PASS       ""

#define AIO_SERVER      "broker.hivemq.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/sanidhya/parking");



void setup() {
  Serial.begin(115200);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(com, INPUT_PULLUP);
  pinMode(IR, INPUT);
  myservo.attach(4);
  pinMode(TRIGGER_PIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output   
  pinMode(ECHO_PIN,INPUT);
  myservo.write(0);
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

}


int u1=0,u2=0,u3=0,u4=0;
void loop() {
  MQTT_connect();
  setMUX(1);
  delay(20);
  u1 = ping1();
  if(u1 > 15)
  {
    u1 = 1;
  }
  else
  {
    u1 =0;  
  }
  Serial.print("\tping1 = " + String(u1));
  
  
  setMUX(2);
  delay(20);
  u2 = ping1();
  if(u2 > 15)
  {
    u2 = 1;
  }
  else
  {
    u2 =0;  
  }
  Serial.print("\tping2 = " + String(u2));
  
  setMUX(3);
  delay(20);
  u3 = ping1();
  if(u3 > 15)
  {
    u3 = 1;
  }
  else
  {
    u3 =0;  
  }
  Serial.print("\tping3 = " + String(u3));
  
  setMUX(4);
  delay(20);
  u4 = ping1();
  if(u4 > 15)
  {
    u4 = 1;
  }
  else
  {
    u4 =0;  
  }
  Serial.print("\tping4 = " + String(u4));
  Serial.println("\tIR = " + String(digitalRead(IR)));

  
  
//  Serial.println("\t4 = " + String(digitalRead(com)));
//  delay(100);
  int pos;


  if(digitalRead(IR) == 0)
  {
    Serial.println("IR detect");
      if(u1 == 0 & u2 == 0 & u3 == 0 & u4 == 0 )
      {
          myservo.write(0);  
      }
      else
      {
          myservo.write(180);  
          delay(2000);
          myservo.write(0);  
      }    
  }



  String msg = "{\"P1\":" + String(u1) + ",\"P2\":" + String(u2) + ",\"P3\":" + String(u3) + ",\"P4\":" + String(u4) + "}";
  int str_len = msg.length() + 1; 
  char char_array[str_len];
  msg.toCharArray(char_array, str_len);
  Serial.println(char_array);
 
  if (! photocell.publish(char_array)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

}


void setMUX(byte number)
{
    if(number == 1)   //channel 7
    {
      digitalWrite(a,1);
      digitalWrite(b,1);
      digitalWrite(c,1);  
    }    
    if(number == 2)  //chennel 6
    {
      digitalWrite(a,0);
      digitalWrite(b,1);
      digitalWrite(c,1);  
    }    
    if(number == 3)  //channel 5
    {
      digitalWrite(a,1);
      digitalWrite(b,0);
      digitalWrite(c,1);  
    }    
    if(number == 4)  //channel 4
    {
      digitalWrite(a,0);
      digitalWrite(b,0);
      digitalWrite(c,1);  
    }    
}



unsigned int ping1()
{
 digitalWrite(TRIGGER_PIN, LOW); // Send low pulse
 delayMicroseconds(2); // Wait for 2 microseconds
 digitalWrite(TRIGGER_PIN, HIGH); // Send high pulse
 delayMicroseconds(5); // Wait for 5 microseconds
 digitalWrite(TRIGGER_PIN, LOW); // Holdoff

 digitalWrite(ECHO_PIN, HIGH); // Turn on pullup resistor
  echo = pulseIn(ECHO_PIN, HIGH); //Listen for echo
  ultrasoundValue = (echo / 58.138); //convert to CM then to inches
 return ultrasoundValue;
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
