#include "ApSDM120.h"

/*
  ApSDM120.cpp - ApSDM120 Implementation file
  For instructions, go to https://github.com/AntonioPrevitali/ApSDM120
  Created by Antonio Previtali March, 2024.
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the Gnu general public license version 3
*/

ApSDM120::ApSDM120(uint8_t DeRePin, boolean NoOtherModBusLib)
{
   _DeRePin = DeRePin;
   nootherlib = NoOtherModBusLib;
   slaveadress = 1;
   Power = 0;
   Voltage = 0;
   Current = 0;
   PowFact = 0;
   AngolFas = 0;
   Frequenz = 0;
   Export = 0;
   Import = 0;
}


void  ApSDM120::SetSlaveAdress(uint8_t xslave)
{
  slaveadress = xslave;
}


void ApSDM120::BeginSerial(HardwareSerial& xserial)
{
  if (_DeRePin != NO_DERE_PIN) {
    pinMode(_DeRePin, OUTPUT);
    digitalWrite(_DeRePin, LOW);
   }
   if (nootherlib)    // per usare questa ApSDM120 assieme ad un altra libreria x modbus dove questa comunica con gli SDM120 e l'altra
   {                  // con gli altri dispositivi rs485 eventualmente presenti.
      xserial.begin(9600,SERIAL_8N2);  // modbus e sdm120 con parity = n trasmettono comunqnue un bit di parita fisso ad 1.
   }
   _serial = &xserial;
}

#ifdef __AVR__
void ApSDM120::BeginSerial(SoftwareSerial& xserial)
{
  if (_DeRePin != NO_DERE_PIN) {
    pinMode(_DeRePin, OUTPUT);
    digitalWrite(_DeRePin, LOW);
   }
   if (nootherlib)
   {
     xserial.begin(9600);   // x softwareserial farà 8N1 che comunque funziona...
   }
   _serial = &xserial;
}
#endif

uint16_t ApSDM120::calculateCrc(byte* data, size_t size)   // questa la ho copiata !
{
    uint16_t _crc;
    _crc = 0xFFFF;
    for (size_t i = 0; i < size; i++) {
        _crc ^= data[i];
        for (byte j = 0; j < 8; j++) {
            if (_crc & 0x0001)
                _crc = (_crc >> 1) ^ 0xA001; //< fixed polynomial
            else
                _crc = _crc >> 1;
        }
    }
    return _crc >> 8 | _crc << 8;
}


float ApSDM120::bytesToFloat(byte* regData)  // pure questa copiata !
{
    float result = NAN;
    for (int8_t i = 3; i >= 0; i--) {
        ((uint8_t*)&result)[i] = regData[3 - i];
    }
    return result;
}


boolean ApSDM120::okTranza(uint8_t xreq[])
{
  uint16_t ilcrc;
  if (slaveadress != 1)
  {
    xreq[0] = slaveadress;
    ilcrc = calculateCrc(xreq,6);
    xreq[6] =  highByte(ilcrc);
    xreq[7] = lowByte(ilcrc);
  }
  while (_serial->available()) _buffer[0] = _serial->read(); // pulisce eventuali skifezze nel buffer rx.
  if (_DeRePin != NO_DERE_PIN) digitalWrite(_DeRePin, HIGH);
  _serial->write(xreq, 8);
  _serial->flush();
  if (_DeRePin != NO_DERE_PIN) digitalWrite(_DeRePin, LOW);
  if (!waitResponse()) return false;
  return true;
}

boolean ApSDM120::okReadPower(void)
{
  uint8_t xreq[] = {1, 4, 0, 0x0C, 0, 2, 0xB1,0xC8};
  Power = 0;
  if (!okTranza(xreq)) return false;
  Power = bytesToFloat(_buffer+3);
  return true;
}

boolean ApSDM120::okReadVoltage(void)
{
  uint8_t xreq[] = {1, 4, 0, 0, 0, 2, 0x71, 0xCB};
  Voltage = 0;
  if (!okTranza(xreq)) return false;
  Voltage = bytesToFloat(_buffer+3);
  return true;
}


boolean ApSDM120::okReadCurrent(void)
{
  uint8_t xreq[] = {1, 4, 0, 6, 0, 2, 0x91, 0xCA};
  Current = 0;
  if (!okTranza(xreq)) return false;
  Current = bytesToFloat(_buffer+3);
  return true;
}



boolean ApSDM120::okReadPowFact(void)
{
  uint8_t xreq[] = {1, 4, 0, 0x1E, 0, 2, 0x11, 0xCD};
  PowFact = 0;
  if (!okTranza(xreq)) return false;
  PowFact = bytesToFloat(_buffer+3);
  return true;
}

boolean ApSDM120::okReadAngolFas(void)
{
  uint8_t xreq[] = {1, 4, 0, 0x24, 0, 2, 0x31, 0xC0};
  AngolFas = 0;
  if (!okTranza(xreq)) return false;
  AngolFas = bytesToFloat(_buffer+3);
  return true;
}


boolean ApSDM120::okReadFrequenz(void)
{
  uint8_t xreq[] = {1, 4, 0, 0x46, 0, 2, 0x90, 0x1E};
  Frequenz = 0;
  if (!okTranza(xreq)) return false;
  Frequenz = bytesToFloat(_buffer+3);
  return true;
}

boolean ApSDM120::okReadExport(void)
{
  uint8_t xreq[] = {1, 4, 0, 0x4A, 0, 2, 0x50, 0x1D};
  Export = 0;
  if (!okTranza(xreq)) return false;
  Export = bytesToFloat(_buffer+3);
  return true;
}


boolean ApSDM120::okReadImport(void)
{
  uint8_t xreq[] = {1, 4, 0, 0x48, 0, 2, 0xF1, 0xDD};
  Import = 0;
  if (!okTranza(xreq)) return false;
  Import = bytesToFloat(_buffer+3);
  return true;
}


boolean ApSDM120::waitResponse(void)
{
  unsigned long startTimeMs = millis();
  unsigned long startTimeus;
  uint16_t ilcrc;
  uint8_t numBytes = 0;
  while (!_serial->available()) {
    if (millis() - startTimeMs >= 36) {  // 36 ms di timeout mediamente a 9600 baud ne servono 18ms...
      return false;
    }
  }
  do {
    do {
      if (_serial->available()) {
        startTimeus = micros();
        _buffer[numBytes] = _serial->read();
        numBytes++;
      }
    } while (micros() - startTimeus <= 2870 && numBytes < 9);   // 2870 tempo massimo tra un carattere e l'altro i caratteri devono arrivare uno di seguito all'altro con un certo ritmo imposto dal modbus
    if (numBytes < 9) // cioè uscito dal do per fuori ritmo..
    {
      numBytes = 0;   // caratteri arrivati prima sono rumore o skifezze li butta via !
    }
  } while (millis() - startTimeMs <= 36 && numBytes < 9);
  if (numBytes < 9)
  {
    // non sono arrivati tutti i caratteri della risposta quindi non va bene ritorna false.
    return false;
  }
  // ok vediamo se il crc è giusto
  ilcrc = calculateCrc(_buffer,7);
  if (_buffer[7] != highByte(ilcrc) || _buffer[8] != lowByte(ilcrc) )
  {
     return false;  // crc errato !
  }
  // vediamo anche se questi sono giusti !
  if (_buffer[0] != slaveadress || _buffer[1] != 4 || _buffer[2] != 4) return false;
  return true; // ok è giusto !
}

