/*
Projeto de Iniciação científica - Reator para síntese de material por rota de combustão.
Autor: Marcus Vinicius Vieira dos Santos
Setembro de 2024 - Agosto de 2025
*/

// --- Incluindo Bibliotecas ------------------------
#include <WiFi.h>
#include <PubSubClient.h>
#include "Passwords_Logins.h"

// --- Portas do ESP32 --------------------------------
#define ledWifi 23
#define ledBroker 21
#define turnOnOff 12

// --- Topicos MQTT ----------------------------------
//Dados vindo do aparelho mobile (SUB)
#define TOPICO_SUB_TURNON "topic_mqtt_powerOnOff_reator"
#define TOPICO_SUB_TEMPALVO "topic_mqtt_temperaturaalvo_reator"

//Dados enviados ao aparelho mobile (PUB)
#define TOPICO_PUB_TEMPCOMPOSTO "topic_mqtt_tempcomposto_reator"
#define TOPICO_PUB_TEMPRESISTENCIA "topic_mqtt_tempresistencia_reator"
//-----------------------------------------------------

// --- Define o id ESP32 para o  mqttBroker-----------
#define id_mqtt "esp-reator_mqtt"

// --- Dados da rede WIFI e do Broker -----------------
const char* ssid = WEB_SSID;
const char* password = WEB_PASSWORD;
const char* broker_mqtt = BROKER_MQTT;
int broker_port = 1883;

// --- variáveis globais -----------------------------
WiFiClient espClient;
PubSubClient MQTT(espClient);


// --- Prototypes -------------------------------------
float temperaturaComposto(void);
float temperaturaResistencia(void);
float controlPI(void);
float controlCoolers(void);
void initWifi(void);
void initMqtt(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnectWifi(void);
void reconnectmqtt(void);
void verifywifi_mqtt(void);
void OnledWifi(void);
void OnledMqtt(void);

// --- Conectando no WIFI ----------------------------
void initWifi(void){
  delay(10);
  Serial.println("--------Conexão Wifi--------");
  Serial.println("Conectando-se a rede: ");
  Serial.println(ssid);
  Serial.println("Aguarde.");
  reconnectWifi();
}

void reconnectWifi(void){
  if(WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado com sucesso.");
  OnledWifi();
  Serial.println("IP obtido:");
  Serial.println(WiFi.localIP());
}

// --- Conectando no MQTTBroker --------------------
void initMqtt(void){
  MQTT.setServer(broker_mqtt,broker_port); // connecting to broker;
  MQTT.setCallback(mqtt_callback);
// função de callback é chamada toda vez que uma informação de um dos tópicos subescritos chega informação deles;
}

void reconnectmqtt(void){
 while(!MQTT.connected()){
   Serial.print("* Tentando se conectar ao Broker MQTT: ");
   Serial.println(broker_mqtt);
   if(MQTT.connect(id_mqtt)){
     Serial.println("Conectado com sucesso ao broker MQTT!");
     OnledMqtt();

     //subscribe nos tópicos para as informações serem recebidas
      MQTT.subscribe(TOPICO_SUB_TURNON); 
      MQTT.subscribe(TOPICO_SUB_TEMPALVO);
      MQTT.subscribe("topic_mqtt_testS"); 
   }else{
     OnledMqtt();
     Serial.println("Falha ao reconectar no broker.");
     Serial.println("Havera nova tentatica de conexao em 2s");
     delay(2000);
   }
 }
}

// --- Receber informação do celular ----------------------------------
void mqtt_callback(char* topic, byte* payload, unsigned int length){
  String msg;
  
  for(int i = 0; i<length; i++){
    char c = (char)payload[i];
    msg += c;
  }

  Serial.print("Chegou a seguinte string via MQTT: ");
  Serial.println(msg);
  String topico = topic;
  // action about sign;

  // --- recebe o sinal do dashboard ---------------
  // --- Iniciar Sistema ---------------------------
  if(topico.equals(TOPICO_SUB_TURNON)){
    if(msg.equals("1")){ 
    digitalWrite(turnOnOff,1);
    Serial.println("Sistema iniciado mediante comando MQTT");

    }else{
      digitalWrite(turnOnOff,0);
      Serial.println("Sistema desligado mediante comando MQTT");
    }
  }

  // --- Temperatura Set Point -----------------------
  if(topico.equals(TOPICO_SUB_TEMPALVO)){
    Serial.print("A Temperatura a ser atingida é: ");
    Serial.println(msg);
  }
  
}

// --- Verificar se as conexões estão mantidas -----------
void verifywifi_mqtt(void){
  if(!MQTT.connected() && WiFi.status() != WL_CONNECTED){
    OnledMqtt();
    OnledWifi();
  }
  if(!MQTT.connected()){
    OnledMqtt();
    reconnectmqtt(); 
  }
  if(WiFi.status() != WL_CONNECTED){
    OnledWifi();
    reconnectWifi();
  }
}

// --- LED's para conferir as conexões -------------
void OnledWifi(void){
  if(WiFi.status()){
    digitalWrite(ledWifi,1);
  }else{
    digitalWrite(ledWifi,0);
  }
}
void OnledMqtt(void){
  if(MQTT.connected()){
    digitalWrite(ledBroker,1);
  }else{
    digitalWrite(ledBroker,0);
  }
}

// --- Funções para refirnar e enviar informaçãoes ao Dashboard -----------
float temperaturaComposto(void){
//utilizar return e ponteiro caso necessário tratar saida
//return temcomp;
}

float temperaturaResistencia(void){
  //return tempres;
}

float controlPI(void){
  //recebe(parâmetro) a temperatura alvo e executa o controle pi
  //utilizar um return de sinal pwm para 
}

float controlCoolers(void){
  //recebe(parâmetro) a velocidade alvo do dashboard e executa o controle da velocidade
  // ligar e desligar coolers
  
}

// --- Função Setup ----------------
void setup() {
  Serial.begin(115200);

  // --- Definir pinos de saídas de entradas -----------
  pinMode(ledBroker,OUTPUT);
  pinMode(ledWifi,OUTPUT);
  pinMode(turnOnOff,OUTPUT);
  digitalWrite(ledBroker,0);
  digitalWrite(ledWifi,0);

  // --- Iniiciar wifi e mqtt na placa -----------------
  delay(500);
  initWifi();
  initMqtt();
}

// --- Função Loop --------------------------
void loop() {

  // --- Verifica se estão devidamente funcionando as ligações ---------
  verifywifi_mqtt();
  

  // if on
  // --- Variáveis tipo 'char' para serem publicadas ------------
  char tempcomp[10] = {0};
  char tempres[10] = {0};
  //sprintf();
  // --- Envia informação ao dashboard -------------------------
  MQTT.publish(TOPICO_PUB_TEMPCOMPOSTO,tempcomp);
  MQTT.publish(TOPICO_PUB_TEMPRESISTENCIA,tempres);



  // --- Mantém a conexão com o broker -------------------
  MQTT.loop();
  delay(500);
}



















