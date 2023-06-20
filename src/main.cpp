#include <string>

// algumas bibliotecas
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <FS.h>
#include <LittleFS.h>

static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

//TODO:Biblioteca do lcd nãao esta funcionando

#define ID "29 C6 12 B4"

//Senha
const char *senha = "zwhy9410";
const char *ssid = "SHINGEN";
const char *mqtt_server = "6ef1a08cd900468db15022a29a20e84b.s2.eu.hivemq.cloud";

//Topics
const char *request_open = "requestOpen";
const char *receive_request = "receiveRequest";

//Client stuff
WiFiClientSecure espClient;
PubSubClient client(espClient);

const char *mqtt_username = "CPS-ATILA";
const char *mqtt_password =  "CPS12345678";

//MQTT Message
String msg = "";
// unsigned long lastMsg = 0;
// #define MSG_BUFFER_SIZE (500)
// char msg[MSG_BUFFER_SIZE];

//Port
int port = 8883;
int value = 0;

//define alguns pinos do esp32 que serao conectados aos modulos e componentes
#define LedVerde 32
#define LedVermelho 25
#define tranca 33
#define buzzer 4
#define SS_PIN 5
#define RST_PIN 15

MFRC522 mfrc522(SS_PIN, RST_PIN);   // define os pinos de controle do modulo de leitura de cartoes RFID
//LiquidCrystal_I2C lcd(0x27, 16, 2); // define informacoes do lcd como o endereço I2C (0x27) e tamanho do mesmo

void setupWifi()
{
  delay(10);
  Serial.print("Connecting to:");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, senha);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("WIFI CONNECTED!");
  Serial.print("IP ADRESS:");
  Serial.print(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  //String incommingMessage = “”;
  for (int i = 0; i < length; i++) msg +=(char)payload[i];
  msg += "Message arrived " + String(topic);
  Serial.println(msg);
  // check for other commands
  /* else if( strcmp(topic,command2_topic) == 0){
  if (incommingMessage.equals(“1”)) { } // do something else
  }
  */
}

void publishMessage(const char* topic, std::string payload , boolean retained){
  if(client.publish(topic, payload.c_str(), true))
  {
    std::string m = "Message publised: " + std::string(topic)+": " + payload;
    Serial.println(m.c_str());
  }
}


void reconnect() {
  // Loop until we’re reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection…");
    String clientId = "ESP8266Client-"; // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) 
    {
      Serial.println("connected");

      client.subscribe(receive_request);   // subscribe the topics here
      //client.subscribe(command2_topic);   // subscribe the topics here
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");   // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  SPI.begin();   // inicia a comunicacao SPI que sera usada para comunicacao com o mudulo RFID
  
  //lcd.begin(16, 2);   // inicia o lcd

  mfrc522.PCD_Init();  //inicia o modulo RFID

  Serial.begin(115200);  // inicia a comunicacao serial com o computador na velocidade de 115200 baud rate
  LittleFS.begin();
  setupWifi();
  //setDateTime();
  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
  //int numCerts = certsotr

  Serial.println("RFID + ESP32");
  Serial.println("Passe alguma tag RFID para verificar o id da mesma.");

  // define alguns pinos como saida
  pinMode(LedVerde, OUTPUT);
  pinMode(LedVermelho, OUTPUT);
  pinMode(tranca, OUTPUT);
  pinMode(buzzer, OUTPUT);  
}

void loop() 
{
  if (!client.connected()) reconnect();
  else
  {
    msg = "";
    publishMessage("Oi", "Ola", false);
    client.loop();
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      return;                 // se nao tiver um cartao para ser lido recomeça o void loop
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;                  //se nao conseguir ler o cartao recomeça o void loop tambem
    }

    String conteudo = "";      // cria uma string

    Serial.print("id da tag :"); //imprime na serial o id do cartao

    for (byte i = 0; i < mfrc522.uid.size; i++)
    {      // faz uma verificacao dos bits da memoria do cartao
      //ambos comandos abaixo vão concatenar as informacoes do cartao...
      //porem os 2 primeiros irao mostrar na serial e os 2 ultimos guardarao os valores na string de conteudo para fazer as verificacoes
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    conteudo.toUpperCase();                      // deixa as letras da string todas maiusculas


    if (conteudo.substring(1) == ID){ // verifica se o ID do cartao lido tem o mesmo ID do cartao que queremos liberar o acesso

        digitalWrite(LedVerde, HIGH);            // ligamos o led verde

        digitalWrite(tranca, HIGH);              //abrimos a tranca por 5 segundos

        for(byte s = 5; s > 0; s--){             //vai informando ao usuario quantos segundos faltao para a tranca ser fechada
        //   lcd.setCursor(8,1);
        //   lcd.print(s);
        delay(1000);
        }

        digitalWrite(tranca, LOW);               // fecha a tranca
        digitalWrite(LedVerde, LOW);             // e desliga o led
        //lcd.clear();                             // limpa os caracteres q estao escritos no lcd

    }else{                                       // caso o cartao lido nao foi registrado

      digitalWrite(LedVermelho, HIGH);           // vamos ligar o led vermelho

      for(byte s = 5; s > 0; s--){               // uma contagem / espera para poder fazer uma nova leitura

          // lcd.clear();                           // limpa as informacoes que estao na tela
          // lcd.home();                            // nota na posicao inicial
          // lcd.print("Acesso negado");            // infoma ao usuario que ele nao tem acesso
          // lcd.setCursor(8,1);                    // coloca o cursor na coluna 8 da linha 2
          // lcd.print(s);                          // informa quantos segundos faltam para pode fazer uma nova leitura

          // faz o buzzer emitir um bip por segundo
            delay(800);
            //digitalWrite(buzzer, HIGH);
            delay(200);
            digitalWrite(buzzer, LOW);
        }
          digitalWrite(LedVermelho, LOW);         // desliga o led vermelho
          //lcd.clear();                            // limpa as informacoes do lcd
      }
 // recomeça
  }
}
