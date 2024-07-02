
## Projeto IoT para Envio de Leituras do Medidor de Energia para o Servidor Backend

### Descrição do Projeto

Este projeto visa criar um sistema IoT que lê as portas do medidor de energia e envia essas leituras para um servidor backend através de requisições HTTP POST. A comunicação é feita utilizando um módulo ESP8266, que se conecta a uma rede Wi-Fi para transmitir os dados.

### Esquema de Conexão

-   Conecte o pino 13 ao GND.
-   Conecte o pino 15 ao RX. 
- ![Esquema PIMA](https://github.com/jonkstro/energiapp_wifimanager/assets/103316482/d279d10c-deae-4aa9-8ebb-5f25d5e91112)

### Referências

-   [Repositório PIMA](https://github.com/jrbenito/PIMA)

### Código do Programa


    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClient.h>
    #include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
    
    // URL do Servidor Backend
    const char* ip = "Seu endereço de IP";
    String serverName = "http://" + String(ip) + ":8000/leituras/";
    
    // Variáveis de controle de tempo
    unsigned long lastTime = 0;
    unsigned long timerDelay = 20000; // Intervalo de envio das leituras (20 segundos)
    
    void setup() {
      Serial.begin(2400);
      pinMode(D4, OUTPUT);
      
      WiFiManager wifiManager;
      // Reseta as configurações salvas, se necessário
      // wifiManager.resetSettings();
    
      // Cria uma rede Wi-Fi com o nome "EnergiApp"
      wifiManager.autoConnect("EnergiApp");
     
      Serial.println("Conectado...yeey :)");
      digitalWrite(D4, HIGH);
    }
    
    void loop() {
      String envio = "";
      int conteudo[50];
      int i = 0;
    
      // Envia uma requisição HTTP POST a cada intervalo definido
      if ((millis() - lastTime) > timerDelay) {
        // Verifica o status da conexão Wi-Fi
        if(WiFi.status() == WL_CONNECTED) {
          digitalWrite(D4, !digitalRead(D4));
    
          // Lê os dados do medidor de energia
          while(Serial.available() > 0) {
            if (Serial.available()) {
              int leitura = Serial.read();
              conteudo[i] = leitura;
              i++;
            }
          }
    
          for (int x = 0; x < 30; x++) {
            envio = envio + String(conteudo[x]) + "|";
          }
    
          WiFiClient client;
          HTTPClient http;
          
          // Inicializa a conexão HTTP
          http.begin(client, serverName);
          
          // Define os headers da requisição
          http.addHeader("Content-Type", "application/json");
          http.addHeader("Disp", "1");
    
          // Envia a requisição POST
          int httpResponseCode = http.POST("{\"mac\":\"" + WiFi.macAddress() + "\",\"leitura\":\"" + envio + "\"}");
    
          // Exibe o código de resposta HTTP
          Serial.print("Código de resposta HTTP: ");
          Serial.println(httpResponseCode);
          Serial.println("Dados enviados ao servidor: " + envio);
          
          digitalWrite(D4, !digitalRead(D4));
    
          // Libera os recursos
          http.end();
          
        } else {
          Serial.println("WiFi desconectado, tentando reconectar...");
          WiFiManager wifiManager;
          wifiManager.autoConnect("EnergiApp");
        }
        
        lastTime = millis();
      }
    }




### Descrição do Código

1.  **Importação de Bibliotecas:**
    
    -   `ESP8266WiFi.h`: Biblioteca para a conexão Wi-Fi.
    -   `ESP8266HTTPClient.h`: Biblioteca para fazer requisições HTTP.
    -   `WiFiClient.h`: Cliente Wi-Fi.
    -   `WiFiManager.h`: Biblioteca para facilitar a conexão com redes Wi-Fi.
2.  **Configuração Inicial:**
    
    -   Define a URL do servidor backend.
    -   Configura o intervalo de envio das leituras (20 segundos).
3.  **Setup:**
    
    -   Inicializa a comunicação serial com o medidor de energia a 2400 bps.
    -   Configura o pino D4 como saída.
    -   Configura e conecta à rede Wi-Fi usando o WiFiManager.
4.  **Loop:**
    
    -   A cada 20 segundos, verifica se a conexão Wi-Fi está ativa.
    -   Lê os dados do medidor de energia e os armazena em um array.
    -   Constrói a string de dados a ser enviada ao servidor.
    -   Envia os dados ao servidor através de uma requisição HTTP POST.
    -   Exibe o código de resposta HTTP e os dados enviados no monitor serial.
    -   Reestabelece a conexão Wi-Fi, se necessário.

### Desafios e Considerações

-   **Estabilidade da Conexão Wi-Fi:** É crucial garantir que o dispositivo esteja sempre conectado à rede para evitar perda de dados.
-   **Segurança:** Implementar criptografia para proteger os dados durante a transmissão.
-   **Tratamento de Erros:** Adicionar mecanismos para lidar com falhas na comunicação e reconectar automaticamente.
