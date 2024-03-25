# ApSDM120
Library for interfacing meter SDM120 with Arduino

A direct approach to the meter is used without the need to know the Modbus protocol, an approach paradoxically more robust than some Modbus libraries I tried before writing this!

![ApSDM120](doc/IMG_00_presentazione.jpg "ApSDM120")

## Getting Started

very simple !

```c++
#include <ApSDM120.h>

ApSDM120  MySDM120(NO_DERE_PIN, NO_OTHER_MODBUS_LIB);

void setup() {

    MySDM120.BeginSerial(Serial1);   // usa la serial1 e la imposta a 9600 N 8 2 usabile anche con SoftwareSerial
    MySDM120.SetSlaveAdress(1);      // 1 è indirizzo di default di un SDM120 appena comprato.
    
    Serial.begin(115200); // è questa invece è la seriale per lo standard output di arduino usata per visualizzare dati sul pc e per caricare software.
    
}


void loop() {

    Serial.println("------------------------");
    Serial.print("Power   = ");
    if ( MySDM120.okReadPower() )
    {
      Serial.println(MySDM120.Power);
    }
    else  Serial.println("Error");
    delay(100);   // distanziare sempre le richieste non inviare mai richieste a raffica al misuratore...
  
    Serial.print("Voltage = ");
    if ( MySDM120.okReadVoltage() )
    {
      Serial.println(MySDM120.Voltage);
    }
    else  Serial.println("Error");
    delay(100);

    Serial.print("Current = ");
    if ( MySDM120.okReadCurrent() )
    {
      Serial.println(MySDM120.Current);
    }
    else  Serial.println("Error");
    delay(100);

    Serial.print("PowFact = ");
    if ( MySDM120.okReadPowFact() )
    {
      Serial.println(MySDM120.PowFact);
    }
    else  Serial.println("Error");
    delay(100);

    Serial.print("AngolFas = ");
    if ( MySDM120.okReadAngolFas() )
    {
      Serial.println(MySDM120.AngolFas);
    }
    else  Serial.println("Error");
    delay(100);

    Serial.print("Frequenz = ");
    if ( MySDM120.okReadFrequenz() )
    {
      Serial.println(MySDM120.Frequenz);
    }
    else  Serial.println("Error");
    delay(100);

    Serial.print("Export = ");
    if ( MySDM120.okReadExport() )
    {
      Serial.println(MySDM120.Export);
    }
    else  Serial.println("Error");
    delay(100);

    Serial.print("Import = ");
    if ( MySDM120.okReadImport() )
    {
      Serial.println(MySDM120.Import);
    }
    else  Serial.println("Error");
    delay(100);

    // ok next loop
    delay(1000);
}
```
```
Output is:
------------------------
Power   = 84.20
Voltage = 242.70
Current = 0.34
PowFact = 1.00
AngolFas = 0.00
Frequenz = 50.00
Export = 0.00
Import = 0.01
------------------------
```
## For further information see the doc folder


