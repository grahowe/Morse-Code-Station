# Morse Code Station - Improvement on Mario Gianota's project

***THIS IS NOT MY ORIGINAL CODE! All credit belongs to Mario Gianota.***

The Hackaday article/page here: https://hackaday.io/project/175129-arduino-morse-code-station#:~:text=Description,to%20display%20the%20entered%20characters.

I have made several additions to this code and circuit:

1. Instead of a loud, obnoxious piezo buzzer, I've replaced it with a PWM output. It outputs a pure-ish square wave to any standard headphones or earbuds. Use a 10K potentiometer to adjust volume.
2. I plan on using the Adafruit library for now, ~~but in the future, I plan to implement the U8g2 library. It is easier, in my opinion, to work with. It also give you more fonts to work with and~~ it still utilizes I2C.
3. I removed the LED and placed the LED circuitry on the built-in pin, D13. Put an LED here if you want an external optical indicator.
4. I have not figured out how to scroll the screen. To refresh/reset, I've added a momentary push-button on the RESET pin going to ground.

For documentation and more information, refer to the attached schematic and the Hackaday page.

73s and happy coding!!!
