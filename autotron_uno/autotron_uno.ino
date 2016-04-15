
// jednostavan program za autonomno upravljanje kolicima povratnom vezom preko ultrazvučnog senzora udaljenosti

// hardver:
// arduino uno: http://www.diykits.eu/shophr/products/p_A000066 !!! ili bilo koja druga kompatibilna pločica !!!
// arduino motor shield: http://www.diykits.eu/shophr/products/p_A000079
// HC-SR04 ultrazvučni senzor: http://www.diykits.eu/shophr/products/p_10204
// robotska kolica: http://www.diykits.eu/shophr/products/robot-drone/p_10406

// upravljanje H mostovima integriranim u čipu L298P
// H most koristi 3 pina - 2 logička određuju režim rada motora, a 3. pwm određuje brzinu vrtnje
// arduino motor shield sređuje logiku rada motora tako da pinovi režima rada motora nisu direktno dostupni
// dostupan je jedan pin (FWD) čije stanje određuje smjer vrtnje motora
// PWM implementiran na arduino platformi radi na frekvenciji 500Hz
// unutar čujnog područja je, i u određenim prilikama unutar DC motora, moguće je čuti ton kojeg stvaraju vibrirajuće žice
// na pinove PWM zapisuje se 8-bitni broj u rasponu 0-255 koji označava postotak propusnosti PWM ciklusa (255~100%)

// Tomislav Mamić za MIKROTRON d.o.o. 16.2.2016.

// port map
#define FWD1 12
#define PWM1 3
#define FWD2 13
#define PWM2 11
#define MARCO A2
#define POLO A3

// zapisane vrijednosti i reference - reference se ne koriste ali su potrebne za naprednije upravljanje
int M1ref;
int M1refTarget;
int M2ref;
int M2refTarget;
boolean M1fwd;
boolean M1fwdTarget;
boolean M2fwd;
boolean M2fwdTarget;

void updateMotors(){
  
  // zapisivanje zakona upravljanja na L298P

  if(M1fwd){
    digitalWrite(FWD1,HIGH);
  }else{
    digitalWrite(FWD1,LOW);
  }
  analogWrite(PWM1,M1ref);
  if(M2fwd){
    digitalWrite(FWD2,HIGH);
  }else{
    digitalWrite(FWD2,LOW);
  }
  analogWrite(PWM2,M2ref);

}

int USrangeLocking(){
  
  // sonar funkcija, pulseIn je locking funkcija s timeoutom u mikrosekundama
  
  digitalWrite(MARCO,LOW);
  delayMicroseconds(2);
  digitalWrite(MARCO,HIGH);
  delayMicroseconds(10);
  digitalWrite(MARCO,LOW);
  return pulseIn(POLO,HIGH,100000)/2/29.1;
  
}

void turn(boolean side, int pwm, int ms){

  // skretanje brzinom pwm u trajanju ms
  
  M1ref=pwm;
  M2ref=pwm;
  M1fwd=side;
  M2fwd=!side;
  updateMotors();
  delay(ms);
  M1ref=0;
  M2ref=0;
  updateMotors();
  
}

int look(boolean side){
  
  // traženje prepreke lijevo-desno
  
  turn(side, 180, 200);
  int ret=USrangeLocking();
  turn(!side, 180, 200);
  return ret;
}

void drive(boolean fwd, int pwm, int ms){

  // vožnja naprijed-natrag brzinom pwm trajanja ms
  
  M1fwd=!fwd;
  M2fwd=!fwd;
  M1ref=pwm;
  M2ref=pwm;
  Serial.println("fwd start");
  updateMotors();
  delay(ms);
  M1ref=0;
  M2ref=0;
  updateMotors();
  Serial.println("fwd end");

}

void setup() {
  
  // IO
  pinMode(FWD1,OUTPUT);
  pinMode(FWD2,OUTPUT);
  pinMode(PWM1,OUTPUT);
  pinMode(PWM2,OUTPUT);
  pinMode(MARCO,OUTPUT);
  pinMode(POLO,INPUT);
  
  // init serial
  Serial.begin(9600);
  
  // init motors
  M1ref=0;
  M2ref=0;
  M1fwd=LOW;
  M2fwd=LOW;

}

void loop() {

  int range=USrangeLocking();                                                  // pogledaj ispred sebe
  int rleft=look(true);                                                        // pogledaj malo ulijevo
  int rright=look(false);                                                      // pogledaj malo udesno
  
  Serial.println(String(range)+", "+String(rleft)+", "+String(rright));
  
  if (range>30&&rleft>30&&rright>30){                                          // ako ima barem 30cm prostora ispred, vozi malo naprijed
    drive(1,180,800);
    Serial.println("fwd");
  }else if(rleft>30){                                                          // ako ne možeš naprijed, ali lijevo ima mjesta, okreni se malo ulijevo
    turn(true, 180,300);
    Serial.println("left");
  }else if(rright>30){                                                         // ako ne možeš ni naprijed ni lijevo, ali desno ima mjesta, okreni se malo udesno
    turn(false, 180, 300);
    Serial.println("right");
  }else{                                                                       // ako ne možeš nikud, okreni se skroz
    turn(true, 180, 600);
    Serial.println("around");
  }
  
}
