
// jednostavan program za upravljanje kolicima preko lokalne bežične mreže

// hardver:
// croduino nova: http://www.diykits.eu/products/p_10670
// motor driver: http://www.diykits.eu/products/motor/p_10218
// robotska kolica: http://www.diykits.eu/shophr/products/robot-drone/p_10406

// upravljanje H mostovima integriranim u čipu L298P
// H most koristi 3 pina - 2 logička određuju režim rada motora, a 3. pwm određuje brzinu vrtnje
// PWM implementiran na arduino platformi radi na frekvenciji 500Hz
// unutar čujnog područja je, i u određenim prilikama unutar DC motora, moguće je čuti ton kojeg stvaraju vibrirajuće žice
// na pinove EN zapisuje se 8-bitni broj u rasponu 0-255 koji označava postotak propusnosti PWM ciklusa (255~100%)
// IN pinovi definiraju režim rada motora
// oba LOW - free stop, sav protok struje kroz motor je isključen - isti efekt ima i zapisivanje 0 na EN pin
// oba HIGH - aktivno kočenje
// jedan LOW, jedan HIGH - vrtnja u smjeru koji ovisi o tome koji IN pin je HIGH, a koji LOW

// primanje naredbi preko WLAN mreže
// pozivom funkcije initWiFi nova se spaja na upisanu mrežu i otvara TCP server na zadanom port-u
// program ulazi u glavnu petlju u kojoj server sluša naredbe
// ukoliko server ne dobije novu naredbu, izvršava se naredba stop, u suprotnom se izvršava primljena naredba

// naredbe su podijeljene na dva bajta - prvi bajt je oznaka naredbe, a drugi je argument
// naredba "s" nema argument - nije bitna vrijednost drugog bajta ukoliko je prvi "s"
// ostalim definiranim naredbama drugi bajt označava brzinu
// Definirane naredbe:
// "s" - stop
// "f*" - naprijed brzinom *
// "b*" - nazad brzinom *
// "l*" - lijevo brzinom *
// "r*" - desno brzinom *

// Tomislav Mamić za MIKROTRON d.o.o. 16.3.2016.

#include <ESP8266WiFi.h>

// port map
#define IN1 0
#define IN2 2
#define ENA 15
#define IN3 14
#define IN4 16
#define ENB 12

// wifi data
#define MAX_SRV_CLIENTS 5
#define S_PORT 9988
const char* ssid = "krbljovrtislav";
const char* password = "Kobasica";
WiFiServer server(S_PORT);
WiFiClient serverClients[MAX_SRV_CLIENTS];

// reference i zapisana stanja motora - reference se ne koriste u ovom programu, ali za naprednije upravljanje su dobra ideja
int M1ref;
int M1refTarget;
int M2ref;
int M2refTarget;
boolean M1fwd;
boolean M1fwdTarget;
boolean M2fwd;
boolean M2fwdTarget;

// mjerenje vremena
long unsigned int lastCall=0;
long unsigned int timer=0;
int inc;

void updateMotors(){
  
  // zapisivanje zakona upravljanja na L298P

  if(M1fwd){
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
  }else{
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
  }
  analogWrite(ENA,M1ref*4);
  if(M2fwd){
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
  }else{
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH);
  }
  analogWrite(ENB,M2ref*4);

}

void initWiFi(){
  WiFi.begin(ssid,password);
  Serial.print("\nConnecting to "); Serial.println(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  Serial.println(" (attempt "+String(i)+")");
  if(i == 21){
    Serial.print("Could not connect to"); Serial.print(ssid);
    while(1) delay(500);
  }
  server.begin();
  server.setNoDelay(true);
  Serial.print("Listening to "+String(S_PORT)+" at ");Serial.println(WiFi.localIP());
}

String handleServer(){
  if (server.hasClient()){
    for(int i = 0; i < MAX_SRV_CLIENTS; i++){
      if (!serverClients[i] || !serverClients[i].connected()){  // ima li klijenta na i-tom mjestu, i je li klijent spojen?
        if(serverClients[i]) serverClients[i].stop();           // ako ima, ali nije spojen, odbaci
        serverClients[i] = server.available();                  // spoji novog dolaznog klijenta
        continue;
      }
    }
    WiFiClient serverClient = server.available();               // ako nema slobodnog mjesta, ulovi dolaznog klijenta i odbaci
    serverClient.stop();
  }
  char buff[2];
  for(int i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){                                // ako je i-ti klijent povezan
      if(serverClients[i].available()){                                                   // ako ima poslanih podataka
        for(int j=0;j<2;j++){                                                             // napuni spremnik sa 2 znaka (ocekivani format naredbe)
          if (serverClients[i].available()){
            buff[j]=serverClients[i].read();
          }
        }
      }
    }
  }
  String pass="";
  for (int i=0;i<2;i++){                                                                  // provjera valjanosti primljene naredbe
    if (buff[0]=='s'||buff[0]=='f'||buff[0]=='b'||buff[0]=='l'||buff[0]=='r'){
      pass+=buff[i];
    }else{
      pass+="s";
    }
  }
  return pass;
}

void setup() {
  
  // IO
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENB,OUTPUT);
  
  // timer init
  lastCall=millis();
  inc=1;
  
  // comm init
  Serial.begin(9600);
  
  //motor init
  M1ref=0;
  M2ref=0;
  updateMotors();
  
  //wifi init
  initWiFi();
}

void loop() {
  
  long unsigned int T=millis();
  String msg=handleServer();
  if (T-lastCall>50){
    Serial.println(msg);
    if (msg[0]=='s'){                        // stop (soft stop)
      M1ref=0;
      M2ref=0;
    }
    if (msg[0]=='f'){                        // naprijed
      int spd=(int)msg[1];
      M1ref=spd;
      M1fwd=true;
      M2ref=spd;
      M2fwd=true;
    }
    if (msg[0]=='b'){                        // natrag
      int spd=(int)msg[1];
      M1ref=spd;
      M1fwd=false;
      M2ref=spd;
      M2fwd=false;
    }
    if (msg[0]=='l'){                        // lijevo
      int spd=(int)msg[1];
      M1ref=spd;
      M1fwd=true;
      M2ref=0;
      M2fwd=false;
    }
    if (msg[0]=='r'){                        // desno
      int spd=(int)msg[1];
      M1ref=0;
      M1fwd=false;
      M2ref=spd;
      M2fwd=true;
    }
    updateMotors();                          // zapisivanje referenci na motore
    lastCall=T;
  }
}
