# Dev Env Setup
```
cd /home/matthewf/skyworks/prep/esp-idf
. ./export.sh
```

# Flashing and Running
```
idf.py set-target esp32c3
idf.py fullclean
idf.py build
idf.py -p /dev/ttyACM0 flash monitor # no flash to run only
```

# UART Assignment
- RX0 and TX0 (receive and transmit for UART protocol) are mapped to debug UART that is shared with USB0 interface (only on baud rate 115200)
- Connected TX from USB TTL with RX0 on board
- Connected RX from USB TTL with TX0 on board
- ACM0 interface is debug port (the one configued in menuconfig, to 115200)

# USB to serial converter

```sh
sudo picocom -b 9600 /dev/ttyUSB0
```
## Loopback (use cable)
- RXD <-> TXD

# Breadboard
- Connects on the column

# Microcontroller
- /dev/ttyACM0

# Setup
- USB-c in board to USB-A in laptop
## Pins of GNSS
- http://www.stotoncn.com/gnssmodule/showproduct.php?lang=en&id=62
- Connect to UART1 of board
- Red is VCC (power), connect to 3V3 of board
- Black is GND
- Green is TX, connect to RX of board (GPIO09)
- Blue is RX, connect to TX of board (GPIO10)
- **Yellow is pps (pulse per second), connect to GPIO4 (defined as PPS_PIN in main)**
- If use logic analyzer, need special grey USB with extra adaptor to connect to laptop USB on the other side
## Running
- build, then flash, then monitor to run
## Testing
- Leave GNSS by the window for clear view of sky
- Connect GNSS to UART1 of board (above)
- Wait to get good nmea sentences (need >= 4 connected satallites)
- **May need to leave running > 15 min** to get signal
### Testing pps
- Can either connect GNSS pins to board or UART2USB adaptor (all except yellow pps pin)
- Connect pps pin to logic analyzer interface analog 1, and connect manually ground in pair from analog 1 to ground of board or UART2USB adaptor
- Run pulseview in Ubuntu
- See output in A0 interface

# Source code
## Current description
### `app_main()` flow
* Configure UART1 of board to correct baud rate (9600)
* install UART1 driver and set pins per global defines
* Call `configure_pps()` to install isr handler for pps pulse and GPIO event queue
** (can't print in isr handler, need task (`gpio_task_example()`)) to handle event from queue 
* Create `gnss_task_read_nmeas()` to read NMEA setences to ensure GNSS is connected
### `gnss_task_read_nmeas()`
- Just read and print NMEA strings when they come over UART interface
### `gpio_task_example()`
- Receive pps events from `pps_isr_handler()`
- Currently just prints that received something
- **TODO** Measure and store number of CPU cycles since last pps here!
- **TODO** If > 2 second passed (maybe second hardware timer), output pulses based on timer to count up to the last stored number of CPU cycles in 1 second. Do this by creating a new output task and event queue. 
- **TODO** Determine the Timers available on the microcontroller, and what can be done with them, or what they can do (e.g. it can toggle a GPIO line) Select one.
### `pps_isr_handler()`
- Called when pps signal comes from GNSS rising edge
- Add event to queue
#### **TODO** 
* Stop hardware timer since last pps received
* Start new hardware timer
* Send to gpio_task_example's event queue the number of cycles in last second