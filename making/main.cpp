#include <avr/io.h>
#include <util/delay.h>

int main(void) {
	DDRB = 0xff;

	while (1) {
		PORTB = 0xff;
		_delay_ms(300);
		PORTB = 0;
		_delay_ms(300);
	}
	return (0);
}
