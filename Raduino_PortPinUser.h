/*
Die Raduino-Platine hat die Größe eines Standard-16x2-LCD-Panels. Es hat drei Anschlüsse:
   Erstens ist ein 8-poliger Anschluss vorhanden, der +5 V, GND und sechs analoge Eingangspins bietet, die auch verwendet werden können
   konfiguriert, um als digitale Eingangs- oder Ausgangspins verwendet zu werden. Diese werden als A0, A1, A2,
   Stifte A3, A6 und A7. Die Pins A4 und A5 fehlen an diesem Anschluss wie gewohnt
   Sprechen Sie mit dem Si5351 über das I2C-Protokoll.
    A0 A1 A2 A3 GND + 5V A6 A7
   SCHWARZ BRAUN ROT ORANGE GELB GRÜN BLAU VIOLETT (gleiche Farbcodierung wie für Widerstände)
   Zweitens ist ein 16-poliger LCD-Anschluss. Dieser Anschluss ist speziell für den Standard 16x2 vorgesehen
   LCD-Anzeige im 4-Bit-Modus. Für den 4-Bit-Modus sind 4 Datenleitungen und zwei Steuerleitungen erforderlich:
   Verwendete Zeilen sind: RESET, ENABLE, D4, D5, D6, D7
   Wir fügen die Bibliothek hinzu und deklarieren auch die Konfiguration des LCD-Panels
   */
  // LiquidCrystal(rs, enable, d4, d5, d6, d7) 
  // LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
  #define lcd_rs (8)
  #define lcd_en (9)
  #define lcd_d4 (10)
  #define lcd_d5 (11)
  #define lcd_d6 (12)
  #define lcd_d7 (13)
  /*
  Wir müssen die Pinbelegung für verschiedene Zwecke sorgfältig auswählen.
   Es gibt zwei Sätze vollständig programmierbarer Stifte am Raduino.
   Auf der Oberseite der Platine befindet sich in Übereinstimmung mit dem LCD-Anschluss ein 8-poliger Anschluss
   Das ist hauptsächlich für analoge Eingänge und die Steuerung auf der Frontplatte gedacht. Es hat einen geregelten 5V Ausgang,
   Boden und sechs Stifte. Jeder dieser sechs Pins kann individuell programmiert werden
   entweder als analoger Eingang, digitaler Eingang oder digitaler Ausgang.
   Die Pins sind wie folgt belegt:
          A0, A1, A2, A3, GND, +5 V, A6, A7
       Pin 8 7 6 5 4 3 2 1 (Stecker P1)
        SCHWARZ BRAUN ROT ORANGE GELB GRÜN BLAU VIOLETT
        (Halten Sie das Board hoch, sodass die Rückseite des Boards Ihnen zugewandt ist.)
   Obwohl dies sowieso zugewiesen werden kann, werden wir für diese Anwendung des Arduino Folgendes machen
   Zuordnung:
   A0 (digitaler Eingang) zur Erkennung der PTT. An den Ausgang von U3 (LM7805) des BITX40 anschließen.
      Auf diese Weise erhält der A0-Eingang 0 V (LOW), wenn die PTT-Taste nicht gedrückt wird, und + 5 V (HIGH), wenn die PTT-Taste gedrückt wird.
   A1 (digitaler Eingang) dient zum Anschließen an eine gerade Taste oder an den 'Dit'-Kontakt eines Paddle-Keyers. Öffnen (HOCH) während des Hochfahrens, schalten Sie während des Herunterfahrens auf Masse (NIEDRIG).
   A2 (digitaler Eingang) kann zur Kalibrierung verwendet werden, indem diese Leitung geerdet wird (nicht erforderlich, wenn Sie die Funktionstaste bei A3 haben).
   A3 (digitaler Eingang) ist mit einem Taster verbunden, der diese Leitung kurzzeitig erden kann. Diese Funktionstaste wird zum Umschalten zwischen verschiedenen Modi usw. verwendet.
   A4 (wird bereits für Gespräche mit dem SI5351 verwendet)
   A5 (wird bereits für Gespräche mit dem SI5351 verwendet)
   A6 (Analogeingang) wird derzeit nicht verwendet
   A7 (Analogeingang) wird mit den beiden anderen Enden an einen Mittelstift eines 100K- oder 10K-Linearpotentiometers guter Qualität angeschlossen
       Masse- und +5 V-Leitungen sind am Stecker verfügbar. Dies implementiert den Abstimmungsmechanismus.!!!!!!
    

 Der zweite Satz von 16 Pins am unteren Anschluss P3 verfügt über die drei Taktausgänge und die digitalen Leitungen zur Steuerung des Rigs.
 Diese Zuordnung ist wie folgt:
 Pin 1    2     3   4    5   6    7   8   9   10  11 12 13 14 15 16 (Stecker P3)
   +12 V +12 V CLK2 GND GND CLK1 GND GND CLK0 GND D2 D3 D4 D5 D6 D7
     Auch diese sind flexibel, was Sie damit machen können. Für den Raduino verwenden wir sie, um:
     Ausgang D2 - PULSE: wird für den kapazitiven Touchkeyer verwendet
     Eingang D3 - DAH: wird mit dem 'Dah'-Kontakt eines Paddle-Keyers verbunden (Schalter auf Masse).
     Eingang D4 - SPOT: ist mit einem Taster verbunden, der diese Leitung kurzzeitig erden kann. Wenn die SPOT-Taste gedrückt wird, wird ein Mithörton für die Null-Takt-Abstimmung erzeugt.
     Ausgang D5 - CW_TONE: Seitentonausgabe
     Ausgang D6 - Leitung CW_CARRIER: Schaltet den Träger für CW ein
     Ausgang D7 - TX_RX Leitung: Schaltet im CW-Modus zwischen Senden und Empfangen um
     */
/*
#define PTT_SENSE (A0)
#define KEY (A1)
#define FBUTTON (A3)
#define CLARIFIER (A6)
#define ANALOG_TUNING (A7)
*/
//******************************************************************************************
// ab hier persönliche Anpassungen

#define mycall "DL1YAR"

#define baud 9600     //Baudrate

#define my_ver "Raduinotest 0.5a" 

//40m Band in Hz

const int dreh_a = 2;
const int dreh_b = 3;

#define mode_taste (A7) //juni19 angepasst

int taste_w[] ={810,0,507,680,765};//Werte anpassen mit Testprogramm ermitteln

int mode, mode_max = 4;// *Mode +1 !!
char *Mode [] = {"CALi","CwU","CwL","USB","LSB"};

uint64_t mode_diff[] ={0,800,-800,2000,-2000};//BFO-Shift in Hz TESTEN************
uint64_t Zf =12000000;// in Hz

int vfo_max = 4, vfo_m;
char *vfo_ [] ={"Rx==Tx","Rx!=Tx","Tx!=Rx","  Rx  "," Tx  "};

int band,band_max = 3; // max bandabschnitte 160m .... 2m  band_seg +1
uint64_t band_seg[] ={7000000,7050000,7100000,7160000};// hier nur zum TESTEN************

uint64_t vfo_schritt[] ={1,5,10,20,50,100,500,1000,2000,5000};//in Hz
int vfo_sw,vfo_ma =10;

//------------------------------------------
//------------------------------------------
uint64_t target_freq = 1000000000ULL; // 10 MHz, in hundredths of hertz CALIBRATION----------
int32_t cal_factor = 0;
int32_t old_cal = 0;
//******************************************
//#define testpin 32
