#include "game.h"

extern volatile t_state	status;

volatile uint8_t tick_pending = 0;
volatile uint8_t resolve_pending = 0;

void init_led(void);

static void show_leds(uint8_t n)
{
	PORTB &= ~(_BV(PORTB4) | _BV(PORTB2) | _BV(PORTB1) | _BV(PORTB0));
	if (n >= 1) PORTB |= _BV(PORTB0);
	if (n >= 2) PORTB |= _BV(PORTB1);
	if (n >= 3) PORTB |= _BV(PORTB2);
	if (n >= 4) PORTB |= _BV(PORTB4);
}

void start_slave_watchdog(void)
{
    OCR1A = 25000; // OCR1A = ((F_CPU * 0.1) / 64) - 1 (every 100ms)
    TCCR1B |= (_BV(WGM12) | _BV(CS10) | _BV(CS11)); // prescaler of 64
    TIMSK1 = _BV(OCIE1A);
    status.countdown = 3; //set countdown at 3 for 300ms (if no info from CPU for 300ms stop program)
}

void	start_game(void)
{
	if (status.role == MASTER)
	{
		OCR1A = 62499; // OCR1A = ((F_CPU * Time_in_seconds) / prescaler) - 1 (1 second)
		TCCR1B |= (_BV(WGM12) | _BV(CS12)); // prescaler of 256
		TIMSK1 = _BV(OCIE1A);
		status.countdown = 4; //set countdown
		show_leds(4);
		write_mode(); // enter write mode to trigger handle_master_tx()
	}
	else
		start_slave_watchdog();
}

void	resolve_game(void)
{
	tick_pending = 0;
	TCCR1B = 0;
	status.waiting = 0;
	uint8_t master_wins = (status.countdown > 0) ? !status.mt_button_pressed : status.mt_button_pressed;
	
	set_rgb(get_rgb_from_hex(master_wins ? "#00FF00" : "#FF0000"));
	// we use status.countdown as value to send to slave (5 is lost, 6 win)
	status.countdown = master_wins ? 5 : 6;
	reset_polling();
	write_mode();
}

void	handle_slave_led_info(uint8_t led)
{
	if (!status.started)
	{
		status.started = 1;
		start_game();
	}
	status.countdown = 3;

	if (led < 5)
		show_leds(led);
	switch (led)
	{
		case 6 :
			set_rgb(get_rgb_from_hex("#00FF00"));
			_delay_ms(3000);
			break;
		case 5 :
			set_rgb(get_rgb_from_hex("#FF0000"));
			_delay_ms(3000);
			break;
	}
	if (led > 4)
		stop_game();
}

void	on_master_data_sent()
{
	if (status.countdown > 4) // countdown > 4 means that the game was resolved (only way to reach it)
	{
		_delay_ms(3000);
		stop_game();
	}
	else
	{
		status.waiting = 1;
		start_polling(POLL_FOREVER);
	}
}

void    stop_game(void)
{
	status = (t_state){0};
	resolve_pending = 0;
	tick_pending = 0;
	TCCR1B = 0; // stop timer1
	TIFR1 |= _BV(OCF1A); 
	TCNT1 = 0;
	reset_polling();
	init_led();
	TWCR = TW_SEND | _BV(TWSTO); // Tries to Clear bus possession (No Master)
	TWAR = (SLAVE_ADDRESS << 1);
	set_rgb(get_rgb_from_hex("#000000"));
	PCIFR |= _BV(PCIF2); // clear interruption cache for button in case we clicked them twice
}


uint8_t on_poll_idle(void) // Wait for the split second poll is in idle (not launched) to cancel it properly
{
	if (resolve_pending)
	{
		resolve_pending = 0;
		resolve_game();
		return 1;
	}
	if (tick_pending)
	{
		tick_pending = 0;
		status.countdown--;
		show_leds(status.countdown);
		status.waiting = 0;
		reset_polling();
		write_mode(); // Use Rep start we dont need to stop the BUS
		return 1;
	}
	return 0;
}

void TIMER1_COMPA_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void TIMER1_COMPA_vect(void)
{
	if (status.role == MASTER)
	{
		if (tick_pending) // in case of a tick not being compute 
		{
			stop_game();
			return ;
		}
		if (status.countdown <= 1)
			TCCR1B = 0;
		tick_pending = 1; // add pending tick of led lighting
	}
	else if (--status.countdown == 0)   // Slave timeout
		stop_game();
}