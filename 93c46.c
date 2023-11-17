/*******************************************
 * eeprom 93c46 driver
 ******************************************/

#include <Arduino.h>


#define SS D1
#define MOSI D2
#define MISO D3
#define SCK D4

uint16_t dataword;

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
  PORTB |= (1 << SCK);

  // 1ms delay
  _delay_ms(1);

  // Make SCK pin LO
  PORTB &= ~(1 << SCK);
}

void eeprom_opcode(uint8_t code) {
  // Set SS pin LO
  PORTB &= ~(1 << SS);

  strobe_sck();

  // Set SS HI and MOSI HI
  PORTB |= (1 << SS) | (1 << MOSI);

  strobe_sck();

  //if code & 0x02
  if (code & 0x02) PORTB |= (1 << MOSI); // MOSI HI
  else PORTB &= ~(1 << MOSI); // MOSI LO

  strobe_sck();

  // If code & 0x01
  if (code & 0x01) PORTB |= (1 << MOSI); // MOSI HI
  else PORTB &= ~(1 << MOSI); // MOSI LO
  strobe_sck();
}

void eeprom_address(uint8_t address) {
  for (uint8_t i = 0; i <= 5; i++) {
    if (address & 0x20) PORTB |= (1 << MOSI); // MOSI HI
    else PORTB &= ~(1 << MOSI); // MOSI LO
    strobe_sck();
    address = address << 1;
  }
}

void eeprom_erase(uint8_t address) {
  eeprom_opcode(0x03);
  eeprom_address(address);
  PORTB &= ~(1 << SS); // SS LO
}

void eeprom_eraseall(void) {
  eeprom_opcode(0x00);
  eeprom_address(0x20);
  PORTB &= ~(1 << SS); // SS LO
}

void eeprom_ew_disable(void) {
  eeprom_opcode(0x00);
  eeprom_address(0x00);
  PORTB &= ~(1 << SS); // SS LO
}

void eeprom_ew_enable(void) {
  eeprom_opcode(0x00);
  eeprom_address(0x30);
  PORTB &= ~(1 << SS); // SS LO
}

void eeprom_read(uint8_t address) {
  uint16_t divider = 0x8000;
  eeprom_opcode(0x02);
  eeprom_address(address);
  dataword = 0x00;
  strobe_sck();
  for (uint8_t i = 0; i <= 15; i++) {
    if (PINB & (1 << MISO)) dataword = dataword + divider; // if MISO is HI, dataword = dataword + divider
    divider = divider >> 1;
    strobe_sck();
  }
  PORTB &= ~(1 << SS); // SS LOW
}

void eeprom_write(uint8_t address, uint16_t data) {
  eeprom_opcode(0x01);
  eeprom_address(address);
  for (uint8_t i = 0; i <= 15; i++) {
    if (data & 0x8000) PORTB |= (1 << MOSI); // if (condition), MOSI HI
    else PORTB &= ~(1 << MOSI); // else MOSI LO
    strobe_sck();
    data = data << 1;
  }
  PORTB &= ~(1 << SS); // SS LO
}

void eeprom_writeall(uint16_t data) {
  eeprom_opcode(0x00);
  eeprom_address(0x10);
  for (uint8_t i = 0; i <= 15; i++) {
    if (data & 0x8000) PORTB |= (1 << MOSI); // MOSI HI
    else PORTB &= ~(1 << MOSI); // MOSI LOW
    strobe_sck();
    data = data << 1;
  }
  PORTB &= ~(1 << SS); // SS LO
}
