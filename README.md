# STM32-controllerless-LCD-drive for old 640x480 monochrome LCD

Experimental project to use the STM32 as LCD controller.

The monochrome 640x480 LCD uses the following signals : 

4 bit data bus on PB3-4-5-6

#define frame PB7  //40Hz

#define Msig  PA12 //20Hz

#define load  PB8  //10kHz

#define cp    PB9  //980kHz

#define ena   PA11  


The STM32 can handle the speed of all signals for controlling the LCD. The Arduino program
uses fast port write so the clock speed is around 1 Mhz. 

A controller like this has the following tasks: send frame start signal, read 4 bits from RAM, 
put the 4 bits on port, move clock, repeat this 640/4 times, at end of the line send line latch signal,
repeat line cycle to do all 480 lines.

For more explanation please read the code, and see the next link for info on hardware :

http://img.hqew.com/.../Elect.../20080807001540982DVDL83.PDF

The LCD main signals are : line load, frame, data 4 bit wide, clock, M signal.

They need +5V, -22V voltage for the operation, provided by a MC34063 converter.

