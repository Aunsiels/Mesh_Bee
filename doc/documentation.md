#Documentation of MeshBee Project by Julien Romero

This document will present and explain my work during my **semester project**. The goal is to build a **wireless network of sensors for performance analysis**. The project began in February 2016. I hope that thanks to this document you will be able to understand what I did, why I did it and to continue my work.

## Why MeshBee ?

### Choosing a Wireless Protocol

The first thing to think about was how to communicate. There are plenty of available protocols and we had to choose among them the one which will best fit to our needs. Eight protocols were considered: ZigBee, Bluetooth, BLE, Rubee, Zwave, ANT/ANT+, EnOcean and WiFi. To evaluate them, four characteristics were chosen: the data rate, the size,  the consumption and the range. Of course, this properties depends of a lot of parameters : hardware, environment,... So, we tried to choose values which appeared in most of the description. The use cases are also useful to know what kind of things are possible with a protocol.

#### Zigbee

![Zigbee](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/Zigbee-logo.jpg) 

**Zigbee** is based on 802.15.4 specification, at 2.6 GHz. It is used in smart home, industrial control, medical tools, fire sensors... It is also used for low energy sensors, which is useful for us. A Ad Hoc network is created around a coordinator. Different architecture of the network are possible, we shall see it later. However, there can be problems with inferences as 2.4GHz is often used.

#### Bluetooth

![Bluetooth](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/bluetooth.png)

**Bluetooth** is based on IEEE 802.15.1 and works at 2.6GHz. It is used in smart phones, keyboards, mice, sensors,... The fact that it is in all smart phone popularized in. The consumption is medium.

#### BLE

![BLE](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/Bluetooth_Smart_Logo.jpg) 

The **Bluetooth Low Energy**, just like Bluetooth, works at 2.4GHz. We can see it as a low consumption version of Bluetooth, which exists to answer modern problems like consumption. So, it is used in health care, sport, sensors...

#### Rubee

![Rubee](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/rubee-CHR8VIS.png) 

**Rubee** is a 450kHz protocol designed to transmit information in harsh environment (with a lot of steel for example), with high security. So, it is often used for military applications. It works with tags, a bit like NFC but the range can be higher. However, it is not that easy that find chip for Rubee development.

#### Zwave

![ZWave](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/zwave.png) 

**Zwave** is a protocol around 900MHz, designed for home automation applications. It can be integrated to a lot of existing of house products. It has a small consumption and a lower data rate than Zigbee.

#### ANT/ANT+

![ANT](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/ANT-plus-logo.jpg)

**ANT/ANT+** works on 2.4GHz. It is a very low consumption protocol. It is mainly used in health, sport, smart home and industry.

#### EnOcean

![EnOcean](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/EnOcean.png) 

**EnOcean** works on 902 MHz, 928.35 MHz, 868.3 MHz and 315 MHz and is a very low energy protocol (can also work without battery, inside a switch for example). The data rate is low. It can be used in smart house and in sensors.

#### WiFi

![Wifi](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/wifi.png) 

**WiFi** is a very popular protocol in personal application and is based on IEEE 802.11 . It operates on 2.4, 3.6, 5, and 60 GHz frequency bands. The speeds that can be achieve are high but the consumptions are also high. It is also used for the Internet of things and a lot of ship are now design with wifi integrated (Raspberry Pi 3, Arduino Genuino MKR1000, Red Pitaya, Electric Imp...).

#### Putting Everything Together

Everything was summarized in an [odt file](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/semester_project_protocols.ods). The results are on sheet 1 and 2 and some results were plotted.

![conso-vs-data-rate](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/conso_vs_data_rate.png) 

We can see that each protocol is in a different area and so, we need to choose the one which will be good for us. For that, a cost function was design according to our needs. The function has four parameters (for the four protocol characteristics) that can be tuned. The chips available were also considered. Are  they hard it get ? How easy is it to program them ? Is their help for it somewhere ? What are the prices. Finally we decided to go for **Zigbee**

### Choosing a Zigbee Chip

There are plenty of Zigbee chips available. What we wanted to find is a chip which controls both Zigbee and a sensor. So, the prototype can be programmed on the chip directly, without having to use an additional board like an Arduino. We also needed to be able to read sensors thanks to i2c and ADC. We found a nice design, based on a NXP chip : the **MeshBee**. It is **open source** (both software and hardware) and easy to prototype with. Thus, thanks to it, we were able to first try to write a program to test if my prototype was working and then we designed a new chip in which sensors were directly integrated. That made the final result smaller.

## The System Architecture

The system architecture was composed of three parts :

* The **MeshBee network** with sensors
* An **interface** between MeshBee and a server
* A **HTTP server** which collected information and displayed it for the user

This choose of architecture was driven by several reason. The final goal would be to visualize in some way the measured data. So, a screen was needed somewhere. What would be nice would be that any device can print those data, and so a web page was the most practical way. So, if a user was close enough to the Raspberry Pi, he could connect directly with his smart phone with Wifi (which is on all smart phone) or if the Raspberry Pi was connected to the Internet, he was able to access it from everywhere.

In our implementation, both the interface and the server were on  a **Raspberry Pi** (3) which created a WiFi network. So, people were able to connect to it and to visualize data.

For the Zigbee architecture, the **mesh architecture** was used. There were three kinds of nodes :

* **A *unique* coordinator** : it was in charge of organizing the network and was directly linked to the Raspberry Pi.
* **Router** : it was able to propagate the network and was an access point to the network.
* **End-node** : it was only able to connect to the network.

![Zigbee network](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/meshbee_network_architecture.jpg) 

For the interface between the Zigbee world and the server, a serial communication was used. The data were then read by a **python script** which transformed them into **HTTP request** for the server. Then, the server organized everything and was ready to display, in real-time, information from sensors.

![global architecture](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/global_network.jpg) 

## The Embedded Software

The embedded software on the MeshBee have to read data from a sensor and to send information through the network to the main node, the coordinator.

### The MeshBee Framework

First things first, we needed to understand how MeshBee works.

![meshbee](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/MeshBeeArchitecture.jpg)

The MeshBee firmware is built on an OS provided by NXP, **JenOs**, and a **SDK for Low Energy Zigbee**, also provided by NXP. It is important to know that because sometimes, the functions we were looking for (as interrupt-liked functions) were not in the MeshBee Framework but directly in the library provided by NXP.

Inside the MeshBee, we found a **Arduino programming** environment. The AUPS (Arduino-ful User Programming Space) provided two functions : a **setup and a loop function**. The setup function was called during the initialization of the system and the loop function was called periodically (it was possible to specify the period).

In the middle of the picture, there are the four modes of the MeshBee :

* **AT** : this is an **interactive mode**. Easy commands have to be send to the MeshBee and it answers in an user-friendly way. To go to AT mode from everywhere, ***+++*** have to be send.
* **API** : this is a more effective and formatted way to communicate with the MCU. Thanks to it, AT function can be called from inside and outside the MCU, via UART for example. It is also possible send query to other nodes through the network thanks to the API commands.
* **MCU** : this is the **Arduino mode**. The Arduino loop is only executed if the MeshBee is in this mode.
* **Data** : this is a **transparent mode** : all data received on the Zigbee network are directly transmitted to UART and all data sent via UART are broadcast on the network.

The last thing to understand about this firmware is the **Suli** interface. This is a general library created by SeeedStudio. The goal is to make the interaction with GPIOs easier. So, for I2C, ADC,... there is no need to call the functions from JenOs but the easy functions provided by Suli can be used. However, we needed more advanced functions.

### Important Documents

Here are some documents and links which can be useful while developing with the MeshBee.

* The [MeshBee wiki](http://www.seeedstudio.com/wiki/Mesh_Bee) : on this page you will find useful information for the installation and use of the MeshBee. The documents up-to-date are (normally) also available (some are not updated anymore).
* The [programmer wiki](http://www.seeedstudio.com/wiki/UartSBee_v5) : can always be useful.
* A [Google group](https://groups.google.com/forum/#!forum/seeedstudio-mesh-bee-discussion-group) dedicated to MeshBee : try to ask your questions there but I am not sure they answer.
* The [User's Manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_User_Manual_v0.3.pdf)
* The [CookBook](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_Cook_Book.pdf) : useful to start using MeshBee.
* The [AT manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/AT_command_manual_for_Mesh_Bee_V1.0.pdf) : AT commands, not updated. See User's manual.
* The [Zigbee PRO Stack User Guide](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/JN-UG-3048.pdf) : provided by NXP
* [JenOS User Guide](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/JN-UG-3075.pdf) : provided by NXP
* [Peripheral API](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/JN-UG-3087.pdf) : provided by NXP
* [Datasheet JN516x](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/JN516X.pdf) : provided by NXP
* [Zigbee Cluster Library](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/JN-UG-3077.pdf) : provided by NXP
* [Zigbee Home Automation](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/JN-UG-3076.pdf): provided by NXP

![MeshBee pins](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/600px-Mesh_Bee_Pin.jpg)

![Pin disposition](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/pin_dispo.png)

### Installation

To be able to compile the code, we needed to install the tools given by NXP. Here are summarized instructions given on [this page](http://www.seeedstudio.com/wiki/Mesh_Bee#9.1_Firmware_Downloads).

To be able to compile the program, we needed to use **Windows**.

The first thing we needed to check is if whether the programmer is well configured or not. We only needed to do that if it was the first time we use it. We downloaded [this program](http://www.seeedstudio.com/wiki/File:FT_Prog_v2.8.2.0.zip) and unzipped it with our favorite unzipper. We connected the UartSBee v5 to PC, opened **FT_Prog** and configured it like this:

![programmer configuration](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/FT_Prog.png)

**THIS IS VERY IMPORTANT TO BE SURE THE PROGRAMMER IS ON 3.3V ! OTHERWISE, THAT DESTROYS THE MESHBEE.** To do it, we used the right switch (if the antenna is up) and put it up to 3.3.

We also had to put the programmer in **PROG** mode thanks to the second switch, in the bottom position. This mode allowed us to program the MeshBee.

We needed the tool to program the MeshBee. [We downloaded it](http://www.seeedstudio.com/wiki/File:Jennic_flash_programmer.zip) and unzipped it. We connected the MeshBee to the programmer. The antenna **MUST** be on the same size than the usb port. We were then able to open **FlashGUI.exe**. This is the software to flash the MeshBee. We created a shortcut somewhere because we needed it a lot. Here are the steps we followed to program the MeshBee :

![Program MeshBee](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/400px-Flash_programmer1.png.jpeg)

We began by choosing the binary file. There were in the **Mesh_Bee/build/output** directory (our MeshBee project can be find on our [github](https://github.com/Aunsiels/Mesh_Bee). There were three of them : one for the coordinator, one for routers and one for end-nodes. We chose one to try, the coordinator one for instance (COO). Next, we needed to select the COM port, which was where our MeshBee was connected. We had several of them as we had other devices connected. To know which one was the good one, we unplugged the programmer, looked at the list, plugged it again and the one which was added was my MeshBee. **We needed to remember the number here**, after the COM. It was useful to communicate with the MeshBee later. We needed to be sure that the **Connect** box was checked. We clicked on the Refresh button. If everything was OK, the MAC address of the Zigbee would be printed in the boxes. Otherwise, we checked that everything was well connected, in **PROG** mode and we did the previous steps again. We were then able to program the MeshBee by pressing **Program**. We normally received a message which said that everything went well. We had the program on the chip.

Then, we installed the SDK. We downloaded the [SDK Toolchain JN-SW-4041](http://cache.nxp.com/documents/other/JN-SW-4041.zip), the [SDK Zigbee Smart Energy JN-SW-4064](http://cache.nxp.com/documents/other/JN-SW-4064.zip) and the [SDK ZigBee Home Automation JN-SW-4067](http://cache.nxp.com/documents/other/JN-SW-4067.zip). We installed them in the **same directory**, C:/Jennic for instance. This is important as the compiler will look for them there.

Our project had also to be in the same directory, in C:/Jennic/Application. We cloned in from [github](https://github.com/Aunsiels/Mesh_Bee) (it can also be downloaded as a zip file). In a console (git exists on Windows too) :

	git clone https://github.com/Aunsiels/Mesh_Bee

We were able to open a program called Jennic Bash Shell. We opened it. It was a Linux-like console. It opened in the C:/Jennic/Application directory. We went to the build directory with **cd Mesh_Bee/build/**. There were scripts here for compilation. We had problems with them so we ran directly the commands in the build files directly in the console. For the coordinator :


	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=COO clean
	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=COO all

For the router :

	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=ROU clean
	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=ROU all

And for the end-node :

	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=END clean
	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=END all

Compiling can be long. If we did something wrong, you would have an error message. Otherwise, the files in Mesh_Bee/build/output were updated (we usually checked the date to be sure).

We then knew how the compile a program and flash it. It was time then to interact with the MeshBee.

### AT MODE

We had everything to compile so we were ready to communicate with the MeshBee. This communication used **UART1**, so any USB-to-TTL working at **3.3V (this is super important)** device would work. At that time, we only used the programmer. We just switched the left switch to **UART1** (up).

We needed a tool to communicate. After looking for a serial terminal on Windows, we used [realterm](http://realterm.sourceforge.net/) but anything else would work. [We downloaded](https://sourceforge.net/projects/realterm/files/Realterm/) the latest version and installed it.

![realterm](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/realterm1.png)

When we opened it, we were asked to choose a display mode. In **AT mode**, we simply communicated with ASCII so there was nothing to do here. We went to the second tab, **PORT**. This was were we configured the communication. The Baud was **115200**, there was **no parity bit**, **8 data bits**, **1 stop bit** and **no flow control**. Here, we needed to remember the number after **COM** when we programmed the MeshBee. We had to put this number in the Port field.

![Port](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/Ports1.png)

We were then able to send and receive data. We went to the **Send** tab. We saw two boxes with *Send Numbers* and *Send ASCII* on the right. This is where we had to type the messages we wanted to send. When we sent an **AT** command, it was important to have **CR** (which is \r) at the end of the line. To do so, **we checked the CR box**. We will not specify anymore this <CR> symbol at the end of each AT command.

![send](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/sendchars1.png)

It was time for our first command : **+++**. This allows us to go to AT mode. If we were in MCU mode before and the loop was too long, we could have problems to enter in AT mode. So, we usually just spammed **+++** while reseting the MeshBee (button on the side of the programmer). We received an *Enter AT Mode* if we were not in AT mode yet or an *Error, invalid command* if we were in AT mode. At least normally we received something.

We then had to understand AT commands structures. AT commands were composed as follow : **ATXX[YYYY]**. They always began by AT. Then follow two characters to identify the command, for example IF or AJ. Then follow up to 4 optional numbers which are parameters of the command. For more about it, everything was explained in details in the AT commands section in the  [User's Manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_User_Manual_v0.3.pdf). We noted by looking at the code (that's something we had to often do) that all commands were not available for all device type. For instance, an end-node was not able to create a network and thus did not have this command.

Other commands were tried. We began with **ATIF**. This printed information about the MeshBee. We checked that the Device Type was the one I thought, the coordinator for example. With the coordinator, we sent **ATPA1**. This command created a new network. The MeshBee was then reset and the **ASSOC** led on the programmer light up. We sent **ATIF** again and read the **PANID** field, which was the id of our network. We went to an other mode, for example data mode with **ATDT**.

![ATIF](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/400px-Configure_coo2.png)

Next, an other node was connected to the network. We had to remember that we **always** need the coordinator to be on. We unplugged it from the programmer and gave it power with the raspberry pi (see below). We wanted to be sure that it was connected, so a LED was also connected (and a resistor !) to the ASSOC pin (We were able to read the names of the pins below the MeshBee but there is also a picture above to check). A node was plugged in the programmer. It had been flashed with the router or end-node code.

As before, we went to AT mode with **+++** and got information with **ATIF**. If the MeshBee was not configured before, the PANID was 0x0000 or a number different from the one of the coordinator. We had to say to our device to automatically join a network with **ATAJ1**. Then the network was rescanned with **ATRS** and after some seconds, the **ASSOC** led lit up. We checked information with **ATIF** and then we had the same PANID than the coordinator. To know what are the other nodes on the network, we sent the **ATLA** command. That sent a request to all nodes and we received information about all of them. We went to data mode with **ATDT** and everything which was typed in the console was broadcast on the network. As the coordinator was also linked to a serial port, the raspberry pi one (see below), we saw our messages which appeared in the coordinator console.

We ended exploring the basis of AT commands, how to simply configure a MeshBee and how to send messages.

### MCU MODE

The MCU mode is the "Arduino like" mode. We went to this mode from the AT mode by just typing **ATMC**. Then, the Arduino-loop started.

#### The Arduino-like functions

The source code was located in Mesh_Bee/src. One of the file was called **ups_arduino_sketch.c** . This file contained the two Arduino-like functions : **arduino_setup** and **arduino_loop**.

The **arduino_setup** function was called when entering in MCU mode, through the function **ups_init** we found in **firmware_aups.c** (AUPS stands for Arduino User Programming Space). This function also called **suli_init**, so there was no need to initialize Suli (see below) if the device is in MCU mode. In the **arduino_setup** function, everything that needs to be done at the beginning of our program was written, for example initialization of peripherals.

The **arduino_loop** was a function which was called  periodically. WE defined the time between two loops by typing in AT mode, **ATMFXXXX** where XXXX was a number between 0 and 3000 and was the time in milliseconds between two **Arduino_Loop**. This Arduino_Loop function was a task, defined in **firmware_aups.c**,  and either called **arduino_loop** or exit MCU mode to AT mode if **+++** was read.

We noticed that here we had to use the C preprocessor to know for which device we were compiling the program for.

	#ifdef TARGET_COO
	// Code for the coordinator
 	#elif TARGET_ROU
	// Code for the router
	#else
	// Code for the end node
	#endif

We then knew where to write code. We needed to know what code to put in these two functions.

#### The SULI Library

SULI is a library written by SeeedStudio to easily use the peripherals. It is imported it by just adding :

	#include "suli.h"

When the device is in MCU mode, we did not need to initialize Suli. Otherwise :

	suli_init();

To remember the pin disposition, we put the picture above, in **IMPORTANT DOCUMENTS**.

##### Simple GPIO

Our first experiment was with simple input/output. First, we needed to initialize the pin. We needed to use the **void suli_pin_init(IO_T *pio, PIN_T pin)** function. It took two arguments : an **IO_T** variable that contained the properties of the pin and the pin number.

	IO_T led_io;
	suli_pin_init(&led_io, D9);

The pin number was either an int or a defined pin name (they were defined in suli/suli.h) :

	D0 = 0, D1=1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15, D16, D17, D18, D19, D20, DO0=33, DO1=34, A3=0, A4=1, A2=50, A1, TEMP, VOL

Then we had to set the direction, input or output, of the pin. We used the **void suli_pin_dir(IO_T *pio, DIR_T dir)** function, which took a IO_T argument and a direction, which could either be **HAL_PIN_OUTPUT** or **HAL_PIN_INPUT**.

	suli_pin_dir(&led_io, HAL_PIN_OUTPUT);

The state of a pin can be read with **int16 suli_pin_read(IO_T *pio)** and be written with **void suli_pin_write(IO_T *pio, int16 state)**. The state can either be **HAL_PIN_HIGH** or **HAL_PIN_LOW**

	suli_pin_write(&led_io, HAL_PIN_HIGH);

A pulse could be read with uint32 suli_pulse_in(IO_T *pio, uint8 state, uint32 timeout), which returns the length of the pulse in microseconds.

##### ADC

Like for simple input/output, the pin needed to be initialized for analog reading first. We used **void suli_analog_init(ANALOG_T * aio, PIN_T pin)** where ANALOG_T was the equivalent of IO_T. For the pin, there was choice between **A3=0, A4=1, A2=50, A1, TEMP, VOL**. Then, we were able to read with **int16 suli_analog_read(ANALOG_T *aio)**.

	ANALOG_T temp_pin;
	suli_analog_init(&temp_pin, TEMP);
	int16 temper = suli_analog_read(temp_pin);

##### Time functions

We found two functions in Suli to wait a certain amount of time : **void suli_delay_us(uint32 us); ** for microseconds waiting and **void suli_delay_ms(uint32 ms);** for milliseconds waiting. We also found a function to know for how long the program had been running : **uint32 suli_millis(void);** for milliseconds and **uint32 suli_micros(void);** for microseconds. There could be overflow, after 50 days for suli_millis but after 70 minutes for suli_micros.

##### I2C

I2C worked the same way than ADC and input/output. There were an initialization function **void suli_i2c_init(void * i2c_device);**, a write function **uint8 suli_i2c_write(void * i2c_device, uint8 dev_addr, uint8 *data, uint8 len);** and a read function **uint8 suli_i2c_read(void * i2c_device, uint8 dev_addr, uint8 *buff, uint8 len);**. For I2C, as we had no choice for the pin, specify the i2c_device is useless. We just gave NULL.

	suli_i2c_init(NULL);
	uint8 data = TRIGGER_HUMD_MEASURE_NOHOLD;
	suli_i2c_write(NULL, HTDU21D_ADDRESS, &data, 1);
	uint8 msb;
	suli_i2c_read(NULL, HTDU21D_ADDRESS, &msb, 1);

We wrote a use example of I2C in **src/humidity.c**. This is a driver to read data from a humidity and temperature sensor (see below).

##### UART

Like I2C, we had no choice for the UART Port, it had to be UART1. So, in the initialization function **void suli_uart_init(void * uart_device, int16 uart_num, uint32 baud);**, there was no need to specify uart_device and uart_num. The baud can be : 4800, 9600, 19200, 38400, 57600 or 115200. We noticed that by default, the baud rate is initialized to 115200, so there was no special need to initialize it.

For the writing part, there was a general function, **void suli_uart_send(void * uart_device, int16 uart_num, uint8 *data, uint16 len);** which just took an array of data and the length of this array. Then, there were more specific functions : void suli_uart_send_byte(void * uart_device, int16 uart_num, uint8 data);, void suli_uart_write_float(void *uart_device, int16 uart_num, float data, uint8 prec);, void suli_uart_write_int(void * uart_device, int16 uart_num, int32 num);. However, we wanted to use printf-like function, much easier to use. So, most of the time, we used **void suli_uart_printf(void *uart_device, int16 uart_num, const char *fmt, ...); ** (the three dots are a notation of C to say that the number of argument is undetermined, as it is in printf).

	suli_uart_printf(NULL, NULL, "<HeartBeat%d>\r\n", random());

For the reading part, we had a function to know if there was something to read : **uint16 suli_uart_readable(void *uart_device, int16 uart_num);** which returns one if uart has received readable data, zero otherwise. Then we were able to read a byte with **uint8 suli_uart_read_byte(void *uart_device, int16 uart_num);**

That's it for Suli. There was nothing hard here and if we wanted more advanced functions, we will have to dig into API provided by NXP. However, **suli/suli.c** can be useful as there are examples of use of the NXP API.

### API MODE

The API was a simple way to communicate with the MCU from outside. The messages had a particular form, which was explained in details in the [User's Manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_User_Manual_v0.3.pdf). It was also possible to call API commands from MCU mode.

#### API Frames

TODO, for now read the [User's Manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_User_Manual_v0.3.pdf). It is important to understand it as the Raspberry Pi communicates with the MCU with API frames (see below).

#### Send API Commands from MCU Mode

To send an API Command, we needed to build a **tsApiSpec**, which is a representation of an API Frame. Here is what was inside :

	/* API-specific structure */                                                                             
	typedef struct                                                                                           
	{                                                                                                        
 	    uint8 startDelimiter;   // start delimiter '0x7e'                                                    
	    uint8 length;           // length = sizeof(payload)                                                  
	    uint8 teApiIdentifier;  //indicate what type of packets this is                                      
	    union                                                                                                
	    {                                                                                                    
	        /*diff app frame*/                                                                               
	        uint8 dummyByte;            //dummy byte for non-information frame                               
	        tsNwkTopoReq nwkTopoReq;                                                                         
	        tsNwkTopoResp nwkTopoResp;                                                                       
	        tsLocalAtReq localAtReq;                                                                         
	        tsLocalAtResp localAtResp;                                                                       
	        tsRemoteAtReq remoteAtReq;                                                                       
	        tsRemoteAtResp remoteAtResp;                                                                     
	        tsTxDataPacket txDataPacket;                                                                     
	        tsOtaNotice    otaNotice;    //OTA notice message                                                
	        tsOtaReq otaReq;                                                                                 
	        tsOtaResp otaResp;                                                                               
	        tsOtaStatusResp otaStatusResp;                                                                   
	    }__attribute__ ((packed)) payload;                                                                   
	    uint8 checkSum;                             //verify byte                                            
	}__attribute__ ((packed)) tsApiSpec;

Let's go through it. We did not care about **startDelimiter**, it was always 0x7e and was set for us by a function. Then came the **length** of the payload, which was the useful data in the Frame. The **teApiIdentifier** was the id to identify a packet. It can be one of the following option :

	typedef enum
	{
	    /* API identifier */
	    API_LOCAL_AT_REQ = 0x08,      //local At require
	    API_LOCAL_AT_RESP = 0x88,    //local At response
	    API_REMOTE_AT_REQ = 0x17,    //remote At require
	    API_REMOTE_AT_RESP = 0x97,   //remote At response
	    API_DATA_PACKET = 0x02,      //indicate that's a data packet,data packet is certainly remote packet.
	    API_TEST = 0x8f,             //Test
	    API_OTA_NTC = 0xd3,
	    API_OTA_REQ = 0xb0,
	    API_OTA_RESP = 0x06,
	    API_OTA_ABT_REQ = 0xf7,
	    API_OTA_ABT_RESP = 0xdb,
	    API_OTA_UPG_REQ = 0x5a,
	    API_OTA_UPG_RESP = 0xe6,
	    API_OTA_ST_REQ = 0x91,
	    API_OTA_ST_RESP = 0x89,
	    API_TOPO_REQ = 0xfb,
	    API_TOPO_RESP = 0x6b
	}teApiIdentifier;

Depending of what we were sending, we had to choose the correct id. They were described in the [User's Manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_User_Manual_v0.3.pdf). Then, there was a union, **payload**. In C, it simply meant that we had to choose one of the following options. The \_\_attribute\_\_ ((packed)) meant that the compiler had to put all the fields together, without a hole, in the memory : they were "packed"". We noticed that in the payload, we had for example **localAtReq** to make, as the name said, a local At request. To find the exact definition of a payload, we went to **include/firmware_at_api.h** Then, finally, we had a checksum to be a bit more sure that data are transmitted without problems (like a bit switch).

Most of the time, we did not have to complete the tsApiSpec myself. Functions were provided to make it easier. It was also the case for **tsLocalAtResp** and **tsRemoteAtResp**.

For example, if we wanted to send data to another node in the network. It could be done by sending a API command.

	#include "firmware_at_api.h"
	#include "firmware_api_pack"
	//Those two include contains the function to easily create tsApiSpec and send them

	tsApiSpec apiSpec;
	uint8 tmp[sizeof(tsApiSpec)]={0}; // Will contain the final result
	//For now we put in it a string
	sprintf(tmp, "TEST %d", 3); // Just a formatted string (like in printf) in tmp
	PCK_vApiSpecDataFrame(&apiSpec, 0xec, 0x00, tmp, strlen(tmp)); // This function create a DataFrame
	uint16 size = i32CopyApiSpec(&apiSpec, tmp); //We actually create the array containing the API Frame in tmp
	API_bSendToAirPort(UNICAST, 0x0000, tmp, size); // We send the message to 0x0000, the coordinator

To fill tsApiSpec, we used, for data, **void PCK_vApiSpecDataFrame(tsApiSpec *apiSpec, uint8 frameId, uint8 option, void *data, int len);** in **firmware_api_pack.h**. The frameId just identified the frame, we put whatever we wanted, it was not that important. The option was 0 for UNICAST, 1 for BROADCAST. It was also important to create the actual frame inside an array with **int i32CopyApiSpec(tsApiSpec *spec, uint8 *dst);** also in **firmware_api_pack.h**. It returned the size of the frame, which was useful when we wanted to send my frame with **bool API_bSendToAirPort(uint16 txMode, uint16 unicastDest, uint8 *buf, int len);** in **firmware_at_api.h**. The txMode can be either UNICAST (to only one node) or BROADCAST (to all nodes). Then we had to specify an address which was only used if we were in UNICAST mode (otherwise it did not matter), the frame array we had just created and its length.

We were also able to call AT commands thanks to **int API_i32AtCmdProc(uint8 *buf, int len);** in **firmware_at_api.h**. We just gave it a command and its length and it returned if the function had succeeded or not.

	char * aj = "ATAJ1";
	API_i32AtCmdProc(aj, 5);

We found other functions in firmware_at_api.h and firmware_api_pack.h but for now we did not find them useful. It was for example possible to send a message to a given Mac address. We explored the code if we wanted to go in more details.

### First Experiments

The first experiments we carried out when we received the MeshBees allowed us to better understand how it worked. We began with a simple hello word, with a LED.

#### Hello World !

The circuit was really simple : a Led and a resistor connected to D9.

![led](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/led_scheme.png)

The code we used was also simple.

	#include "suli.h"

	IO_T led_io;
	int state;

	void arduino_setup(void){
		suli_pin_init(&led_io, D9);
		suli_pin_dir(&led_io, HAL_PIN_OUTPUT);
		state = HAL_PIN_OUTPUT; // contains the current state of the LED
		suli_pin_write(&led_io, state);
	}

	void arduino_loop(void){
		state = ~state; // We just exchange the state of the pin

		suli_pin_write(&led_io, state); //Blink

		suli_delay_ms(1000); // wait one second
	}

Note that instead of waiting actively with suli_delay_ms, we could also change the period of the arduino loop with **ATMF1000**.

#### Read a Brightness

To measure the brightness, we used a photo-resistor : the higher the brightness the lower the resistance. So, to measure it, we did a voltage divider with a resistor of 5K ohms.

![photo-resistor](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/photores.png)

We connected the middle point with ADC3, i.e. D0. Here was the code, we read the brightness and sent it to the coordinator :

	#include "suli.h"
	#include "firmware_api_pack.h"
	#include "firmware_at_api.h"

	ANALOG_T brightness_pin;

	void arduino_setup(void){
		suli_analog_init(&brightness_pin);
	}

	void arduino_loop(void){
		uint8 tmp[sizeof(tsApiSpec)]={0};
		tsApiSpec apiSpec;

		int16 bright = suli_analog_read(brightness_pin);
		sprintf(tmp, "BRIGHTNESS%ld\r\n", bright);
		PCK_vApiSpecDataFrame(&apiSpec, 0xec, 0x00, tmp, strlen(tmp));

		uint16 size = i32CopyApiSpec(&apiSpec, tmp);
		if(API_bSendToAirPort(UNICAST, 0x0000, tmp, size))
		{
			suli_uart_printf(NULL, NULL, "Brightness read %ld", bright);
    		}

		suli_delay_ms(1000); // wait one second
	}

Note that we could also read the internal temperature the same way : the initialization of the pin was replaced by **suli_analog_init(&temp_pin, TEMP);**.

#### A driver for HTU21D

HTU21D was a humidity sensor which can also output a temperature. It was communicating by I2C with the MCU. The code for it is in **src/humidity.c**. There are three functions. The first one was **void init_humidity(void)**. It simply initialized everything to read the sensor, here it was only initializing I2C.

Then we wrote two functions. One to read humidity : **unsigned int read_humidity(void)**. It returned a raw_humidity. To get the real one, we just computed :

	-6 + (125 * rawHumidity / (float)65536);

The function request to read humidity to HTU21D. Then, it waited for the result to be available and read it. The message read was composed of three parts : the most and least signification bytes of the humidity and a checksum to be sure of what we read. We noted that inside the humidity value, there were two status bits we needed to erase.

The temperature reading worked the same way, with **unsigned int read_temperature(void)**. It returned the raw temperature value, so to get the real one, we needed to compute :

	(float)(-46.85 + (175.72 * rawTemperature / (float)65536))

#### A driver for LSM9DS0

We would like to detect taps. To do so, we needed an accelerometer which can generate interrupts on taps. We had a LSM9DS0, so we used it. To communicate, we used I2C. In the meantime, there were a gyroscope and a magnetometer, so we used it. The driver could be found in **src/LSM9DS0.c**. To initialize the LSM9DS0, we wrote an initialization function, **uint16 init_LSM(LSM\_parameters params);**. It took the parameters to configure the LSM9DS0. There were defined in **include/LSM9DS0.h**. Then, we calibrated the gyroscope and the accelerometer with **void calLSM9DS0(LSM\_properties* prop);**, the bias were stored in LSM\_properties, in the fields **float abias[3]** and **float gbias[3]**.

We wrote functions to read the accelerometer, the gyroscope, the magnetometer and even the temperature. There are : **void   readAccel(LSM\_properties* prop);**, **void   readMag(LSM\_properties* prop);**, **void   readTemp(LSM\_properties* prop);** and **void   readGyro(LSM\_properties* prop);**. These functions write the read values in the fields of LSM\_properties, in ax, ay, az, gx, gy, gz, mx, my, mz and temperature. However, these are raw values. We used functions to do the conversions : **float  calcGyro(int16 gyro);**, **float  calcAccel(int16 accel);** and **float  calcMag(int16 mag);**.

All these functions were just reading and writing in registers. The register description was found in the [documentation](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/LSM9DS0.pdf). Finally, we wrote functions to configure interrupts. These interrupts can be on pin **INTG** for the gyroscpope and on pins **INT1XM** and **INT2XM** for the accelerometer and the magnetometer. The functions were **void configGyroInt(uint8 int1Cfg, uint16 int1ThsX, uint16 int1ThsY, uint16 int1ThsZ, uint8 duration);** and **void configTapInt(float threshold, int8 duration);**. They take configuration (int1Cfg was what went in the register INT1_CFG_G), thresholds and a duration for the interrupt. The tap interrupt is on **INT1XM**.

We connected the LSM9DS0 to the MeshBee (3.3V, GND, SDA, SCL and INT1XM) and used the interrupt handler we defined (see next part).

### More in Depth

#### Create new Tasks

While trying to do time synchronization, we needed to call a given function every one second. In JenOs, we needed to create a new **Task**. Tasks are special functions which can only be called by the OS. They are run in parallel of the main function (so we needed to be careful with shared resources). The first thing we had to do was to say to the OS to add a new task. This is done in the file **src/MeshBee.oscfgdiag**. When we opened the raw files, there were a lot of text and it was hard to understand everything. NXP provides a graphical interface to edit this file. We used it.

In the **C:/Jennic/Tools**, we found eclipse, a well-known code editor. We opened it and chose **C:/Jennic/Application** as the Workspace. The file is still a raw text when we opened it. We had to choose plugins in **C:\Jennic\Tools\Eclipse_plugins**. Once the plugins installed, the raw text became a diagram.

![Eclipse configuration file](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/eclipse.png)

We saw, on the left, the project explorer where are our files. On the right, there are the components that can be added in the diagram, which is in the middle if it was opened. At the bottom, there are the properties of the selected component. If it is not there, it can be added by clicking on the bottom left hand corner icon and by selecting "Properties".

We looked at diagram in more details.

![Diagram](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/global_config.jpg)

We saw that there are different kinds of blocks : Tasks, Software Timer, mutexes... These blocks were linked to each other in a meaningful way. For instance, a task was able to use a mutex to enter in a critical section. We noticed that sometimes the documentation of JenOs assumed that some blocks are declared. For example, it was assumed that there was a task declared for time synchronization or that there was a interrupt handler for System Controller's interrupts. So, the first time we discovered it, it was a bit surprising that given function did not work. If it was written to give a callback function for an interrupt, even if the function existed, if there was nothing in the configuration file to declare the interrupt and link it to an ISR (see later to know what it is), the function was useless and did not work at all.

We created a task to synchronize time. The task name was **APP_ZCLTask**. The goal of the task was to keep the count of the time (see later). We just selected **Task** in the Palette. Then, we clicked in a free space in the diagram. The Task box appeared. We gave it a name and a priority (for the scheduler). Then was created links with other components. Some links can be created directly by using the input and output arrows at the border of the box (they appeared when passing the mouse above the box). Some links need to be selected in the Palette. It was the case for **Critical Section**

![Task](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/zoom_softtimer.jpg)

The task was declared to the OS. A file will be generated at compile time to transform those information into code. The files generated were called **build/gen/os\***.

We then had to add code to explain to the OS what to do with the task. To declare the task, we used the macro : **OS_TASK(task_name)**. For example, if the task's name was App_Task, it looked like :

    OS(APP_Task) {
        // Code
    }

The task was created. We needed to say when to execute our task. The easiest way to it was to use the function **OS_teStatus OS_eActivateTask(OS_thTask hTask);**. It took the task name as an argument and returned if it succeeded or not : OS_E_OK (successful), OS_E_BADTASK (invalid task handle used) or OS_E_OVERACTIVATION (maximum number of activations exceeded: 65535). It asked the OS to schedule the given task. However, it was quite limited because we needed to know exactly when to call the task. However, we needed to call the task every second. So, we needed to use a timer, the one we declared in our configuration file. We had a function to say to start the timer : **OS_teStatus OS_eStartSWTimer(OS_thSWTimer hSWTimer, uint32 u32Ticks, void *pvData);**. It took the timer's name, the number of ticks before the timer ends (we used the macro **APP_TIME_MS(n_milliseconds)**), and data which are only useful for callback functions (here we provided NULL for our task). It returned a status which could be OS_E_OK (successful), OS_E_BADSWTIMER (invalid software timer handle) or OS_E_SWTIMER_RUNNING (software timer already running). Once our task was called, it was either possible to restart the timer with the same function or to continue to use the same timer (which continued to count while we did some computation) with **OS_teStatus OS_eContinueSWTimer(OS_thSWTimer hSWTimer, uint32 u32Ticks, void *pvData);** (returned the same status than OS_eStartSWTimer). Finally, we needed check the status of the timer with **OS_teStatus OS_eGetSWTimerStatus(OS_thSWTimer hSWTimer);** to know whether it had just expired or not. It returned either OS_E_BADSWTIMER (invalid software timer handle), OS_E_SWTIMER_RUNNING (software timer is running), OS_E_SWTIMER_STOPPED (software timer has been stopped) or OS_E_SWTIMER_EXPIRED (software timer has expired).

#### Application to Time Synchronization

Our first try with time synchronization was to use the given ZCL library (Zigbee Cluster Library). A cluster was something defined in a norm and was a kind of container of data which can be shared in the network. So, it provides us a structure to store data and functions to interact with it. It also update the time every second thanks to a timer and a task. However, at that time, we did not know that we needed to declared everything ourself in the configuration file. Finally, the library does nothing special as we needed to code almost everything. We used the **time cluster**. The ZCL library can be found in Jennic/Components/ZCL. We had to add the source files of this library to the Makefile.

The first thing we had to do was to give the time to the coordinator. To do so, the created a new **AT command** we called through the API frame. Creating new AT commands was quite easy. We opened the file **src/firmware_at_api.c**. Then, there are two variables : **atCommands** which is the array containing all AT command which could be called from the AT mode, and **atCommandsApiMode** which are commands which can be called from the API mode. We created a new command for API mode, to set the time. So, we added :

		{"ATST", ATST, NULL, API_setTime_CallBack},

to atCommandsApiMode. This is a structure of type **AT_Command_ApiMode_t**, which was defined as follow (in include/firmware_at_api.h) :

	typedef struct
	{
	    const char  *name;                  //AT command name
	    uint8       atCmdIndex;             //AT command index
	    uint16      *configAddr;            //config address
	    AT_CommandApiMode_Func_t function;  //AT commands call back function
	}AT_Command_ApiMode_t;

The name was simply "ATST" (it needed to be four chars). atCmdIndex was a unique number which identified the command and was generally a constant declared in **include/firmware_at_api.h**, in teAtIndex. ATST had as index 0x80. The configAddr was the address of a variable which would be set to the value given after the AT command (see above to learn more about AT commands). Finally, there was the function which was called for this command. The signature and content were :

	int API_setTime_CallBack(tsApiSpec *reqApiSpec, tsApiSpec *respApiSpec, uint16 *regAddr)
	{
		uint32 time;
		memcpy(&time, reqApiSpec->payload.localAtReq.value, 4);
		vZCL_SetUTCTime(time);
	
	    return OK;
	}

As an argument, we had the received API frame, the frame which would be the answer and the variable which was assigned before and which contained the value of the AT command. We used the memcpy function in order to copy the time stamp in the AT command. Then we used the **vZCL_SetUTCTime** function which took the current time in milliseconds as an argument to set the time of the cluster. To be able to use this function, the time cluster had to be initialized. We did that in the initialization of the node, in **src/zigbee_node.**, in the function **node_vInitialise**. We used the function **teZCL_Status eZCL_CreateZCL(tsZCL_Config config);**. This function received a configuration to initialize the cluster. This configuration contained for instance the callback method for the time cluster, but also other useful information. In the callback method, **cbZCL_GeneralCallback** in zigbee_node.c, we caught some event that can happen. Among them, there can be event to request the ZCL mutex.

It was important to mutexes when we had interrupts and concurrent accesses. Using mutexes allowed us to defined **Critical Sections**. Critical section is a part of the program which prevent the current task to be pre-empted by an other task from the same mutex group (linked to the same mutex in the configuration file), even by a task of higher priority. To enter a critical section, we used **OS_eEnterCriticalSection(name_mutex);** and to leave it : **OS_eExitCriticalSection(name_mutex);** in **vLockZCLMutex** and **vUnockZCLMutex** (zigbee_node.c).

Then, we had to call the Event Handler of the time cluster every second. For that, we had to configure a software timer, **APP_ZclTimer**, linked to a task, **APP_ZCLTask**, which belonged to the **ZCL mutex** group. In APP_ZCLTask (in zigbee_node.c), we called the event handler, **vZCL_EventHandler**, which took a tsZCL_CallBackEvent as an argument. We put in that structure the kind of event, here E_ZCL_CBET_TIMER.

Then, we had a system to count the time in seconds.

#### Time Synchronization V2

Counting in seconds was too restrictive for what we wanted to do. So we decided to rewrite all the synchronization part to be more flexible and count in milliseconds. The first thing we had to change was to use **uint64** instead of uint32. This way, we avoided overflow. In the meanwhile, we also changed the SULI library to also use uint64. The new time synchronization code was written in **src/time\_sync.c**.

The use was almost the same. There was a function to initialize the variables of the time synchronization : **void init\_time\_sync(void);**. Then, it was possible to set the low part and the high part of the timestamp with **void setHighTime(uint32 time);** and **void setLowTime(uint32 time);**. We had to separate the lowest part and the highest part because for the AT commands, we were only allowed to transmit four bytes of date, half of a uint64. Then, we were able to check whether the time is synchronized or not with **int timeHasBeenSynchronised(void);** and finally, we can get current time with **uint64 getTime(void);**. All in all, we replaced all previous functions.

However, we erased the timer part as we did not need to count seconds anymore : we used the SULI **suli_millis** function to keep track of the time. We still used mutex as we had concurrent accesses.

As we separated the highest part and the lowest part of the timestamp, we needed to do two different AT function to set the time. ATST was replaced by **ATSH** (for Set High) and **ATSL** (for Set Low). The way it worked was exactly the same, except that it called the two functions to set the time.

Once we did that, the communication protocol changed. A frame was then composed of four bytes to describe the value read, eight bytes for the timestamp and four bytes for the data, which was then totally raw (whereas before it was a string). So, the size was fixed.

#### Time synchronization : drift correction

Once we had the framework to synchronize the time, we wanted to check what was the error of time between all nodes of the network and the server, and how it evolves with time. At first, we were resynchronizing the time every minute, so the error was low, some milliseconds. However, we tried to only synchronize time once or a limited amount of time and to see how the system evolves. The first experiment we carried out was to compare the time between two MeshBees. Both of them were connected to the Raspberry Pi via wires and Zigbee. The MeshBees synchronized their internal time with the server at the beginning of the experiment. Then, the server sent at regular intervals signals via the wire which triggered an interrupt which sent the current local time. When we plotted the time difference between two MeshBees, we observed a drift.

![time sync two meshbee 1 hour](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/time_sync_1_hour_2meshbees.png)

(the time step is 30s) We observed a small drift, of around 0.2 ms per minute. The error can become too big with after a lot of time without synchronization but synchronizing once every 45 minutes gave us an error of less than 10ms. Next, we tried to see the drift of a MeshBee with the server.

![time sync meshbee server](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/time_sync_8_hours_meshbee_server.png)

We observed a huge drift, more than 5 ms per minutes. At first, we did not really understand where it was from. However, as it was very linear, we tried a simple linear regression to correct it.

![linear regression correction](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/time_difference_simple_regression_server_meshbee.png)

We had some noise due to Zigbee communication but the standard deviation was less than 10 ms. There was almost no drift : 0.012 ms per minutes, even if it was hard to say with the noise. The problem with this approach was that we needed to record some samples before being able to correct the read values. To remove this problem, we had to understand where the drift problem was from.

We first thought it was because of a stop of the timer due to interrupts, but this was very unlikely because we used a hardware timer. We checked that by using an oscilloscope and measuring the clock frequency directly. There seemed to be a small problem here. The oscilloscope was not precise enough to be exactly sure, but it seemed than the MeshBee clocks were too fast. This hypothesis was not directly considered because an external clock was used and it was supposed to be very precise. However, by reading the documentation, we learnt that the clocks had a frequency tolerance of more or less 40ppm (0.004%). These contraints came from the constraints we had on UART and Zigbee communications. So the 32MHz external clock had a frequency of 32MHz more or less 128kHz, the peripheral frequency (main clock divided by 2) was 16Mhz more or less 64kHz and after the prescaler we used to count time (divided by 16), we had a frequency of 1Mhz more or less 4kHz. So, at the end, in the worst case, we had a error of 4ns every microsecond. So, after one second, we could have an error of 4ms. We measured something smaller but it could be worst.

We explored the possibility to do a calibration of the clock the first time the MeshBee is used with a wired connection. To do so, the MeshBee was connected to the Raspberry Pi with four wires : VCC, GND, a synchronization enable signali (D18) and a clock signal (D1). The clock was generated with a PWM by the Raspberry Pi. A script was used : **synchronize.py**. Then, the MeshBee caught the interrupto and compare the PWM frequency (which is supposed to be known) and its internal time counter. The correction was computed (in **utils\_meshbee.c**) and saved permanently in the EEPROM memory by using the PDM library (see in **time_sync.c**).

![Time wire corrected](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/time_sync_wire_correction_meshbee_server.png)

We observed that the drift was reduced and that the noise was still here. There seems to be still a small drift but like for the linear regression correction, it was hard to estimate it on a short time interval. The advantages here were that we did not have to compute coefficient by receiving time steps. We only needed the first time stamp to give a good estimate of the future times. However, we cannot correct the bias there can be with only one time stamp. In fact, this bias is due to the noise in Zigbee communication, which makes the first value not centered. However, this bias could be estimated online with similar technics we used for the linear regression, but we would lose some advantages of the wired method. Another advantage of wire synchronization compared to the linear regression approch is that it does not depend on the size of the network. The big networks the noise would be higher and so linear regression less precise. However, the bias of the wire method will also be higher.

#### Interrupt From a Button

To try our time synchronization, we needed a physical common event. Our first experiment was carried out with a simple button. Later, it would be replaced by an interrupt from a sensor for example. An interrupt was something which has to be scheduled by the OS, so we need to declare it in the configuration file. The document claims that they have a function a register a callback for I/O interrupts but it did not work. So, we declared an interrupt source, **System Controller"". System controller gathered different kind of interrupt among which the IO ones. Then we declared a task, **sysctrl_callback**, which was stimulated by the interrupt source we had just created.

![interrupt](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/zoom_interrupt.jpg)

The interrupt function was defined in **src/utils_meshbee.c**. It sent a simple frame to the server when the button was pressed. The button was initialized
in the arduino_setup function. Suli was not enough so we had to use function from JenOs :

	uint32 mask = (1 << D0);
	vAHI_DioSetDirection(mask, 0);
	vAHI_DioInterruptEdge(0, mask); // First argument for rising edge
	vAHI_DioInterruptEnable(mask, 0);

The mask simply indicated which IOs are concerned by a change. We needed to set to 1 the bit at the position of the number of the IO. **vAHI_DioSetDirection** set the direction of the pin, the first arguments are the pins we wanted to change to input and the second to output. **vAHI_DioInterruptEdge** allowed us to change the edge which triggers the interrupt, the first argument being falling and the second rising. Then **vAHI_DioInterruptEnable** was used to enable the interrupt (first argument) or to disable it (second argument).

## The Interface

To do the interface between the MeshBee network and the server side, we used a raspberry pi which was connected directly to the coordinator by UART.

### Configuring the Raspberry Pi

We used a Raspberry Pi 3. It was pretty cool because it was powerful enough to also run a server (see later) and had a Wifi chip integrated. For the OS, we used Raspbian. The first thing we had to do when we received a new Raspberry Pi was to install the OS. We used a 8Go SD card (less is not enough for the full version of Raspbian). We followed [adafruit raspberry pi tutorial](https://learn.adafruit.com/category/learn-raspberry-pi) for example to know how or we searched on Google. During the configuration, when we were in raspi-config, we did not forget to activate the serial communication in the advanced parameters.

We needed to configure the Serial port to communicate with the MeshBee. We found the serial port in **/dev/ttyS0** on RP3 or **/dev/ttyAMA0** for older versions. The problem was that by default, the raspberry is configured to have a terminal on this port. So we needed to remove it. To do so, we opened the file **/boot/cmdline.txt** :

	sudo nano /boot/cmdline.txt

and we removed the part with ttyAMA0 or serial0 (console=serial0, 115200). We had something like :

	dwc_otg.lpm_enable=0 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait

(no serial0). What followed is for the Raspberry Pi 3. We needed to say that we activated UART. For that, we had to open the file **/boot/config.txt**

	sudo nano /boot/config.txt

and to add the lines

	enable_uart=1
	core_freq=250

For the version of Raspbian we used, we needed to change the core frequency. It may be corrected in future version. We had problems with the communication with the MeshBee. So, we took an oscilloscope and measured the baud rate and instead of being 115200, it was around 72000.

We were then ready to run our program. With the complete version of Raspbian, we did not have to install git and python libraries to manipulate the GPIOs. We cloned our Mesh_Bee repository :

	git clone https://github.com/Aunsiels/Mesh_Bee.git

Also, as we do not like that much nano, we installed another editor, like vim. Raspbian came from Debian, so we also found apt-get

	sudo apt-get install vim

### Connections

Obviously, to communicate with the MeshBee via the serial port, we needed to link it to the Raspberry Pi. Here is how the pins are organized on the Raspberry Pi 3.

![RP3 pin layout](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/pi3_gpio.png)

To give power to the MeshBee, we connected the 3.3V pin of the MeshBee to the 3.3V pin of the Raspberry Pi (pin 01). We did the same for Ground (GND) which was on ports 06 or 09 for example. Then, and it was **important** to do it correctly, to connect the TX1 pin of the MeshBee to the RXD0 pin of the Raspberry Pi (pin 10) and the RX1 pin of the MeshBee to the TXD0 pin of the Raspberry Pi (pin 08). We had something like that :

![connect mb rp](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/connect_meshbee_rp.jpg)

Note that we added a LED (and a resistor !) connected to the ASSOC pin of the MeshBee to be sure that the MeshBee was connected and working.

Now that we had the MeshBee connected, we tried to communicate with it. For that, we used software cutecom. It was easy to use. We installed it :

	sudo apt-get install cutecom

Then we opened it by typing **cutecom** in the terminal.

![cutecom](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/cutecom-0.14.0.png)

Parameters were good when you open it. We just checked that the device was **/dev/ttyS0** and at the bottom that we had **CR line end**. We then tried to send **AT** commands for example like we saw above.

### The Python Script

At that stage, we were sure that we were communicating with the coordinator. We needed to automatize the communication and to write a program which can make the connexion between the MeshBee network and the server side. The script was **Mesh_Bee/rpi_script.py**. A MeshBee was represented by a class, MeshBee.

The first things we wanted to do was to be able to change the mode of the coordinator to be sure that we are in the right one. To begin with, we needed to initialized the serial connexion, when a MeshBee object is created. We used the **serial library**, which is install be default in the full version of Raspbian. The baud rate was still **115200**, and we set the timeout to 1, to be sure we do not get stuck at some point of the program. Then, to facilitate the communication, we used a buffer over the serial. So, to write, we used **self.meshbee.write** with **self.meshbee.flush()** (this is very important if you want to actually send). To read, we have **self.meshbee.readline** or **simply self.meshbee.read(n);** where n is the number of byte to read.

To know if we did change the mode, we checked what the MeshBee answered. To be sure that there were no old data, we needed to empty the buffer before. So, we set the timeout to 0 and read until there was nothing left. We wrote three functions, **to_at_mode()**, **to_data_mode()** and **to_api_mode()** to either go to AT, DATA or API mode.

The first experiment we did was only using AT mode and DATA mode. Actually, we used the AT to retrieve information about the network and DATA mode to read the messages. We wrote two functions for the information : **print_information()** which printed information about the current node (it called ATIF, see above) and **print_nodes()** to show the nodes connected to the network. Finally, we had a function to send a message to the entire network, **write(message)**. Then, we simply read messages in a loop.

The first protocol was very simple : 4 bytes to identify the value read, the id (the mac address) on 8 bytes and then the value read on the rest. At that time, we did not have time synchronization, so no time stamp. However, the mac address is also contained in the DATA frame, we did not have access yet.

The problem was that changing the mode can be long (up to a second) and moreover, we lost data by changing the mode. In fact, we wanted to use the **ATLA** command to check which nodes are connected to the network (this information can be useful for the user). So, we decided to only use the **API mode**. Thus, we needed to construct and decrypt API frames (see above for API frame structure).

We wrote a general function to read a frame : **api_read_frame()**. There could be different kind of frames : data frame, topology frame,... We recognized which kind of frame it was and then call the good function to decrypt the payload : api_data_frame(payload), api_topo_frame(payload),... While reading the data frame, we retrieved the MAC address, so it was removed from our custom protocol.

So, the new protocol looked like that : 4 bytes to identify the value read, 4 bytes for the time stamp and the rest for the value. We had a time stamp. To share the time, we broadcast an AT command, **ATST**. This was done thanks to the function **send_time()**, which sent the time in second. Finally, we wrote a function to request the nodes on the network : **request_topo()**. We did not have to wait for the result as we had done before : each module answered with a frame which was decrypted when received. The mac addresses were stored in an attribute of MeshBee, and they were all sent with **send_mac_addresses()**.

Once we changed the time synchronization part, the protocol became : four bytes to describe the value read, eight bytes for the timestamp and four bytes for the data, which was then totally raw (whereas before it was a string). So, the size was fixed (see above). The **send_time()** function then sent time in milliseconds. **WARNING** : the way bytes were sent wass the opposite of the way it was in memory. This was because the **endianess** was not the same on the MeshBee (Big Endian) and the computer.

TODO, check endianess.

As the server was also on the Raspberry Pi, we just had to send our requests to the localhost domain, port 9000. Then to send all the mac addresses, we opened the URL **http://localhost:9000/updatesensors?s=** , followed by all MAC addresses.

Finally, we wrote a main function to call all our functions. This is done in an infinite loop. It just called api_read_frame, and from time to time request to read the network topology and send a time stamp.

To run the script, simply type **python2 rpi_script.py** in the Mesh_Bee directory.

## Server

### Introduction

We wrote a HTTP server to retrieve information from the MeshBee network and display it to the user. The server was written using Play Framework, with Scala. We will not explain how Play Framework works nor how to program in Scala. To run the server, we went to **Mesh_Bee/server/bin** and typed **./activator run**. Then we saw the port number appear. The project compiled the first time a page was required.

The configuration of the route can be found in **conf/routes**. The HTML files were in **app/views** and the controllers were in **app/controllers**. The main controller was **HomeController.scala**. The code for the real time printing of data thanks to Web sockets was in **MyWebSocketActor.scala**.

### Record Sensor Data

The first communication we did with the server was with data directly in the HTTP request. So, to send new measured data to the server, we needed to send, to **/measuredata** :

	    tosend = "http://localhost:9000/measuredata?id=" + id_sensor
        tosend = tosend + "&dataType=" + data_type  + "&data=" + data
        tosend = tosend + "&time=" + str(time)

 The measured data was caught by the **measureData** function in HomeController.scala. Then, if the data was a special type, we did some computation with the data (for instance, for the temperature, we needed to transform the raw data to degrees). Then, we created a Measure object and stored it. To keep the user updated, we also sent a web socket message to update the chart. Finally, we sent a message to confirm we read the measure.

 The measured date was first stored in a global variable, **measures**. We needed to take care of concurrent accesses. Later, we would change it by a database access, using MongoDB for instance. However, for development purposes, global variables are easier and easy to clean.

### Real-time Communication

 The real-time communication was done thanks to web sockets. We handled them asynchronously, thanks to actors, in **app/controllers/MyWebSocketActor.scala**. When we created a web socket, we stored it into a map, mapping. So, when a user connected to the page, it created a web socket. When a new data was received, it was transmitted to all connected users. TODO, update the list of users when closing a web socket.

### Checking Connected Devices

 When the python wanted to tell which devices are connected, it sent the list of all MeshBee as a String. Then, we checked if all devices were in the String. Then, the user web page refreshed regularly to keep the printed device list updated. This is done using JavaScript, directly in the HTML page, in app/views.

### Charts

 To print the charts, we used a JavaScript library, **Highcharts**. The chart was then loaded when the user clicked on a given device.

 ![homepage](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/homepage.png)
