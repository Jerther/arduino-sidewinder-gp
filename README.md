# About
This project aims to correctly interpret signals sent by the gamepad using an Arduino Uno. Anything can then be done and a keyboard mapper written in Python is provided as an example.

# How to use
### Requirements
- A Arduino Uno*
- A Sidewinder Gamepad*
- A DB15 female connector
- Some wires

*: Other Arduino boards might also work and feel free to try but be aware that the Uno works at 16 MHz and timing is very important.

*: While the protocol is similar through the Sidewinder product line, the Gamepad is purely digital and the data packets are smaller than other products so this project will not work with other Sidewinder products.
### Hardware
Note: To function properly, the Gamepad needs **all** VCC and GND pins to be connected.
```
 DB15          => Function => Arduino
-------------------------------------
 1 , 8, 9, 15  => VCC      => 5V
 2             => Clock    => 2
 3             => Trigger  => 3
 4, 5, 12      => GND      => GND
 7             => Data     => 4
```

### Software
Upload the C program to the Arduino and open the serial monitor at 115200 baud rate. You should see a stream of lines, 14 characters each. Each character represents a button on the pad in that order:

```UP, DOWN, RIGHT, LEFT, A, B, C, X, Y Z, LT, RT, START, M```

Now, edit the Python 3 program called serial_to_kb.py. Change the serial port to the one used by the Arduino, and then run the program. This will listen to the Arduino and map every packet it receives to some keyboard strokes. Just map those keys on a game and you've got an actual **game**pad ;)

Be aware that the serial data transmission in the Arduino and the serial data interpretation in the Python program were made as a proof of concept and are really not optimized. Therefore, some of you hardcore gamers might feel some input lag. You're welcome to chime in and contribute to improve that though ;)

### Troubleshooting
**I get no data!**

Tooggle the mode button off and on again. IIRC, the official manual stated that too ;)

# Technical details
Yeah. Well. First, I'll start by saying that with the very same Arduino, I had a SNES controller working in less than 10 minutes. This took me two weeks. Let's see why!
## The protocol
Looking at the seamingly very stable [code made for the Sidewinder controllers in Linux](https://github.com/torvalds/linux/blob/master/drivers/input/joystick/sidewinder.c) we find a clue:

```The protocol is very, very, very braindamaged.```

And indeed it is. But after many hours trying to make sense of all this, I realized I could save myself a whole bunch of problems by not trying to detect anything else than one Gamepad. Phew.
## The data lines
I first stumbled on [MaZderMind's repository for a similar project](https://github.com/MaZderMind/SidewinderInterface). Although I couldn't make any part of his code work with my Gamepad, his documentation was fairly useful. There seems to be some problem on the clock and data lines when the trigger line is toggled. Let's take a look at a single packet (all lines are 'active low'):

![Single packet](doc/ss1.jpg)

In theory, here's how it's supposed to work:

1. We activate the trigger.
2. We wait for the clock to become active.
3. We release the trigger and wait for the next clock pulses: 14 for the buttons and one for parity check: 15 in total.
4. At each clock pulse, we record a button state.

The picture above sure looks like this right? Well, let's look at this part marked in red:

![Single packet](doc/ss2.jpg)

This is where the clock and data lines reset. Now let's zoom in:

![Lines reset](doc/ss3.jpg)

This noise. This shouldn't be there. But it is. And as far as I know, it is for all Sidewinder products. And if not filtered out, it triggers the MCU interrupt, but it's so fast sometimes it doesn't. So what do we do? It depends on how you see it. It looks like as soon as the trigger is activated, the clock and data lines take around 227 microseconds to stabilize so I just wait 250 before I deactivate the trigger, don't listen to the clock line until after the delay and be done with it. It feels SO dirty but so is this garbage.

![wait](doc/ss4.jpg)

## The clock
On a SNES pad, the MCU (Arduino) drives the clock line. So we just use pretty much any frequency we see fit. But here, it's the Gamepad that controls the clock. So the Arduino has to be fast enough to catch and process every single clock pulse of the gamepad. Even though it ticks at around 100 KHz and the UNO is 16 MHz, the interrupt routine really has no time to lose. Even simple instructions like ```digitalRead()``` are too slow! I've been able to optimize enough in C but anything further would have required assembly language.

## Trigger frequency
The Gamepad can work a trigger signal at 1 Hz and it'd be fine, but going faster than 1 KHz makes it unstable: it seems to work fine but as soon as a button is pressed, the clock doesn't tick for all buttons and sometimes it just does not work at all.

# Final thoughts
I wouldn't have thought such a good gamepad had such an horrible interface. But that's just my opinion. There are far worst things in electronics. I learned a lot and at least, I still get to use the same controllers I've had for 20 years ;)
