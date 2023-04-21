#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager​


// URL DO MEU BACKEND
const ip = "Seu endereço de IP";
String serverName = "http://"+ip+":8000/leituras/";


// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 20000; // DELAY QUE SERÁ REALIZADO O ENVIO DOS POSTS

void setup() {
  // Serial.begin(115200); 
  Serial.begin(2400);
  pinMode(D4, OUTPUT);
  //declaração do objeto wifiManager
  WiFiManager wifiManager;

  //utilizando esse comando, as configurações são apagadas da memória
  //caso tiver salvo alguma rede para conectar automaticamente, ela é apagada.
  //  wifiManager.resetSettings();

  //cria uma rede de nome EnergiApp
  wifiManager.autoConnect("EnergiApp");
 
  Serial.println("connected...yeey :)");
  digitalWrite(D4, HIGH);
}

void loop() {
  String envio = "";
  int conteudo[50];
  int i = 0;

  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      
      digitalWrite(D4, !digitalRead(D4));


      while(Serial.available() > 0){
          if (Serial.available()){
            int leitura = Serial.read();
            
            //Serial.print(leitura);
            //Serial.print('|');
            conteudo[i] = (leitura);
            //envio = envio + (String(conteudo[i])+"|");
            //Serial.print(envio);
            
            i++;  
          }
        }
        for (int x = 0; x<30; x++){
          envio = envio + (String(conteudo[x])+"|");
          //Serial.print(conteudo[x]);
          //Serial.print(x);
        }

      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Disp", "1");
      int httpResponseCode = http.POST("{\"mac\":\""+WiFi.macAddress()+"\",\"leitura\":\""+envio+"\"}");

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
  
     Serial.println("O que será enviado ao servidor: "+envio);
      digitalWrite(D4, !digitalRead(D4));
      // Free resources
      http.end();
      
    }
    else {
      Serial.println("WiFi Disconnected, trying to connect...");
      WiFiManager wifiManager;
      wifiManager.autoConnect("EnergiApp");
    }
    lastTime = millis();
    
  }


}
