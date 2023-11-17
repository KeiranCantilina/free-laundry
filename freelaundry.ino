/*******************************************
 * lol, pay for washing
 ******************************************/

#ifndef F_CPU
#define F_CPU 80000000UL
#endif

#include "93c46.c"
#include "laundrycard.c"

int valid;
int balance;


void setup(){
  //go eeprom!
	eeprom_init();
		
	//go rs232!
  Serial.begin(9600);
	//uart_init();
	Serial.println("laundry card here\n\ncommands:\n1: get status and current balance\n2: fill card with 99.90\n3: set amount to 0.00\n\n");
}

int loop(void){
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

