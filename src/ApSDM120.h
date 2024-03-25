#ifndef ApSDM120_h
#define ApSDM120_h

/*
  ApSDM120.h - ApSDM120 include file
  For instructions, go to https://github.com/AntonioPrevitali/ApSDM120
  Created by Antonio Previtali March, 2024.
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the Gnu general public license version 3
*/

#include "Arduino.h"

#define NO_DERE_PIN 255
#define NO_OTHER_MODBUS_LIB true
#define WITH_OTHER_MODBUS_LIB false

#ifdef __AVR__
#include <SoftwareSerial.h>
#endif

class ApSDM120 {
  public:
    ApSDM120(uint8_t DeRePin = NO_DERE_PIN, boolean NoOtherModBusLib = NO_OTHER_MODBUS_LIB );
    void BeginSerial(HardwareSerial& xserial);
    #ifdef __AVR__
    void BeginSerial(SoftwareSerial& xserial);
    #endif
    void    SetSlaveAdress(uint8_t xslave);
    boolean okReadPower(void);
    boolean okReadVoltage(void);
    boolean okReadCurrent(void);
    boolean okReadPowFact(void);
    boolean okReadAngolFas(void);
    boolean okReadFrequenz(void);
    boolean okReadExport(void);
    boolean okReadImport(void);
    
    float   Power;      // potenza, negativa se inviata in rete.
    float   Voltage;    // volt...
    float   Current;    // Ampere...
    float   PowFact;    // fattore di potenza
    float   AngolFas;   // Angolo di fase in gradi.
    float   Frequenz;   // Frequenza
    float   Export;     // energia kwh totali inviati in rete.
    float   Import;     // energia kwh totali comprati da rete.

  private:
    boolean  waitResponse(void);
    uint16_t calculateCrc(byte* data, size_t size);
    float    bytesToFloat(byte* regData);
    boolean  okTranza(uint8_t xreq[]);

    boolean  nootherlib;
    uint8_t  slaveadress;     // di default SDM120 viene venduto con slave adress 1
  
    Stream *_serial;
    uint8_t _DeRePin;
    uint8_t _buffer[9];
    

};

#endif
