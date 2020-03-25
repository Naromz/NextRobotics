#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include <Servo.h>

ESP8266WiFiMulti wifiMulti;      

ESP8266WebServer server(80);       
WebSocketsServer webSocket(81);    

File fsUploadFile;                                

const char *ssid = "Team 0 Robot"; 
const char *password = "NextRobotics";  

int LED = D5;


Servo leftservo;
Servo rightservo;

/*__________________________________________________________SETUP__________________________________________________________*/

void setup() {
  Serial.begin(115200);      
  delay(10);
  Serial.println("\r\n");
  startWiFi();             
  startWebSocket();            
   rightservo.attach(D4);
    leftservo.attach(D3);     
    
  startServer(); 
}

/*__________________________________________________________LOOP__________________________________________________________*/

void startServer() { 
  server.on("/", handle_OnConnect);      

  server.begin();                             // start the HTTP server
  Serial.println("HTTP server started.");
}

void handle_OnConnect() {
  server.send(200, "text/html", "<html><body onload='myFunction()'><div style='outline:1px solid black;background-Color:blue;width:200px;height:200px;' onTouchStart='sendMessage(`1`)'onTouchEnd='sendMessage(`3`)'>test</div> <script> var ws = new WebSocket('ws://192.168.1.225:81'); function myFunction() {ws.onopen = () => {console.log('connected')}} function sendMessage(msg) { ws.send(msg) } </script> </body> </html>"); 
  }



void loop()
{
  webSocket.loop();                           
  server.handleClient();     

}

void startWiFi()
{ 
  WiFi.softAP(ssid, password);          
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started\r\n");

  wifiMulti.addAP("Jack Zach and Korynne", "Password1");   
  wifiMulti.addAP("nextfield", "nextrobotics");

  Serial.println("Connecting");
  while (wifiMulti.run() != WL_CONNECTED && WiFi.softAPgetStationNum() < 1) {
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");
  if(WiFi.softAPgetStationNum() == 0) {     
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());             
    Serial.print("IP address:\t");
    Serial.print(WiFi.localIP());          
  } else {                   
    Serial.print("Hosting wifi");
  }
  Serial.println("\r\n");
}

void startWebSocket() { 
  webSocket.begin();                     
  webSocket.onEvent(webSocketEvent);          
  Serial.println("WebSocket server started.");
}




void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) 
{
  switch (type) {
    case WStype_DISCONNECTED:             
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {             
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
          
      }
      break;
    case WStype_TEXT:                   
      Serial.printf("[%u] get Text: %s\n", num, payload);
      String payloadString=(const char *)payload;
      if(payloadString == "LED")
      {
        digitalWrite(LED,!digitalRead(LED));
      }
      if(payloadString == "1")
      {
       leftservo.write(180);  
      }
       if(payloadString == "2")
      {
       rightservo.write(180);  
      }
       if(payloadString == "3")
      {
       leftservo.write(90);  
      }
       if(payloadString == "4")
      {
       rightservo.write(0);  
      }
      break;
  }
}
