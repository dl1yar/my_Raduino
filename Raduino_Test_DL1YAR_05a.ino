/*
 * Testprogramm für Raduino de DL1YAR begonnen Mai 2019
 * Basis ist der Raduino zum BITX40
 * angepasst an den vorhandenen TRX
 */
#include "Raduino_PortPinUser.h"
#include <LiquidCrystal.h>
#include "PriUint64.h"
#include <Wire.h>

#include <si5351.h>

#include <EEPROM.h>

#define NOP __asm__ __volatile__ ("nop\n\t")


const int testpin = dreh_b;
int dreh_alt =0;
int ee_cal =0;      //Speicherort EEPROM
int vfo,vs,band_i;

uint64_t frequency; // Startfrequenz in Hz

uint32_t clk0_old,clk1_old,clk2_old;

uint64_t  iffreq;// = Zf + mode_diff[mode]; // Zf-Frquenz in Hz
long corr = 10; // this is the correction factor for the Si5351, use calibration sketch to find value.

//******************

 
#define my_ver "Raduinotest 0.5a" 

//******************
 LiquidCrystal lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

//Si5351 si5351;
Si5351 si5351;


//******************************************
void Taste_los(){ //wartet auf lösen der Tasten
  do {
  delay (5);
  } while (analogRead(mode_taste) <=taste_w[0]);
}
//******************************************
void Mode_sw(){
  //CALi;CwL,CwU,LSB,USB,
  mode = mode+1; 
  if (mode == mode_max +1) mode =1;//wegen CALIBREATEMODUS ***********
  Taste_los();
}

void Mode_vfo(){
  vfo_m = vfo_m +1; 
  if (vfo_m == vfo_max +1) vfo_m =0;
  //Serial.print("VFO mode2 ");Serial.print(vfo_m);Serial.println(vfo_[vfo_m]);
  Taste_los(); 
}

//******************************************
void Band_sw(){
  //40m 50kHz Segmente
  band_i =band_i+1;
  if(band_i == band_max +1){band_i =0;}
  frequency= band_seg[band_i];
  /*
  Serial.print("band_i ");Serial.println(band_i);
  Serial.print("QRG ");//Serial.println(frequency);
  Serial.print(PriUint64<DEC>(frequency));
  Serial.println(" ");
  */
  Taste_los();
}

void Mode_sw_(){ 
  mode  = mode +1;
  if (mode >= mode_max) {mode = 0;}
 //Serial.print("VFO mode2 ");Serial.print(vfo_m);Serial.println(vfo_[vfo_m]);
   Taste_los(); 

}

void vfo_step(){
 vfo_sw = vfo_sw +1;
  if (vfo_sw >= vfo_ma) {vfo_sw = 0;}
  lcd.setCursor(0,1); lcd.print("     ");lcd.setCursor(0,1);lcd.print((long)vfo_schritt[vfo_sw]);
 // Serial.print("VFO step ");Serial.print(vfo_sw);Serial.print(" ");Serial.print(vfo_ma);Serial.print(" ");Serial.println(PriUint64<DEC>(vfo_schritt[vfo_sw])); 
  Taste_los(); 
}

//******************************************
void Tastenabfrage(){
    //if (analogRead(mode_taste)>= 850){ mode =mode;}    
  if (analogRead(mode_taste)>=  taste_w[0]-5 ){ NOP;}
    else if (analogRead(mode_taste)<=(taste_w[1]+10))  {Mode_sw();}//{mode =0;}
    else if (analogRead(mode_taste)<=(taste_w[2]+10))  {Band_sw();}//{mode =1;}
    else if (analogRead(mode_taste)<=(taste_w[3]+10))  {Mode_vfo();} //platz für weitere Tasten
    else if (analogRead(mode_taste)<=(taste_w[4]+10))  {vfo_step();}
     //else mode= 4;
     
  }
  
//******************************************
void LcdAusgabe(){
    lcd.setCursor(13, 1);   //lcd.begin(Spalte,Zeile)
    lcd.print(Mode[mode]);
    lcd.setCursor(0,0); 
    lcd.print((long)frequency);
    lcd.setCursor(6,1);
    lcd.print(vfo_[vfo_m]);

  
}
//******************************************
void DDS_Ausgabe(){
  // Ausgabe Frequenz an Si5351
  if(clk2_old != iffreq){  //Änderungen ??
  si5351.set_freq(iffreq * 100ULL, SI5351_CLK2);// Abstimmbarere BFO!!
   //     Serial.print("iffreq ");Serial.println(PriUint64<DEC>(iffreq)); //TEST************
  clk2_old = iffreq;
  }      

  if(clk0_old !=frequency){
  //si5351.set_freq((frequency + iffreq) * 100ULL, SI5351_CLK0);
   si5351.set_freq((frequency ) * 100ULL, SI5351_CLK0);
    //Serial.print("frequency ");Serial.println(PriUint64<DEC>(frequency)); //TEST************
   clk0_old = frequency;
  }
}

//******************************************
void dreha(){
  //Softwareentprellung
  detachInterrupt(digitalPinToInterrupt(dreh_a));// Interrupt abschalten
  delay(3);
  if ( dreh_alt != digitalRead(dreh_a)){
    if (digitalRead(dreh_a) !=digitalRead(dreh_b)) {
         frequency = frequency -vfo_schritt[vfo_sw];
        }
    if (digitalRead(dreh_a) ==digitalRead(dreh_b)){ 
        frequency = frequency +vfo_schritt[vfo_sw];
        }
     dreh_alt = digitalRead(dreh_a);//aktuellen Wert sichern!
  }  
   attachInterrupt(digitalPinToInterrupt(dreh_a), dreha, CHANGE);

}


void EEPROMWritelong(int address, long value){
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address){
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}
//******************************************
/*
 * //Portpin toggeln zum Test der Verbindungen
 
//******************************************
void testport(){
const int testpin = dreh_b;
 pinMode(testpin,OUTPUT);                 //******************************************
 digitalWrite(testpin, !digitalRead(testpin));//Portpin toggeln zum Test der Verbindungen
   Serial.println(analogRead(mode_taste));//****************************************
  delay(222);
}
*/
//----------------------------------------------
void call_cal(){
  cal_factor = (int32_t)(target_freq - frequency) + old_cal;
    si5351.set_correction(cal_factor, SI5351_PLL_INPUT_XO);
    si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
    si5351.pll_reset(SI5351_PLLA);
    si5351.set_freq(target_freq, SI5351_CLK0);
    lcd.setCursor(0,1);    lcd.print((long)cal_factor);
    EEPROMWritelong(ee_cal, cal_factor);
}
//----------------------------------------------
static void vfo_interface(void)
{
  frequency = target_freq;
  cal_factor = old_cal;
      lcd.clear(); 
  while (1)
  {
    lcd.setCursor(0,0);    lcd.print((long)frequency);
    if (analogRead(mode_taste)<=(taste_w[1]+10))  {call_cal();}
    else if (analogRead(mode_taste)<=(taste_w[4]+10))  {vfo_step();}
    
  }
}
//----------------------------------------------
/*static void flush_input(void)
{
  while (Serial.available() > 0)
  Serial.read();
}
*/
//----------------------------------------------
void callibrate(){

Serial.println("Callibrieren"); 
Serial.println(EEPROMReadlong(ee_cal));// zeigt CALwert im EEPROM
//uint64_t rx_freq;frequency

    // The crystal load value needs to match in order to have an accurate calibration
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);

  // Start on target frequency
  si5351.set_correction(cal_factor, SI5351_PLL_INPUT_XO);
  si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
  si5351.set_freq(target_freq, SI5351_CLK0);

  //LOOP??????
   si5351.update_status();
  if (si5351.dev_status.SYS_INIT == 1)
  {
     Serial.println(F("Initialising Si5351, Du solltest nur einen sehen!"));
     delay(500);
  }
  else
  {
    Serial.println();
    Serial.println(F("Stellen Sie ein, bis Ihr Frequenzzähler so nahe wie möglich an 10 MHz liegt."));
    Serial.println(F("Press 'q' when complete."));
    vfo_interface();
  }

 
}
//******************************************
//******************************************
void setup() {
 
 lcd.begin(16, 2);//lcd.begin
 lcd.print(mycall);
 lcd.print(" Juni 2019 ");
 lcd.setCursor(0,1);
 lcd.print(my_ver);
 
 //Grundeistellungen****************
 frequency = band_seg[1];//Bandsegment
 vfo_sw=4;                //Schrittweite
 dreh_alt = digitalRead(dreh_a);//Drehgeber
  
 corr = EEPROMReadlong(ee_cal);         //holt den Korekturwert aus dem EEPROM 
 si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, corr);

    pinMode(dreh_a, INPUT);       //Eingang Drehgeber
    //    digitalWrite(dreh_a, HIGH);// Pullup an
    pinMode(dreh_b, INPUT);       //Eingang Drehgeber
    //    digitalWrite(dreh_b, HIGH);// Pullup an

 attachInterrupt(digitalPinToInterrupt(dreh_a), dreha, CHANGE);
 
    Serial.begin(baud);
    Serial.println(my_ver);
    Serial.println(mycall);   
//Serial.println(EEPROMReadlong(ee_cal));// zeigt CALwert im EEPROM
       mode =2;
       if ((analogRead(mode_taste) <=taste_w[0]) ){ mode =0;callibrate();}  // Tastendruck beim Einschaten ab zum Callibrieren!!   
        delay(2500);
  lcd.clear(); 
  vfo_sw =2;
       vfo_step();
  //    vs =3;
}
//******************************************
//******************************************
//******************************************
void loop() {
  // zum Testen
//testport();delay(10);

iffreq = Zf + mode_diff[mode];
  Tastenabfrage();
  DDS_Ausgabe();
  LcdAusgabe();
    
}
