#include <DS3231.h>
#include <Wire.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote2.h>
#include <DHT.h>
#include <EEPROM.h>
#define pinDHT 5
#define typDHT11 DHT11

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif
DHT mojeDHT(pinDHT, typDHT11);

DS3231 clock;
RTCDateTime dt;
LiquidCrystal_I2C lcd(0x27, 16, 2); // nastav adresu LDC na 0x27 pro 16 sloupcovy a 2 radkovy displej

// toto je zacatek meho dalsiho projektu MULTI-KRABICKA 
// tento projekt jsem zacal 07-09-2023
// Zapojeni IR snimace   VCC, GND, D11
// Zapojeni displeje  VCC, GND, SDA-A4, SCL-A5
// Zapojeni DHT11   VCC, GND, DATA primo na D5, DATA pres odpor 10kOhm na +5V 
// Zapojeni vyp1 GND ,D9  ---  vyp2 je senzor  Vcc,GND,D10
// Zapojeni pipatka GND, D7
// Do pinu A0 a A1 nic nezapojuj

String verze=" SW 200224 odpol"; 
String nazevdne="  PoUtStCtPaSoNe";
String nazevmesice="   ledunobredubkvecercvcsrpzarrijlispro";
byte vstup=0;
byte necobylozadano=0;
int long cislo=0; 
int vysledek=0; 
byte zadano=0; 
byte funkce=0; 
String cisl=" ";
byte prepinac=0; // 1 vstup hodina, 2 vstup minuta, 3 vse zapis do ds3231
byte tlac9 = 0; 
byte tlac10 = 0;
byte desetiny = 0;
int sekundy = 0;
int minuty = 0;
byte hodiny = 0;
unsigned long cas=0;
byte hodina = 0;
byte minuta = 0;
byte sekunda = 0;
int denvtydnu = 1;
byte den = 1;
byte mesic = 1;
int pocetdni =1;
int rok = 2000;
int zapis=0;
int generator=0;
float teplota =20;
byte a1hodina = 1;
byte a1minuta = 1;
byte a2hodina = 0;
byte a2minuta = 0;
byte budetobudit = 0;  // pokud ano 1, pokud ne 0
uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
unsigned long casovaZnacka;
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;

void prerus() 
{ tlac9=digitalRead(9); if (tlac9 == LOW and funkce==2) {sekundy=sekundy+1;};
 if (tlac9 == LOW and funkce==3) {sekundy=sekundy-1;};
 }

void nactiVstupy() 
{
  tlac9=digitalRead(9); tlac10=digitalRead(10);  // D9 vypinac     D10 senzor tlac.
  }

void vstup_domecek()    // tento blok chvili ceka zda-li nekdo stiskne DOMECEK
{          
 irrecv.resume(); irrecv.enableIRIn(); delay(10); 
for (int i = 0; i < 100; i++) {delay(30);
if (irrecv.decode(&results)) 
{digitalWrite(13,HIGH); delay(300) ;digitalWrite(13,LOW);
if (results.value == 1302829408 or results.value == 3784077798)  {vstup=160; 
for (int x = 0; x <17; x=x+2) {lcd.setCursor(x,0); lcd.print(" ");delay(10);lcd.setCursor(x+1,1); lcd.print(" ");delay(10); };
for (int x = 0; x <17; x=x+2) {lcd.setCursor(x,1); lcd.print(" ");delay(10);lcd.setCursor(x+1,0); lcd.print(" ");delay(10); };
 necobylozadano=1; i=100; irrecv.resume();delay(400);};}; irrecv.resume(); 
}; irrecv.resume();
}

void funkce_1()
{ lcd.clear(); clock.enableOutput(false);
  while (funkce==1)
 {
  float tep = mojeDHT.readTemperature();
  float vlh = mojeDHT.readHumidity(); delay(300); 
  if (isnan(tep) || isnan(vlh)) {
    // pri chybnem cteni vypis hlasku
    lcd.setCursor(0, 0); lcd.print("Chyba pri ctení ");} 
else {lcd.setCursor(0, 0);
    lcd.print("Teplota: "); lcd.print(tep);
    lcd.print((char)223); lcd.print("C");
    lcd.setCursor(0, 1); lcd.print("vlhkost: "); 
    lcd.print(vlh); lcd.print(" % ");
  } ; nactiVstupy();
  delay(100);
  if (tlac10==HIGH) {funkce=0;delay(50);} 
    }
   delay(300); 
  }

void funkce_2()
{lcd.clear(); desetiny=0; sekundy=0; minuty=0; hodiny=0; irrecv.enableIRIn();clock.enableOutput(true);
lcd.setCursor(0,0); lcd.print(" Pro start/stop "); lcd.setCursor(0,1); lcd.print(" pouzij vypinac "); delay(2000); 
lcd.setCursor(0,0); lcd.print("                "); lcd.setCursor(5,0);lcd.print(":");
lcd.setCursor(8,0);lcd.print(":"); lcd.setCursor(11,0);lcd.print(",");
while (funkce == 2) {nactiVstupy();  
 if (clock.isOutput()) {lcd.setCursor(15,0);lcd.print("T");} else {lcd.setCursor(15,0);lcd.print("F");};
  if (tlac9 == LOW) {clock.enableOutput(true);attachInterrupt(digitalPinToInterrupt(2),prerus, RISING);
  if ((millis()-cas)>=192) {cas=millis(); desetiny=desetiny+2; };}; //nastaveno-nemenit
nactiVstupy(); 
  if (tlac9 == HIGH and tlac10 == HIGH) {desetiny=0; sekundy=0; minuty=0; hodiny=0; vstup_domecek();delay(3);}; //vynuluje pocitadlo
  if (tlac9 == HIGH) {clock.enableOutput(false); delay(1);}; 
  if (vstup==160) {funkce=0;};
  if (desetiny > 9) {desetiny=0;};
  if (sekundy > 59) {sekundy=0; minuty=minuty+1;}; 
  if (minuty > 59) {minuty=0; hodiny=hodiny+1;};
  if (hodiny > 99) {funkce=0;};
  lcd.setCursor(5,0);lcd.print(":");
lcd.setCursor(8,0);lcd.print(":"); lcd.setCursor(11,0);lcd.print(",");
if (hodiny<10) {lcd.setCursor(3,0);lcd.print(" ");lcd.setCursor(4,0);lcd.print(hodiny);} 
  else {lcd.setCursor(3,0);lcd.print(hodiny);}
if (minuty<10) {lcd.setCursor(6,0);lcd.print("0");lcd.setCursor(7,0);lcd.print(minuty);} 
  else {lcd.setCursor(6,0);lcd.print(minuty);}
if (sekundy<10) {lcd.setCursor(9,0);lcd.print("0");lcd.setCursor(10,0);lcd.print(sekundy);} 
  else {lcd.setCursor(9,0);lcd.print(sekundy);}
lcd.setCursor(12,0);lcd.print(desetiny);
  };
  }

void funkce_3()
{lcd.clear(); lcd.print("Odpocitavani   ");lcd.setCursor(0,1); lcd.print("Max. 99 minut !");
delay(2500); lcd.clear(); clock.enableOutput(false);cislo=0;
while (funkce==3) {vstup_domecek(); nactiVstupy();
if (vstup==160 or tlac10==HIGH) {funkce=0;};
{ lcd.setCursor(0, 0); 
  lcd.print(" Zadej cislo "); 
  vstup_cislo();    
  if (vstup == 100 and cislo>0) 
  {lcd.setCursor(0,1 ); lcd.print("        ");delay(10); necobylozadano=0; 
  lcd.setCursor(0,0); lcd.print("Bylo nastaveno"); 
  lcd.setCursor(1,1); lcd.print(cislo); lcd.print(" minut      ");delay(2500); minuty=cislo;vstup=0;odpocitavani();     
  };
if (vstup<10 and cislo<1 and necobylozadano == 1)
{cislo=vstup; lcd.setCursor(0,1); lcd.print(cislo); lcd.print("    ");delay(800);
necobylozadano=0; vstup=0; 
}  
if (vstup<10 and cislo<10 and necobylozadano == 1) 
{cislo=((cislo*10)+vstup); lcd.setCursor(0,1); lcd.print(cislo); lcd.print("    ");delay(800);
necobylozadano=0; vstup=0;
}
if (vstup<10 and cislo<100 and necobylozadano == 1) 
{cislo=((cislo*10)+vstup); lcd.setCursor(0,1); lcd.print(cislo); lcd.print("    ");delay(800);
necobylozadano=0; vstup=0; 
}
if (cislo>99) {lcd.clear(); lcd.setCursor(0,1); lcd.print(cislo); lcd.print("  Chyba !   ");delay(4000);
necobylozadano=0; cislo=0; vstup=0; lcd.clear();} 
}
};  
}

void odpocitavani()
{lcd.clear(); desetiny=0; sekundy=0; hodiny=0; 
if (minuty>60) {hodiny=hodiny+1; minuty=minuty-60;};irrecv.enableIRIn();
lcd.setCursor(0,0); lcd.print(" Pro start/stop "); lcd.setCursor(0,1); lcd.print(" pouzij vypinac "); delay(2000); 
lcd.setCursor(0,0); lcd.print("                "); lcd.setCursor(5,0);lcd.print(":");
lcd.setCursor(8,0);lcd.print(":"); lcd.setCursor(11,0);
while (funkce == 3) {nactiVstupy();  
  if (tlac9 == LOW) {clock.enableOutput(true);attachInterrupt(digitalPinToInterrupt(2),prerus, RISING);
  if ((millis()-cas)>=192) {cas=millis(); desetiny=desetiny+2; };}; //nastaveno-nemenit
nactiVstupy(); 
  if (tlac9 == HIGH and tlac10 == HIGH) {desetiny=0; sekundy=0; minuty=0;hodiny=0;cislo=0;delay(300);funkce_3();}; //vynuluje pocitadlo
  if (tlac9 == HIGH) {clock.enableOutput(false); delay(1);}; 
  if (vstup==160) {funkce=0;};
  if (desetiny > 9) {desetiny=0;};
  if (sekundy<0) {sekundy=59; minuty=minuty-1;}; 
  if (minuty<0) {minuty=59; hodiny=hodiny-1;};
  if (minuty==0 and sekundy==0 and hodiny==0) {lcd.clear(); lcd.setCursor(0,0); lcd.print(" To je konec!!! ");
  digitalWrite(7,HIGH); delay(200);digitalWrite(7,LOW);delay(400);
   digitalWrite(7,HIGH); delay(200);digitalWrite(7,LOW);delay(5000);funkce=0;};
lcd.setCursor(5,0);lcd.print(":");
lcd.setCursor(8,0);lcd.print(":"); 
if (hodiny<10) {lcd.setCursor(3,0);lcd.print("0");lcd.setCursor(4,0);lcd.print(hodiny);} 
  else {lcd.setCursor(3,0);lcd.print(hodiny);}
if (minuty<10) {lcd.setCursor(6,0);lcd.print("0");lcd.setCursor(7,0);lcd.print(minuty);} 
  else {lcd.setCursor(6,0);lcd.print(minuty);}
if (sekundy<10) {lcd.setCursor(9,0);lcd.print("0");lcd.setCursor(10,0);lcd.print(sekundy);} 
  else {lcd.setCursor(9,0);lcd.print(sekundy);lcd.print(" ");}
 };
}
  
void funkce_4()  
{delay(2000);lcd.clear();clock.enableOutput(false); prepinac=1;
while (funkce==4) {nactiVstupy();  if (tlac10==HIGH) {funkce=0;delay(150);};
lcd.setCursor(0, 0); 
  lcd.print(" Zadej cislo "); 
  vstup_cislo();    
if (vstup<10 and cislo<1 and necobylozadano == 1)
{cislo=vstup; lcd.setCursor(0,1); lcd.print(cislo); lcd.print("    ");delay(800);
necobylozadano=0; vstup=0; 
};
if (vstup<10 and cislo<10 and necobylozadano == 1) 
{cislo=((cislo*10)+vstup); lcd.setCursor(0,1); lcd.print(cislo); lcd.print("    ");delay(800);
necobylozadano=0; vstup=0;
};
if (vstup<10 and cislo<100 and necobylozadano == 1) 
{cislo=((cislo*10)+vstup); lcd.setCursor(0,1); lcd.print(cislo); lcd.print("    ");delay(800);
necobylozadano=0; vstup=0; 
};
if (cislo>23 and prepinac==1) {lcd.clear(); lcd.setCursor(0,1); lcd.print(cislo); lcd.print("  Chyba !   ");delay(3000);
necobylozadano=0; cislo=0; vstup=0; lcd.clear();};

if (cislo>59 and prepinac==2) {lcd.clear(); lcd.setCursor(0,1); lcd.print(cislo); lcd.print("  Chyba !   ");delay(3000);
necobylozadano=0; cislo=0; vstup=0; lcd.clear();};

if (vstup == 100 and cislo>0 and prepinac==1) 
  {lcd.setCursor(0,1 ); lcd.print("        ");delay(10); necobylozadano=0; 
  lcd.setCursor(0,0); lcd.print("Bylo nastaveno"); 
  lcd.setCursor(1,1); lcd.print(cislo); lcd.print(" hodin      ");delay(2500); lcd.clear(); a1hodina=cislo; vstup=0; cislo=0; prepinac=2;};

if (vstup == 100 and cislo>0 and prepinac==2) 
  {lcd.setCursor(0,1 ); lcd.print("        ");delay(10); necobylozadano=0; 
  lcd.setCursor(0,0); lcd.print("Bylo nastaveno"); 
  lcd.setCursor(1,1); lcd.print(cislo); lcd.print(" minut    ");delay(2500); lcd.clear(); a1minuta=cislo; vstup=0; cislo=0; prepinac=3;};

if (prepinac==3) 
  {lcd.setCursor(0,1 ); lcd.print("        ");delay(10); necobylozadano=0; 
  lcd.setCursor(0,0); lcd.print("Budik nastaven"); 
  lcd.setCursor(1,1); lcd.print(" na ");lcd.print(a1hodina); lcd.print((char)58);if (a1minuta<10) {lcd.print("0");}; 
  lcd.print(a1minuta);lcd.print(" ");lcd.print(char(0));lcd.print("   "); 
  delay(3500); lcd.clear();  clock.setAlarm1(0,a1hodina,a1minuta,0, DS3231_MATCH_H_M_S); 
  prepinac=0;budetobudit=1; vstup=0; cislo=0; funkce=5; funkce_5();};
}; 
}

void funkce_5()     //cas a datum
{ delay(2000);lcd.clear();clock.enableOutput(false);  
  while (funkce==5)
 { dt = clock.getDateTime();
  sekunda=(dt.second); 
  hodina=(dt.hour); minuta=(dt.minute); // pocetdni=(clock.dateFormat("t", dt)); zatim nefunguje
  rok=(dt.year); denvtydnu=(dt.dayOfWeek);   
  den=(dt.day); mesic=(dt.month); 
  a1hodina=(clock.getAlarm1().hour);
  a1minuta=(clock.getAlarm1().minute); 
  clock.forceConversion(); teplota = (clock.readTemperature());
  if (clock.isAlarm1() and budetobudit==1) { budik(); }; //ted probiha alarm
  if (budetobudit==1) {lcd.setCursor(15,0);lcd.print(char(0));} else {lcd.setCursor(15,0);lcd.print(char(6));};
  lcd.setCursor(8,0);lcd.print(":");  lcd.setCursor(5,0);lcd.print(":");
if (hodina<10) {lcd.setCursor(3,0);lcd.print(" ");lcd.setCursor(4,0);lcd.print(hodina);} 
  else {lcd.setCursor(3,0);lcd.print(hodina);}
if (minuta<10) {lcd.setCursor(6,0);lcd.print("0");lcd.setCursor(7,0);lcd.print(minuta);} 
  else {lcd.setCursor(6,0);lcd.print(minuta);}
if (sekunda<10) {lcd.setCursor(9,0);lcd.print("0");lcd.setCursor(10,0);lcd.print(sekunda);} 
  else {lcd.setCursor(9,0);lcd.print(sekunda);}
nactiVstupy(); 
 delay(10);
 if (tlac9==LOW) {budetobudit=1;} else {budetobudit=0;};
  if (tlac10==HIGH) {funkce=0;delay(150);}
if (sekunda==25) {lcd.setCursor(2,1);;lcd.print(" ");lcd.print(den);lcd.print("/");
String zobrazto1=(nazevdne.substring((denvtydnu*2),((denvtydnu*2)+2)));
String zobrazto2=(nazevmesice.substring((mesic*3),((mesic*3)+3))); 
lcd.print(zobrazto2);lcd.print("/"); lcd.print(rok);lcd.print("   ");
lcd.setCursor(0,1);lcd.print(zobrazto1); };
  if (sekunda==50) {lcd.setCursor(0,1);lcd.print("Teplota: ");lcd.print((teplota-1));lcd.print((char)223); lcd.print("C   ");} 
  delay(30); 
  if (sekunda==40) {lcd.setCursor(0,1);lcd.print("  Budik ");lcd.print(a1hodina);lcd.print((char)58);if (a1minuta<10) {lcd.print("0");};
  lcd.print(a1minuta);lcd.print("       ");}; 
  delay(30);
  if (sekunda==1) {lcd.setCursor(0,1);lcd.print((char)0);lcd.print(" = budik zapnut");lcd.print("  ");}; 
  if (sekunda==15) {lcd.setCursor(0,1);lcd.print((char)6);lcd.print(" = budik vypnut");lcd.print("  ");}; 
  };
  }


void funkce_6()   // nastaveni casu 
{delay(2000);lcd.clear();clock.enableOutput(false); 
dt = clock.getDateTime();
  hodina=(dt.hour); minuta=(dt.minute);rok=(dt.year);    
  den=(dt.day); mesic=(dt.month); prepinac=1;
while (funkce==6) {nactiVstupy();  if (tlac10==HIGH) {funkce=0;delay(150);};
lcd.setCursor(0, 0); 
  lcd.print(" Zadej cislo "); 
  vstup_cislo();    
if (vstup<10 and cislo<1 and necobylozadano == 1)
{cislo=vstup; lcd.setCursor(0,1); lcd.print(cislo); lcd.print("    ");delay(800);
necobylozadano=0; vstup=0; 
};
if (vstup<10 and cislo<10 and necobylozadano == 1) 
{cislo=((cislo*10)+vstup); lcd.setCursor(0,1); lcd.print(cislo); lcd.print("    ");delay(800);
necobylozadano=0; vstup=0;
};
if (vstup<10 and cislo<100 and necobylozadano == 1) 
{cislo=((cislo*10)+vstup); lcd.setCursor(0,1); lcd.print(cislo); lcd.print("    ");delay(800);
necobylozadano=0; vstup=0; 
};
if (cislo>23 and prepinac==1) {lcd.clear(); lcd.setCursor(0,1); lcd.print(cislo); lcd.print("  Chyba !   ");delay(3000);
necobylozadano=0; cislo=0; vstup=0; lcd.clear();};

if (cislo>59 and prepinac==2) {lcd.clear(); lcd.setCursor(0,1); lcd.print(cislo); lcd.print("  Chyba !   ");delay(3000);
necobylozadano=0; cislo=0; vstup=0; lcd.clear();};

if (vstup == 100 and cislo>0 and prepinac==1) 
  {lcd.setCursor(0,1 ); lcd.print("        ");delay(10); necobylozadano=0; 
  lcd.setCursor(0,0); lcd.print("Bylo nastaveno"); 
  lcd.setCursor(1,1); lcd.print(cislo); lcd.print(" hodin      ");delay(2500); lcd.clear(); a1hodina=cislo; vstup=0; cislo=0; prepinac=2;};
if (vstup == 100 and cislo>0 and prepinac==2) 
  {lcd.setCursor(0,1 ); lcd.print("        ");delay(10); necobylozadano=0; 
  lcd.setCursor(0,0); lcd.print("Bylo nastaveno"); 
  lcd.setCursor(1,1); lcd.print(cislo); lcd.print(" minut    ");delay(2500); lcd.clear(); a1minuta=cislo; vstup=0; cislo=0; prepinac=3;};
if (prepinac==3) 
  {lcd.setCursor(0,1 ); lcd.print("        ");delay(10); necobylozadano=0; 
  lcd.setCursor(0,0); lcd.print("Cas byl nastaven"); 
  lcd.setCursor(1,1); lcd.print(" na ");lcd.print(a1hodina); lcd.print((char)58);if (a1minuta<10) {lcd.print("0");}; 
  lcd.print(a1minuta);lcd.print(":");lcd.print("00   "); 
  delay(3500); lcd.clear();clock.setDateTime(rok,mesic,den,a1hodina,a1minuta,0);
  prepinac=0;budetobudit=0; vstup=0; cislo=0; funkce=5;funkce_5();};
}; 
}


void vstup_cislo()   // tento blok ceka na zadani nejakeho cisla od 0 do 9 vsechna ost.tlac. ignoruje
{ irrecv.enableIRIn(); delay(100); nactiVstupy();   
for (int i = 0; i < 1000; i++) {
if (irrecv.decode(&results)) 
{digitalWrite(13,HIGH); delay(300) ;digitalWrite(13,LOW);
if (results.value == 2014985322 or results.value == 3793052708) {vstup=0; necobylozadano=1;};
if (results.value == 1912981441 or results.value == 4215707931) {vstup=1; necobylozadano=1;}; 
if (results.value == 3527678455 or results.value == 3842848913) {vstup=2; necobylozadano=1;};
if (results.value == 3544456074 or results.value == 3792516056) {vstup=3; necobylozadano=1;};
if (results.value == 1337229932 or results.value == 2317871014) {vstup=4; necobylozadano=1;};
if (results.value == 1944219534 or results.value == 4240154468) {vstup=5; necobylozadano=1;};
if (results.value == 2499832755 or results.value == 3339622349) {vstup=6; necobylozadano=1;};
if (results.value == 2483055134 or results.value == 3389955204) {vstup=7; necobylozadano=1;};
if (results.value == 1010849180 or results.value == 3203695414) {vstup=8; necobylozadano=1;};
if (results.value == 2715176569 or results.value == 4048738311) {vstup=9; necobylozadano=1;};
if (results.value == 182820736 or results.value == 846147286)  {vstup=100; necobylozadano=1;};
}  ;
tlac9=digitalRead(9); 
if (necobylozadano==0) {i=50; irrecv.resume();delay(100);};
if (necobylozadano==1) {i=999; irrecv.resume(); delay(100);}; 
};  
}

void menu()   // tento blok pouze vypise menu
{ necobylozadano=0; cislo=0; vstup=0; clock.enableOutput(false); 
  desetiny=0; sekundy=0; minuty=0; hodiny=0;
  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.print(" Davidovo menu: "); delay(1200);
  for (int a=0; a<16; a++) {lcd.setCursor(a,0); lcd.print(" "); delay(80);};  
  lcd.setCursor(0, 0);
  lcd.print("1.mereni teploty");
  lcd.setCursor(0, 1); 
  lcd.print("2.stopky        "); delay(2000);
  for (int a=0; a<16; a++) {lcd.setCursor(a,0); lcd.print(" "); lcd.setCursor(a,1); lcd.print(" "); delay(80);};
  lcd.setCursor(0, 0);
  lcd.print("3.minutka       ");
  lcd.setCursor(0, 1); 
  lcd.print("4.nastav.budiku "); delay(2000);
for (int a=0; a<16; a++) {lcd.setCursor(a,0); lcd.print(" "); lcd.setCursor(a,1); lcd.print(" "); delay(80);};
  lcd.setCursor(0, 0);
  lcd.print("5.hodiny a budik");
  lcd.setCursor(0, 1); 
  lcd.print("6.nastaveni casu"); delay(100);

}

void checkAlarms()
{
  RTCAlarmTime a1;  
  RTCAlarmTime a2;

  if (clock.isArmed1())
  {
    a1 = clock.getAlarm1();

    switch (clock.getAlarmType1())
    {
      case DS3231_EVERY_SECOND:
        break;
      case DS3231_MATCH_S:
        break;
      case DS3231_MATCH_M_S:
        break;
      case DS3231_MATCH_H_M_S:
        budetobudit = 1; 
        break;
      case DS3231_MATCH_DT_H_M_S:
        break;
      case DS3231_MATCH_DY_H_M_S:
        break;
      default: 
        Serial.println("UNKNOWN RULE");
        break;
    }
  } else
  {  }
 }

 void budik()  
 {
  digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(200);
  digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(200);
  digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(4000);nactiVstupy();
  if (tlac9==LOW) {digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(200);
  digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(200);
  digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(4000);};
  nactiVstupy();
  if (tlac9==LOW) {digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(200);
  digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(200);
  digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(4000);};
  nactiVstupy();
  if (tlac9==LOW) {digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(200);
  digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(200);
  digitalWrite(7,HIGH); delay(100);digitalWrite(7,LOW);delay(4000);};
  } 
 


void setup()
{
  pinMode(10, INPUT); // na ovladani
  pinMode(9, INPUT_PULLUP);// start a stop u stopek
  pinMode(7, OUTPUT);  // pipatko
  pinMode(13, OUTPUT); 
  pinMode(2, INPUT_PULLUP); // impuls 1Hz z modulu ds3231
  Serial.begin(9600);

  clock.begin();
 clock.setBattery(1,0); // Dulezite nastaveni 
 clock.armAlarm1(false);
 clock.armAlarm2(false); 
 checkAlarms();  
  digitalWrite(7,LOW); 
  randomSeed(analogRead(A0));

  // disable 32kHz 
  clock.enable32kHz(false);
  // Select output as rate to 1Hz
  clock.setOutput(DS3231_1HZ);
  // Enable output
  clock.enableOutput(true);
  // clock.setAlarm1(0,18,18,0, DS3231_MATCH_H_M_S);
  irrecv.enableIRIn();
  mojeDHT.begin();
  lcd.init();           // initialize the lcd
  lcd.backlight();  
  lcd.createChar(0, bell);
  lcd.createChar(6, cross);
  lcd.home();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("*** Davidova ***"); 
  lcd.setCursor(0, 1);
  lcd.print(" multi-krabicka "); delay(3000);     
  lcd.setCursor(0, 0);  
  lcd.print(verze); 
  delay(5000);
  casovaZnacka = millis(); // vytvori casovou znacku
lcd.clear();
digitalWrite(7,LOW); 
}

void loop()
{ 
menu();  vstup_cislo();
if (vstup>6 or vstup<1) {vstup=0;
  lcd.setCursor(0, 0); lcd.print("neplatne zadani!"); lcd.setCursor(0, 1); lcd.print("zadej cislo 1-6 "); delay(2000); 
  irrecv.resume(); funkce=0; vstup_cislo();  }
if (vstup==1) {funkce=1; vstup=0;  lcd.setCursor(0, 0); lcd.print("   Teplomer     ");}  // funkce 1 - teplomer
if (vstup==2) {funkce=2; vstup=0;  lcd.setCursor(0, 0); lcd.print("    Stopky      ");}  // funkce 2 - stopky
if (vstup==3) {funkce=3; vstup=0;  lcd.setCursor(0, 0); lcd.print("   Minutka      ");}  // funkce 3 - minutka
if (vstup==4) {funkce=4; vstup=0;  lcd.setCursor(0, 0); lcd.print("  Nastav budik  ");}  // funkce 4 - nastaveni budiku
if (vstup==5) {funkce=5; vstup=0;  lcd.setCursor(0, 0); lcd.print(" Hodiny a budik ");}  // funkce 5 - cas a budik
if (vstup==6) {funkce=6; vstup=0;  lcd.setCursor(0, 0); lcd.print(" Nastaveni casu ");}
lcd.setCursor(0,1);lcd.print("                ");  

if (funkce==1) {funkce_1();};  
if (funkce==2) {funkce_2();};
if (funkce==3) {funkce_3();};
if (funkce==4) {funkce_4();};
if (funkce==5) {funkce_5();};
if (funkce==6) {funkce_6();};
  irrecv.resume(); necobylozadano=0;  
  }
