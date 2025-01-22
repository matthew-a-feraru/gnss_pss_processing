Preparation Notes:

Hardware:

LHT00SU1 logic analyser, 16 MSps, eight channel (of which one channel analogue-capable, 3 MHz)
-> Show you what is going on the bus (quicker than multimeter or compare multiple lines in parallel)
-> Recording session in analog way

Espressif ESP32-C3 dev board, built-in PIFA
Microcontroller, target for code
Wifi and bluetooth
ESP IDS (hardware abstraction layer)
Useful: Github copilot (develop differently for it, more time on algos, comments, and review)

----
Peripherals
---
Lots of devices attached to microcontroller
Direct, not through USB (abstract)

Peripheral: dual meaning
1. Traditional (like mouse, "edge" of computer)
2. In microcontroller, thing that gives you a bus (I/O things)

U-Blox UBX-8030-KT GNSS receiver module, tri-system, built-in patch antenna
(global navigation system, accepts signals like GPS)
Transmit and receive interface (character oriented communication bus)
Sends bytes (each has own frame. 1 bit start + 1 byte + 1 bit stop)
Logic analyzer can decode frames if set correct rate!
Each byte is atomic unit. Can send and receive from either end (clock needs to be matched on both sides, within 1%. Set same board rate on both sides! (9600 board rate))
Receive messages 1 per second from GNSS
Send pulse 1 per second to GNSS (important: when rising edge occurs, more precise than receive time over bus)

Rakon STP3158 LF 12.8 MHz OCXO
MAY NEED EXTERNAL POWER SUPPLY
Transitor heats up quartz crystal at constant temperature 
Don't use this initially!


---------------
TO USE INIITALLY
MICROCONTROLLER, GNSS, LOGIC ANALYZER
---------------

Maxim DS3231 TCXO module
SLOWER, but can connect a lot of things to it
Not needed. Can set it to keep track of time, read time back from microcontroller
Ultimate purpose: keep time if microcontroller looses power
Long term, better accuracy than OCXO (monitors temperature)

WCH CH340E USB to UART adaptor
Probably not useful, can plug microcontroller directly into computer with USB
Lookup driver in manufactuer name

SSD1306 OLED module, 128 × 64 pixel, SPI interface
Lo-fi display
Interface: SPI. One device is the master (ESP32)
Master generates a clock
Devices can transmit back only on clock tick (all buses still bide)

Tactile switch module
Buttons that can be connected to microcontroller

Micro breadboard
Connect point to point 
Run out of ground and power if directly plug everything into microcontroller
If just GNSS, can connect directly. More, start to use this 

Assorted LEDs, high forward voltage
High forward voltage means blue or green (white is blue ultraviolet)
Can connect directly to pins of microcontroller (no transitor needed)

Various Dupont wires
Connecting things



---------------------------
What you could do:
(lots of things to play around with)

Goal idea: make a GPS-DO
GPS-disiplined oscillator

Oscillator could be: OCXO (oscillates at certain frequency)
Don't have to use it (just use microcontroller) 
Output frequency standard: 10 MHz
1 pps output 
Disiplined by GPS receiver
GNSS receiver more accurate source of time (cheats by taking time from satallite's atomic clocks)

This GPS receiver doesn't have frequency output
Don't have anything with tunable frequencies anyway (VCXOs), wouldn't have been helpful

What have instead is 1 pps out from GNSS receiver and can create a 1 pps out from microcontroller

Make a system with GPS system connected to microcontroller
Code takes in 1 pps signal from GPS
Microcontroller has internal clock (not exact)
Figure out how many cycles it should be,
create 1 pps signal out

Goal: loose signal from GNSS, keep outputting time 

Output: 1 second pulse per LED 

1 pps pulse: defacto. Just some pulse out (no more than 50% cycle. Keep up for a few milliseconds. Monitor by connecting to LED)


SOMETHING USEFUL:
"timer" peripheral in microcontroller
concept of timer: microcontroller has some sort of clock (40 MHz from actual crystal oscillator), ESP32 multiplies this up to 160 MHz to core frequency
When have a timer, can run in same frequency as core frequencty at most basic
Can also divide clock down
Can set duration (16 or 32 bit (probably 32 bit))
160 MHz clock, increment each cycle (resets to zero after hit a limit you set)
*** Can perform an action when hit limit. ISR does something when this is triggered
Operates without interfernce from CPU (this is why "peripheral")
Never need to be in situation when interrupt comes and wait for CPU to context switch (very fast)
Basically, if have difficulty reliably figuring out how many cycles (code flucuates a lot, this means that you have CPU in the loop. Try to remove the CPU).
Should be pretty good frequency of output pps, with constant offset likely from 
(btw, can't run floating point calculations in ISR, keep simple).

Bringup:
getting something to run on new hardware
ESP 32 just well known. Just lookup (can develop directly in VS code or platform IO).
1. write code
2. compile code (need ESP IDS, contains compiler)
3. write the compiled code to microcontroller (done over UART, just over USBC once install driver)
4. printfs displayed over same bus on my PC, real time (print too much, start dropping frames)

Generally with ESP32, running freeRTOS OS (real-time OS). Stripped down OS
No virtual memory for instance
Threads = tasks. Nothing to stop processes overwritting memory (all memory absolute)

LATER: 
OCXO: "consistently off" property
Microcontroller chip: more temperature dependent 
Use data from the other


----
if questions, just email


-----------------------------------------------------
Second meeting


Implementation dependent: 
    ublox chip may say over NMEA it is going to be detected
    not very reliable

    Failure modes are undetermined
    Once get pulse accurately, should be stable
        May get unstable first pulse until 

    Cover receiver with foil to see pulse go away
    Detection of missed pulse (constant time lag)

    Challenge: device that outputs 1 pps, even if input goes away temporarily
    Goal now: output from board intiially, then detect from GNSS as discipline
    of board 
    
    If switch to theoretical domain: two things to sync
    - Frequency and phase 
    <Could read up on why time was used to solve GNSS>
    
    Frequency part is easier (look at synchronous Ethernet for frequency)
    Look into configuration of UBLOX receiver. Has antenae delay 
    (can be configured with nanoseconds). Need to account for phase
    shift when discipline the board

    Idea: merge reference sources and "take best parts".

    Read up on board being too fast before

    Temperature flucutations common in ESP-32 board

    Keep this open as enhancement 
    Alternative: always output your own pulse with OCXO
        this is fairly accurate 12.8 MHz, wanders a bit 