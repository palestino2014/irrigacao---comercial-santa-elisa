#include <SPI.h>
// Biblioteca utilizada para comunicação com o Arduino
#include <Ethernet.h> 
#include <dht.h>
 
#define dht_dpin A1 //Pino DATA do Sensor ligado na porta Analogica A1

dht DHT; //Inicializa o sensor

// variaveis para sensor de umidade do solo
int umidade;
int i;
unsigned long time;

// A linha abaixo permite definir o endereço físico (MAC ADDRESS) da placa...
//de rede.  
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 200 }; // Define o endereço IP.

// Porta onde estará aberta para comunicação Internet e Arduino.
EthernetServer server(80);  
 
String readString;
int Pin = 7; //  Pino digital onde será ligado e desligado o lampada.
int led_vermelho = 6; // Pino digital para o led vermelho
int led_verde = 5; // Pino digital para o led verde


void setup(){
 
  pinMode(Pin, OUTPUT); // Define o Pino 7 como saída.
  pinMode(led_vermelho, OUTPUT);
  pinMode(led_verde, OUTPUT);
  Ethernet.begin(mac, ip); // Chama o MAC e o endereço IP da placa Ethernet.
  //  Inicia o servidor que esta inserido junto a placa Ethernet.
  server.begin(); 
  
  Serial.begin(9600);// inicia leitura no sensor de temp. umidade
  delay(1000);//Aguarda 1 seg antes de acessar as informações do sensor
   
}
 
void loop(){
    
  //Programacao relativa ao sensor de umidade e temperatura para a saida na porta serial 
  DHT.read11(dht_dpin); //Lê as informações do sensor
  Serial.print("Umidade = ");
  Serial.print(DHT.humidity);
  Serial.print(" %  ");
  Serial.print("Temperatura = ");
  Serial.print(DHT.temperature); 
  Serial.println(" Celsius  ");
  
  umidade = analogRead(A0);// entrada para o sensor de umidade do solo
  int Porcento = map(umidade, 1023, 0, 0, 100);// // sensor de umidade do solo
   
  //sensor umidade relativa do solo
  Serial.print("h% (solo) = ");
  Serial.print(Porcento);
  Serial.println(" % ");
  
  time = millis();
  
  Serial.println("TEMPO DE MONITORIAMENTO");
  Serial.print("T(min) = ");
  Serial.println((time/1000)/60);
  Serial.print("T(s) = ");
  Serial.println(time/1000);
  Serial.print("T(ms) = ");
  Serial.println(time);
  Serial.println("=========================");  
  
  //Não diminuir o valor abaixo. O ideal é a leitura a cada 2 segundos
  delay(2000);  
    
   //programcao relativa ao servidor WEB
   EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
       if (readString.length() < 100) {
          readString += c;             
        }

        if (c == '\n') {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
    
           // A partir daqui começa o código html.
    
          client.println("<HTML>");
          client.println("<BODY>");
          client.println("<H1>COMERCIAL - SANTA ELISA</H1>");
                                             
             
             
              client.println("<br />");
          
                  client.println("<a href=\"/facacomarduino/LedOn\"\">DESLIGAR</a>");
                  client.println("<br />");
                  client.println("<hr />");
                  client.println("<a href=\"/facacomarduino/LedOff\"\">LIGAR</a><br />");   
                  client.println("<hr />");
                  client.println("<br />");
                  
                  //Saída relativa a temperatura e umidade do solo
                  client.println(DHT.humidity);
                  client.println("[%] ==> UMIDADE RELATIVA DO AR ");
                  
                  client.println("<hr />");
                  client.println("<br />");
                  
                  client.println(DHT.temperature); 
                  client.println("[C] ==> TEMPERATURA  ");
                  
                  client.println("<hr />");
                  client.println("<br />");
                  
                  client.println(Porcento); 
                  client.println("[%] ==> UMIDADE DO SOLO ");
                  
                  client.println("<hr />");
                  client.println("<br />");
                  
                  client.println("TEMPO DE MONITORIAMENTO");
                  client.println("T(Hora) = ");
                  client.println(((time/1000)/60)/60);
                  client.println("T(min) = ");
                  client.println((time/1000)/60);
                  client.println("T(s) = ");
                  client.println(time/1000);
                  client.println("T(ms) = ");
                  client.println(time);
                             
                  client.println("<hr />");
                  client.println("<br />");
                  
                  client.println("<meta http-equiv=refresh content=15;URL='//192.168.0.200/'>");
                               
             client.println("</BODY>");
             client.println("</HTML>");
          
          delay(1);
          client.stop();
          
          if(readString.indexOf("facacomarduino/LedOn") > 0)
          {
            digitalWrite(Pin, HIGH);  // Liga
            digitalWrite(led_verde, HIGH); 
            digitalWrite(led_vermelho, LOW);
          }
          else {
            if(readString.indexOf("facacomarduino/LedOff") > 0)
            {
              digitalWrite(Pin, LOW);  // Desliga
              digitalWrite(led_vermelho, HIGH);
              digitalWrite(led_verde, LOW);
            }
          }
          readString="";    
        }
      }
    }
  }
  
   
  
}
