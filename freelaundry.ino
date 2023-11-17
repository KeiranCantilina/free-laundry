/*******************************************
 * lol, pay for washing
 ******************************************/

#ifndef F_CPU
#define F_CPU 80000000UL
#endif

uint16_t dataword;

#include "93c46.c"

int valid;
int balance;
char msg[128];


void setup(){
  //go eeprom!
	eeprom_init();
		
	//go rs232!
  Serial.begin(9600);
	//uart_init();
	Serial.println("laundry card here\n\ncommands:\n1: get status and current balance\n2: fill card with 99.90\n3: set amount to 0.00\n\n");
}

void loop(void){
	//the main loop does not do anything (uart event based)
	while(1){ delay(20); }
}

//event triggered when uart data recived
void serialEvent(){
//ISR(USART_RXC_vect) {
  int input = Serial.read();
	switch (input){

		// status
		case 1:
			valid=is_valid_card();
			if (valid){
				balance=get_balance();
				sprintf(msg,"valid card, balance: %i\n", balance);
				//put_s(msg);
        Serial.print(msg);
			} else {
				Serial.print("no card detected, double check connections");
			}
			break;
		
		//laundry 4 free
		case 2:
			valid=is_valid_card();
			if (valid){
				set_balance(0x24);
				Serial.print("new card balance: 99.90.-");
			} else {
				Serial.print("no card detected, double check connections");
			}
			break;
		
		//set balance to 0.00.-
		case 3:
			valid=is_valid_card();
			if (valid){
				set_balance(0x00);
				Serial.print("new card balance: 00.00.-");
			} else {
				Serial.print("no card detected, double check connections");
			}
			break;
		
		
		default:
			break;		
	}
	//clear_uart_buffer();
}


// Below functions originally in separate file for unknown reason
unsigned int get_balance(){
	eeprom_read(0x07);
	return dataword;
}

void set_balance(int amount){
	eeprom_write(7,amount);
	eeprom_write(9,amount);
	eeprom_write(11,amount);
	eeprom_write(18,amount);
	eeprom_write(20,amount);
	eeprom_write(21,amount);
}

unsigned int is_valid_card(){
	eeprom_read(0x00);
	eeprom_read(0x00);
	if (dataword==0xe8){
		return 1;
	} else {
		return 0;
	}
}


