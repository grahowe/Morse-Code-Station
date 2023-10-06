# Morse Code Station - Improvement on Mario Gianota's project

***THIS IS NOT MY ORIGINAL CODE! All credit belongs to Mario Gianota.***

***The Hackaday article/page here: https://hackaday.io/project/175129-arduino-morse-code-station#:~:text=Description,to%20display%20the%20entered%20characters.**

I have made several additions to this code and circuit:

1. Instead of a loud, obnoxious piezo buzzer, I've replaced it with a PWM output. It outputs a pure-ish square wave to any standard headphones or earbuds. Use a 10K potentiometer to adjust volume, and place a 47K resistor in series with it to set a maximum volume. Detailed information can be found on the schematic.
2. I removed the LED and placed the LED circuitry on the built-in pin, D13. Put an LED on pin D13 if you want an external optical indicator.
3. After 30 seconds of inactivity, the screen will clear itself. I have yet to figure out how to get it to scroll properly.

This is a simple Morse Code practice oscillator that will help you fine-tune your CW sending skills. You can use a straight key or an external keyer. The timing is adjustable based on dot duration in milliseconds. Remember, a dash is 3x the length of a dot, so while this is useful for a wide spectrum of words per minute, you may need to adjust the duration based on your sending speed.

I have also permanently mounted this to a solderable breadboard and it works just fine.

For documentation and more information, refer to the attached schematic and the Hackaday page.

73s and happy coding!!! A BIG thanks to Mario Gianota for the base code.
