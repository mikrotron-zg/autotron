
// jednostavan program za upravljanje kolicima preko lokalne bežične mreže

// hardver:
// mboard: http://www.diykits.eu/shophr/products/p_10207
// wee modul: http://www.diykits.eu/shophr/products/p_10442
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
// pozivom funkcije initWiFi bee modul se spaja na upisanu mrežu i otvara TCP server na zadanom port-u
// program ulazi u glavnu petlju u kojoj server čeka naredbe najviše 500ms
// ukoliko unutar 500ms server ne dobije novu naredbu, izvršava se naredba stop, u suprotnom se izvršava primljena naredba

// naredbe su podijeljene na dva bajta - prvi bajt je oznaka naredbe, a drugi je argument
// naredba "s" nema argument - nije bitna vrijednost drugog bajta ukoliko je prvi "s"
// ostalim definiranim naredbama drugi bajt označava brzinu
// Definirane naredbe:
// "s" - stop
// "f*" - naprijed brzinom *
// "b*" - nazad brzinom *
// "l*" - lijevo brzinom *
// "r*" - desno brzinom *

// Tomislav Mamić za MIKROTRON d.o.o. 16.2.2016.

#include "ESP8266.h"  // wifi lib

// port map
#define IN1 7
#define IN2 8
#define ENA 10
#define IN3 12
#define IN4 13
#define ENB 11

// wifi data
#define SSID        "krbljovrtislav"
#define PASSWORD    "Kobasica"
#define S_PORT 9988
ESP8266 wifi(Serial1);

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

void updateMotors(){
  
  // zapisivanje zakona upravljanja na L298P

  if(M1fwd){
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
  }else{
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
  }
  analogWrite(ENA,M1ref);
  if(M2fwd){
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
  }else{
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH);
  }
  analogWrite(ENB,M2ref);

}

void initWifi(){
  
  // rutina za podizanje bee modula po ITEAD studio receptu
  
  int errs=0;

  Serial.println("start setup");
    
    Serial.print("firmware:");
    Serial.println(wifi.getVersion().c_str());
      
    if (wifi.setOprToStationSoftAP()) {
        Serial.println("station + softap OK");
    } else {
        Serial.println("station + softap ERR");
        errs+=1;
    }
 
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.println("AP join OK");
        Serial.print("IP: ");
        Serial.println(wifi.getLocalIP().c_str());    
    } else {
        Serial.println("AP join ERR");
        errs+=1;
    }
    
    if (wifi.enableMUX()) {
        Serial.println("mux OK");
    } else {
        Serial.println("mux ERR");
        errs+=1;
    }
    
    if (wifi.startTCPServer(9988)) {
        Serial.println("start tcp server OK");
    } else {
        Serial.println("start tcp server ERR");
        errs+=1;
    }
    
    if (wifi.setTCPServerTimeout(180)) { 
        Serial.println("set tcp server timout set OK (180s)");
    } else {
        Serial.println("set tcp server timout ERR");
        errs+=1;
    }
    
    Serial.print("setup ended with "+String(errs)+" errors");
  }
  
void shutWifi(){
  
  //restart wee modula
  
  wifi.restart();
  initWifi();
  
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
  
  // comm init
  Serial.begin(9600);
  
  // 5s vremena da korisnik uključi serial monitor radi praćenja initWifi poruka, nije pretjerano bitno
  delay(5000);
  initWifi();
  
  //motor init
  M1ref=0;
  M2ref=0;
  updateMotors();

}

void loop() {
  
  long unsigned int T=millis();
  String msg="";
  
  //Serial.println(wifi.getLocalIP().c_str());                          //-------------- odkomentirati za potrebe čitanja IPja - usporava glavnu petlju
  
  uint8_t buffer[2] = {0};                                            // buffer veličine 2 bajta - to je duljina naredbi
  uint8_t mux_id;                                                     // oznaka veze na serveru robota (server mora biti u mogućnosti odgovoriti na više veza odjednom)
  uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), 500);     // primanje naredbe s timeoutom 500ms
  if (len > 0) {                                                      // čitanje naredbe u string msg
    for(uint32_t i = 0; i < len; i++) {
      msg+=char(buffer[i]);
    }
  }
  if (T-lastCall>499){msg[0]='s';}                                    // sigurnosni timeout - ukoliko ne dođe naredba unutar 500ms, robot staje
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
    M1ref=0;
    M2ref=spd;
  }
  if (msg[0]=='r'){                        // desno
    int spd=(int)msg[1];
    M1ref=spd;
    M2ref=0;
  }
  updateMotors();                          // zapisivanje referenci na motore
  Serial.println("recv: "+msg+" T="+String(T-lastCall));
  lastCall=T;
  
}
