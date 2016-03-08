//WOORDKLOK

// ER WORDT GEWERKT MET 3 SHIFTREGISTERS, VOOR ELK REGISTER
// WORDT EEN DECIMALE WAARDE BEREKEND VOLGENS ONDERSTAANDE TABEL

//WAARDE  SHIFT1  SHIFT2    SHIFT3
//1       EEN     NEGEN     VIJF
//2       TWEE    TIEN      TIEN  
//4       DRIE    ELF       KWART
//8       VIER    TWAALF    HALF  
//16      VIJF    MINUUT1   VOOR
//32      ZES     MINUUT2   OVER
//64      ZEVEN   MINUUT3   UUR
//128     ACHT    MINUUT4   HET IS

//BIBLIOTHEKEN

#include <DS1302.h> //bibliotheek van Real Time Clock

//VARIABELEN


//definieren van globale variabelen (deze zijn geldig in het volledige programma, in tegenstelling
//tot lokale variabelen die in de procedures/functies worden gedefinieerd.

//aansluiting van shiftregisters op Arduino

int SHIFTlatchPin=4;
int SHIFTclockPin=3;
int SHIFTdataPin=5;

//aansluiting van Real Time Clock op Arduino

int RTCresetPin=8; //ook CE Pin genoemd
int RTCdataPin=9; // ook I/O Pin genoemd
int RTCclockPin=10; //ook SCLK Pin genoemd
#define RTCgndPin 11
#define RTCvccPin 12

//aansluiting schuifschakelaar voor instellen op Arduino

int KNOPset=2;

//aansluiting van drukknoppen voor instellen op Arduino

int KNOPmin=7;
int KNOPuur=6;





//INITIALISATIE

DS1302 rtc(8,9,10);
int uren;
int minuten;

// declaratie variabelen voor shiftregisters

int shift[4]; 

void setup() {
  
//initialisatie voor seriële monitor
  
  Serial.begin(9600); 

//druktoetsen voor instellen woordklok

  pinMode (KNOPset,INPUT); // overgang naar instellen van klok, naar subrouting zetklok(), bij verlaten starten seconden
  pinMode (KNOPmin,INPUT); // instellen van minuten (verhogen per minuut) 
  pinMode (KNOPuur,INPUT); // instellen van uren

//instellen shiftregister

  pinMode (SHIFTlatchPin,OUTPUT);
  pinMode (SHIFTclockPin,OUTPUT);
  pinMode (SHIFTdataPin,OUTPUT);

//instellen real time clock

  pinMode (RTCgndPin,OUTPUT);
  pinMode (RTCvccPin,OUTPUT);
  digitalWrite (RTCgndPin,LOW);
  digitalWrite (RTCvccPin,HIGH);
  rtc.halt(false);
  rtc.writeProtect(false);
  
}



int leesUren(){
  String urenStr=rtc.getTimeStr();
  int uur=urenStr.substring(0,2).toInt();
  return uur;
  }

int leesMinuten(){
  String minStr=rtc.getTimeStr();
  int minuut=minStr.substring(3,5).toInt();
  return minuut;
  }
 
void zetKlok() {
  
  if(digitalRead(KNOPuur)==1){uren=uren+1;} if(uren==24){uren=0;}
  if(digitalRead(KNOPmin)==1){minuten=minuten+1;}
  }

void stuurShift(int lshift1,int lshift2,int lshift3) {
  digitalWrite(SHIFTlatchPin,0);
  shiftOut(SHIFTdataPin, SHIFTclockPin,MSBFIRST,lshift3);
  shiftOut(SHIFTdataPin, SHIFTclockPin,MSBFIRST,lshift2);
  shiftOut(SHIFTdataPin, SHIFTclockPin,MSBFIRST,lshift1);
  digitalWrite(SHIFTlatchPin,1);
   
  }


void loop() {
  int statusSetKnop=digitalRead(KNOPset);
  int uren=leesUren();
  int minuten=leesMinuten();
  Serial.println("Loop :");
  Serial.print("uren: ");Serial.println(uren);
  Serial.print("minuten: ");Serial.println(minuten);
  if (statusSetKnop==1) {zetKlok();delay(1000);} else {delay(5000);}
  // voor de shift registers wordt een array gebruikt , met 4 indexen, voor overeenstemming
// met de nummer van het shiftregister wordt de waarde voor index nul niet gebruik. 

  int shift[4]= {0,0,0,0};

// vermits het hier over een 12-uur klokt worden volgende correcties toegepast : voor nul uur wordt 12 bijgeteld,
// voor waarden boven twaalf wordt 12 afgetrokken
  
  if (uren>12){uren=uren-12;};
  if (uren==0) {uren=uren+12;};
  int hoekminuten = minuten%5;
  int vijfminuten = (minuten-hoekminuten)/5;

// zodra het aantal minuten hoger is dan 19 wordt er bij het uur 1 bijgeteld (dus vanaf 10 voor half ...'komende' uur

  if (minuten>19){uren=uren+1;}  

// bepaling van de waarden voor shiftregisters 1 en 2
 
 
  
  if (uren<9){
      shift[1]=1;
      for(int a=1;a<(uren);a++) {shift[1]=shift[1]*2;}
      }
  else {
      shift[2]=1;
      for(int b=1;b<(uren-8);b++) {shift[2]=shift[2]*2;}
      }
  
  switch (hoekminuten){
    case 0:break; 
    case 1:shift[2]=shift[2]+16;break;
    case 2:shift[2]=shift[2]+48;break;
    case 3:shift[2]=shift[2]+112;break;
    case 4:shift[2]=shift[2]+240;break;
  }
 

// bepaling van waarden voor shiftregister 3

  switch (vijfminuten){

    case 0:shift[3]=192;break; 
    case 1:shift[3]=161;break;
    case 2:shift[3]=162;break;
    case 3:shift[3]=164;break;
    case 4:shift[3]=154;break;
    case 5:shift[3]=153;break;
    case 6:shift[3]=136;break;
    case 7:shift[3]=169;break;
    case 8:shift[3]=170;break;
    case 9:shift[3]=148;break;
    case 10:shift[3]=146;break;
    case 11:shift[3]=145;break;
    }
    Serial.println("waarden-->>");
    Serial.print("shift1: ");Serial.println(shift[1]);
    Serial.print("shift2: ");Serial.println(shift[2]);
    Serial.print("shift3: ");Serial.println(shift[3]);
  //stuurShift(shift[1],shift[2],shift[3]);
  }
