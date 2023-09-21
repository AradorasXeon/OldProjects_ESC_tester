/*
 * Alpok innen:
        Arduino Brushless Motor Control
     by Dejan, https://howtomechatronics.com
*/

#include <Servo.h>


//Voltage measuring
//const uint16_t CRITICALL_CELL_VOLTAGE_1 = 836; //~ 3,73 V, ha Aref 4,57 !!!!!!!!!!!!!!!!!!!!!!!!!
const uint16_t CRITICALL_CELL_VOLTAGE_PER_DIVIDED_CELL = 232; //~leosztott feszültség, ami megfelel a 3,73 V-nak
const byte CRITICAL_CELL_VOLTAGE_MAX_COUNT = 20;
//ha Aref = 4,57 V !!!!!!!!!!!!!!!!!!!!!!!!!
//végső projektnél majd teszteld, hogy annyi-e (valszeg felkúszik 5-re, akkor excelben ezeket
//ÁT KELL SZÁMOLNI!

bool eStop;
byte eCounter = 0;

Servo ESC1;     // create object to control the ESC
Servo ESC2;
Servo ESC3;
Servo ESC4;

uint16_t potValue;  // value from the analog pin

void setup() 
{
  Serial.begin(9600);
   //********************************************************
  //A feszültség méréshez nem kell ide semmi különöset tenni.
  //********************************************************
  // Attach the ESC on pin 9
  ESC1.attach(3,1000,2000); // (pin, min pulse width, max pulse width in microseconds) 
  ESC2.attach(5,1000,2000);
  ESC3.attach(6,1000,2000);
  ESC4.attach(9,1000,2000);

  //Mielőtt bármit csinálna kb. 6 másodperc, amíg a motor betölt
  
  //delay(5);
  //kikommentelve tesztből, hogy ESC-k megjegyezték-e a kalibráltsági állapotukat
  //ESC kalibrálás menete annak aki esetleg nem tudná:
  //bekapcsolás előtt maxra kell állítani a "gáz"-t
  //bekapcsolod
  //vársz kb 2 másodpercet //gyártónként változhat
  // gázt minimumra veszed
  //ESC bekalibrálva
  // lejebb azért van 6,5 másodperc, mert áram ellátás után kell egy kis idő az ESC-nek amíg magához tér

  //*****************Ki kommentelni innentől ************************************

  /*
  ESC1.write(180);
  ESC2.write(180);
  ESC3.write(180);
  ESC4.write(180);
  delay(6500); //hozzáadva 6000 ennyivel faszán tud kalibrálni, csak arra kell figyelni, h
  //**************************** IDÁIG *********************************
  //az arduino a BEC-ről kapjon áramot, ez a szám erre van kiszámolva!
  //CSAK AKKOR MŰKÖDIK, ha az ESC BEC ágáról kap áramot az arduino
  //ha az ESC előbb kap áramot, mint a vezérlő a kalibrálhatóság ment a levesbe
*/
  
  //Tesztelve 2022 04 08 20:49-kor az ESC-k tényleg nem felejtik el mire vannak kalibrálva
  //3-ból 3 egyszerre pörög fel :) 
  //Ez már ne legyen kikommentelve:
  ESC1.write(0);
  ESC2.write(0);
  ESC3.write(0);
  ESC4.write(0);
  
  //Ha végeztünk a kalibrálással kommenteljük ki a 180-as részeket
  //Aztán töltsük fel újra a kódot!
}

void loop() 
{
  if (eCounter > 250)
  {
    eCounter = CRITICAL_CELL_VOLTAGE_MAX_COUNT+1;
  }
  potValue = 0;
  potValue = analogRead(A7);   // reads the value of the potentiometer (value between 0 and 1023)
  potValue = map(potValue, 0, 1023, 0, 180); //max 180  // scale it to use it with the servo library (value between 0 and 180)
  
  if(eStop) //ha nincs bedugva cellefesz mérő addig kikommentelem
  {
    potValue = 0;
    //Serial.println("eStop");
  }
  

  Serial.print(potValue); //ln
  Serial.print("\t");
  
  ESC1.write(potValue);    // Send the signal to the ESC
  ESC2.write(potValue);
  ESC3.write(potValue);
  ESC4.write(potValue);

  //Feszültség mérés, ha kell vészleállítás a következő körben
   

    //Serialra kiírás
    //Serial.print(F("Voltage: \t"));

    if (analogRead(0)< CRITICALL_CELL_VOLTAGE_PER_DIVIDED_CELL)
    {
      eCounter++;
      //Serial.print("eCounter: ");
      //Serial.println(eCounter);
      if(eCounter> CRITICAL_CELL_VOLTAGE_MAX_COUNT)
      {
        EmergencyStop(0);
      }
    }
    Serial.print(analogRead(0)); //Ha csak az analogRead()-et használod, akkor nem kell kiírni, hogy A0, A1.. elég a: 0,1,... 
    Serial.print("\t");
    
    for (byte i = 0; i<3; i++)
    {
      if (analogRead(i+1) - analogRead(i) < CRITICALL_CELL_VOLTAGE_PER_DIVIDED_CELL && eCounter> CRITICAL_CELL_VOLTAGE_MAX_COUNT)
      {
        eCounter++;
        //Serial.print("eCounter: ");
        //Serial.println(eCounter);
      if(eCounter> CRITICAL_CELL_VOLTAGE_MAX_COUNT)
      {
        EmergencyStop(i+1);
      }
      }
      //Serial.print(analogRead(i+1)); //Ha csak az analogRead()-et használod, akkor nem kell kiírni, hogy A0, A1.. elég a: 0,1,... 
      Serial.print(analogRead(i+1)- analogRead(i));
      Serial.print("\t");
    }

    
    
    Serial.print("\n\r");
}


//Feszültségméréshez
void EmergencyStop(byte cellNUM) //ezt kiirattam serail monitoron tesztelésnél
{
  eStop = true;
  //Serial.print("Emergency Stop on cell ");
  //Serial.println(cellNUM);
}