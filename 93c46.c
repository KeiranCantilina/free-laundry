/*******************************************
 * eeprom 93c46 driver
 ******************************************/

#include <Arduino.h>


#define SS 1
#define MOSI 2
#define MISO 3
#define SCK 4

extern uint16_t dataword;

// Basic SPI init
void eeprom_init(void) {
  // configs port B pins as outputs
  //DDRB |= (1 << MOSI) | (1 << SCK) | (1 << SS);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(SS, OUTPUT);
  pinMode(MISO, INPUT);
}

void strobe_sck(void) {
  // Make SCK pin HI
  //PORTB |= (1 << SCK);
  digitalWrite(SCK, HIGH);

  // 1ms delay
  //_delay_ms(1);
  delay(1);

  // Make SCK pin LO
  //PORTB &= ~(1 << SCK);
  digitalWrite(SCK, LOW);
}

void eeprom_opcode(uint8_t code) {
  // Set SS pin LO
  //PORTB &= ~(1 << SS);
  digitalWrite(SS, LOW);

  strobe_sck();

  // Set SS HI and MOSI HI
  //PORTB |= (1 << SS) | (1 << MOSI);
  digitalWrite(SS, HIGH);
  digitalWrite(MOSI, HIGH);

  strobe_sck();

  //if code & 0x02
  //if (code & 0x02) PORTB |= (1 << MOSI); // MOSI HI
  if (code & 0x02){
    digitalWrite(MOSI, HIGH);
  }
  //else PORTB &= ~(1 << MOSI); // MOSI LO
  else{
    digitalWrite(MOSI, LOW);
  }

  strobe_sck();

  // If code & 0x01
  //if (code & 0x01) PORTB |= (1 << MOSI); // MOSI HI
  if (code & 0x01){
    digitalWrite(MOSI, HIGH);
  }
  //else PORTB &= ~(1 << MOSI); // MOSI LO
  else{
    digitalWrite(MOSI, LOW);
  }
  strobe_sck();
}

void eeprom_address(uint8_t address) {
  for (uint8_t i = 0; i <= 5; i++) {
    //if (address & 0x20) PORTB |= (1 << MOSI); // MOSI HI
    if (address & 0x20){
      digitalWrite(MOSI, HIGH);
    }
    //else PORTB &= ~(1 << MOSI); // MOSI LO
    else{
      digitalWrite(MOSI, LOW);
    }
    strobe_sck();
    address = address << 1;
  }
}

void eeprom_erase(uint8_t address) {
  eeprom_opcode(0x03);
  eeprom_address(address);
  //PORTB &= ~(1 << SS); // SS LO
  digitalWrite(SS, LOW);
}

void eeprom_eraseall(void) {
  eeprom_opcode(0x00);
  eeprom_address(0x20);
  //PORTB &= ~(1 << SS); // SS LO
  digitalWrite(SS, LOW);
}

void eeprom_ew_disable(void) {
  eeprom_opcode(0x00);
  eeprom_address(0x00);
  //PORTB &= ~(1 << SS); // SS LO
  digitalWrite(SS, LOW);
}

void eeprom_ew_enable(void) {
  eeprom_opcode(0x00);
  eeprom_address(0x30);
  //PORTB &= ~(1 << SS); // SS LO
  digitalWrite(SS, LOW);
}

void eeprom_read(uint8_t address) {
  uint16_t divider = 0x8000;
  eeprom_opcode(0x02);
  eeprom_address(address);
  dataword = 0x00;
  strobe_sck();
  for (uint8_t i = 0; i <= 15; i++) {
    //if (PINB & (1 << MISO)) dataword = dataword + divider; // if MISO is HI, dataword = dataword + divider
    if (digitalRead(MISO)){
      dataword = dataword + divider;
    }
    divider = divider >> 1;
    strobe_sck();
  }
  //PORTB &= ~(1 << SS); // SS LOW
  digitalWrite(SS, LOW);
}

void eeprom_write(uint8_t address, uint16_t data) {
  eeprom_opcode(0x01);
  eeprom_address(address);
  for (uint8_t i = 0; i <= 15; i++) {
    //if (data & 0x8000) PORTB |= (1 << MOSI); // if (condition), MOSI HI
    if (data & 0x8000){
      digitalWrite(MOSI, HIGH);
    }
    //else PORTB &= ~(1 << MOSI); // else MOSI LO
    else{
      digitalWrite(MOSI, LOW);
    }
    strobe_sck();
    data = data << 1;
  }
  //PORTB &= ~(1 << SS); // SS LO
  digitalWrite(SS, LOW);
}

void eeprom_writeall(uint16_t data) {
  eeprom_opcode(0x00);
  eeprom_address(0x10);
  for (uint8_t i = 0; i <= 15; i++) {
    //if (data & 0x8000) PORTB |= (1 << MOSI); // MOSI HI
    if (data & 0x8000){
      digitalWrite(MOSI, HIGH);
    }
    //else PORTB &= ~(1 << MOSI); // MOSI LOW
    else{
      digitalWrite(MOSI, LOW);
    }
    strobe_sck();
    data = data << 1;
  }
  //PORTB &= ~(1 << SS); // SS LO
  digitalWrite(SS, LOW);
}

