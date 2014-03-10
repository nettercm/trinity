#include <pololu/orangutan.h>

/*
 * svp-one-servo: for the Orangutan SVP.
 *
 * This example uses the OrangutanServos functions to control one servo.
 * The servo pulse signal is sent on pin PD5, which is hardwired to the
 * input of the demux.  The servo signal is available on demux output 0.
 * This example uses the OrangutanPushbuttons functions to take input
 * from the user, and the OrangutanLCD functions to display feedback on
 * the LCD.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int servo_test()
{
	int target=1300;
	const unsigned char demuxPins[] = {};
	servos_start(demuxPins, sizeof(demuxPins));
	set_servo_target(0, target);	// Make the servo go to a neutral position.
	clear(); // Clear the LCD.
	print_long(target);



	while(1) // Loop forever.
	{
		// When the user presses the top button, execute a pre-programmed
		// sequence of servo movements.
		if (button_is_pressed(MIDDLE_BUTTON))
		{
			switch(target)
			{
			case 580:
				target = 2300; //1450;
				break;
			case 1450:
				target = 2300;//2410;
				break;
			case 2300:
				target = 580;
				break;
			default:
				target = 580;
				break;
			}
			clear(); // Clear the LCD.
			print_long(target);
			servos_start(demuxPins, sizeof(demuxPins));
			set_servo_target(0, target);	
			delay_ms(1000);
			servos_stop();
			//target=500;
		}


		if (button_is_pressed(TOP_BUTTON))
		{
			target += 10;
			//play_note(A(4), 50, 10);
			//delay_ms(250);
			//set_servo_target(0, target);
			//servos_start(demuxPins, sizeof(demuxPins));
			set_servo_target(0, target);	
			//target=500;
			clear(); // Clear the LCD.
			print_long(target);
			// Set the servo speed to 150.  This means that the pulse width
			// will change by at most 15 microseconds every 20 ms.  So it will
			// take 1.33 seconds to go from a pulse width of 1000 us to 2000 us.
			//set_servo_speed(0, 150);

			// Slowly move the servo to position 1800.
			set_servo_target(0, target);
			//wait_for_button_release(TOP_BUTTON);
			delay_ms(200);

			// Disable the speed limit
			//set_servo_speed(0, 0);

			// Make the servo move back to position 1300 as fast as possible.
			//set_servo_target(0, 1300);
		}

		if (button_is_pressed(BOTTOM_BUTTON))
		{
			target -= 10;
			//play_note(A(5), 50, 10);
			//delay_ms(250);
			//set_servo_target(0, target);
			//servos_start(demuxPins, sizeof(demuxPins));
			set_servo_target(0, target);

			//target=2100;
			clear(); // Clear the LCD.
			print_long(target);
			// While the user holds down the bottom button, move the servo
			// slowly towards position 1800.
			//set_servo_speed(0, 60);
			//wait_for_button_release(BOTTOM_BUTTON);
			delay_ms(200);

			// When the user releases the bottom button, print its current
			// position (in microseconds) and then move it back quickly.
			//clear();
			//print_long(get_servo_position(0));
			//print_from_program_space(PSTR(" \xE4s"));
			//set_servo_speed(0, 0);
			//set_servo_target(0, 1300);
		}
	}
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
