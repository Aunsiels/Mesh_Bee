#Documentation of MeshBee Project by Julien Romero

This document will present and explain my work during my **semester project**. The goal is to build a **wireless network of sensors for performance analysis**. I began the project in February 2016. I hope that thanks to this document you will be able to understand what I did, why I did it and to continue my work.

## Why MeshBee ?

### Choosing a Wireless Protocole

The first thing I had to think about was how to communicate. There are plenty of availible protocols and I have to choose among them the one which will best fit to my needs. I considered eight protocols : ZigBee, Bluetooth, BLE, Rubee, Zwave, ANT/ANT+, EnOcean and WiFi. To evaluate them, I chose four caracteristics : the data rate, the size,  the consumption and the range. Of course, this properties depends of a lot of parameters : hardware, environment,... So, I tried to choose values which appear in most of the description. The use cases are also useful to know what kind of things are possible with a protocol.

#### Zigbee

![Zigbee](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/Zigbee-logo.jpg) 

**Zigbee** is based on 802.15.4 specification, at 2.6 GHz. It is used in smart home, industrial control, medical tools, fire sensors... It is also used for low energy sensors, which is useful for us. A Ad Hoc network is created around a coordinator. Different architecture of the network are possible, we shall see it later. However, there can be problems with inferences as 2.4GHz is often used.

#### Bluetooth

![Bluetooth](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/bluetooth.png)

**Bluetooth** is based on IEEE 802.15.1 and works at 2.6GHz. It is used in smartphones, keyboards, mice, sensors,... The fact that it is in all smartphone popularized in. The consumption is medium.

#### BLE

![BLE](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/Bluetooth_Smart_Logo.jpg) 

The **Bluetooth Low Energy**, just like Bluetooth, works at 2.4GHz. We can see it as a low consumption version of Bluetooth, which exists to answer modern problems like consumption. So, it is used in healthcare, sport, sensors...

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

**WiFi** is a very popular protocol in personal application and is based on IEEE 802.11 . It operates on 2.4, 3.6, 5, and 60 GHz frequency bands. The speeds that can be acheive are high but the consumptions are also high. It is also used for the internet of things and a lot of ship are now design with wifi integrated (Rapsberry Pi 3, Arduino Genuino MKR1000, Red Pitaya, Electric Imp...).

#### Putting Everything Together

I summarized everything in an [odt file](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/semester_project_protocols.ods). The results are on sheet 1 and 2. I tried to plot some results.

![conso-vs-data-rate](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/conso_vs_data_rate.png) 

We can see that each protocole are in different areas and so, we need to choose the one which will be good for us. For that, we have to design a cost function according to our need. My function has four parameters (for the four parameters) that can be tuned. I also considered the chips availible (some are hard it get), how easy it is to program it, to get help, and the prices. Finally I decided to go for **Zigbee**

### Choosing a Zigbee Chip

There are plenty of Zigbee chips availible. What we would like to do is a chip which control both Zigbee and a sensor (it does not require power). So, we need to be able to program the prototyping chip directly, without having to use an additional board like an arduino. We also need to be able to read sensors thanks to i2c and ADC. We found a nice design, based on a NXP chip : **MeshBee**. It is **open source** (both software and hardware) and easy to prototype with. Thus, thanks to it, we can first try to write a program to test if our prototype is working and then design a new chip in which sensors are directly integrated. That will make the final result smaller.

## The System Architecture

For now, the system architecture is composed of three parts :

* The **MeshBee network** with sensors
* An **interface** between MeshBee and a server
* A **server** which collects information and display it for the user

In my implementation, both the interface and the server are on the **Raspberry Pi** (3) which creates a WiFi network. So, people can connect to it and visualize data.

For the Zigbee architecture, I use the **mesh architecture**. There are three kinds of nodes :

* **A _unique_ coordinator** : it is in charge of organizing the network and is directly linked to the Raspberry Pi.
* **Router** : it is able to propagate the network and is an access point to the network.
* **End-node** : it can only connect to the network.

![zigbee_network](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/meshbee_network_architecture.jpg) 

For the interface between the Zigbee world and the server, we use a serial communication. The data are then read by a **python script** which transform them into **http request** for the server. Then, the server organizes everything and is ready to display in real-time information from sensors.

![global_architecture](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/global_network.jpg) 

## The Embedded Software

The embedded software on the MeshBee have read data from a sensor and send information through the network to the main node, the coordinator. Let's see how it is done.

### The MeshBee Framework

First things first, I need to understand how MeshBee works.

![meshbee](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/MeshBeeArchitecture.jpg)

The MeshBee firmware is built on an OS provided by NXP, **JenOs**, and a **SDK for Low Energy Zigbee**, also provided by NXP. It is important to know that because sometimes, we shall not find the functions we are looking for (as interrupt-liked functions) in the MeshBee Framework but directly in the librairy provided by NXP.

Let's look inside the firmware. If you are used to **arduino programming**, you will find the same kind of architecture. The AUPS (Arduino-ful User Programming Space) provides you two functions : a **setup and a loop function**. The setup function is called during the initialization of the system and the loop function is called periodically (it is possible to specify the period).

In the middle, you can see the four modes of the MeshBee :

* **AT** : this is an **interactive mode**. You just have to send easy commands to the MeshBee and it will answer in a beautiful way. To go to AT mode from everywhere, you have to type ***+++***
* **API** : this is a more effective and formatted way to communicate with the MCU. Thanks to it, you can call AT function from inside and outside the MCU, via UART for example. You can also send query to other nodes through the network thanks to the API commands.
* **MCU** : this is the **arduino mode**. If you are not in this mode, the arduino loop will not be executed.
* **Data** : this is a **transparent mode** : all data received on the Zigbee network are directly transmitted to UART and all data sent via UART are broadcasted on the network.

The last thing to understand about this firmware is the **Suli** interface. This is a general librairy created by SeeedStudio. The goal is to make the interaction with GPIOs easier. So, for I2C, ADC,... we shall not need to call the functions from JenOs but we can use the easy functions provided by suli.

### Important Documents

Here are some documents and links which can be useful while developing with the MeshBee.

* The [MeshBee wiki](http://www.seeedstudio.com/wiki/Mesh_Bee) : on this page you will find useful information for the installation and use of the MeshBee. The documents up-to-date are (normally) also availible (some are not updated anymore).
* The [programmer wiki](http://www.seeedstudio.com/wiki/UartSBee_v5) : can always be useful.
* A [google group](https://groups.google.com/forum/#!forum/seeedstudio-mesh-bee-discussion-group) dedicated to MeshBee : try to ask your questions there but I am not sure they answer.
* The [User's Manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_User_Manual_v0.3.pdf)
* The [CookBook](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_Cook_Book.pdf) : useful to start using MeshBee.
* The [AT manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/AT_command_manual_for_Mesh_Bee_V1.0.pdf) : AT commands, not updated. See User's manual.
* The [Zigbee PRO Stack User Guide](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/JN-UG-3048.pdf) : provided by NXP
* [JenOS User Guide](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/JN-UG-3075.pdf) : provided by NXP
* [Peripheral API](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/JN-UG-3087.pdf) : provided by NXP
* [Datasheet JN516x](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/JN516X.pdf) : provided by NXP
* [Zigbee Cluster Library](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/JN-UG-3077.pdf) : provided by NXP

![MeshBee pins](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/600px-Mesh_Bee_Pin.jpg)

![Pin disposition](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/pin_dispo.png)

### Installation

To be able to compile our code, we need to install the tools given by NXP. I will summurize instructions given on [this page](http://www.seeedstudio.com/wiki/Mesh_Bee#9.1_Firmware_Downloads).

To be able to compile the program, you will need to use **Windows** (sorry for that).

The first thing we need to check is if whether the programmer is well configured or not. You only need to do that if it is the first time you use it. Download [this program](http://www.seeedstudio.com/wiki/File:FT_Prog_v2.8.2.0.zip) and unzip it with your favorite unzipper. Connect the UartSBee v5 to PC, open **FT_Prog** and configure it like this:

![programmer configuration](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/FT_Prog.png)

**THIS IS VERY IMPORTANT ! BE SURE THE PROGRAMMER IS ON 3.3V ! OTHERWISE, YOU WILL DESTROY THE MESHBEE.** To do it, use the rigth switch (if the antenna is up) and put it up to 3.3.

You also have to put the programmer in **PROG** mode thanks to the second switch, in a bottom position. This mode allows you to program the MeshBee.

Now, we need the tool to program the Meshbee. [Download it](http://www.seeedstudio.com/wiki/File:Jennic_flash_programmer.zip) and unzip it. Connect the Meshbee to the programmer. The antenna **MUST** be on the same size than the usb port. You can now open **FlashGUI.exe**. This is the software to flash the MeshBee. You should create a shortcut somewhere because you will need it a lot. Here are the steps to program the Meshbee :

![Program meshbee](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/400px-Flash_programmer1.png.jpeg)

You begin by choosing the binary file. There are in the **Mesh_Bee/build/output** directory. There are three of them : one for the coordinator, one for routers and one for end-nodes. Choose one to try. Next, you need to select the COM port, which is where your Meshbee is connect. You may have several of them if you have other devices connected. To know which one is the good one, unplug the programmer, look at the list, plug it again and the one which was added is your MeshBee. **Remember the number here**, after the COM. It will be usefull to communicate with the MeshBee later. Be sure that the Connect box is checked. Click on the Refresh button. If everything is ok, the MAC address of the Zigbee will be printed in the boxes. Otherwise, check that everything is connected and do the previous steps again. You can now program the MeshBee by pressing **Program**. You will normally receive a message which says that everything went well. You now have the program on the chip. Easy :)

Let's install the SDK now. Download the [SDK Toolchain JN-SW-4041](http://cache.nxp.com/documents/other/JN-SW-4041.zip), the [SDK Zigbee Smart Energy JN-SW-4064](http://cache.nxp.com/documents/other/JN-SW-4064.zip) and the [SDK ZigBee Home Automation JN-SW-4067](http://cache.nxp.com/documents/other/JN-SW-4067.zip). Install them in the **same directory**, C:/Jennic for instance. This is important as the compiler will look for them there.

You now have a program called Jennic Bash Shell. Open it. It is a linux-like console. Go to the build directory with **cd Mesh_Bee/build/**. You have script here for compilation. I had problems with them so you should run directly the command in the build files directly in the console. For the coordinator :


	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=COO clean
	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=COO all

For the router :

	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=ROU clean
	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=ROU all

And for the end-node :

	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=END clean
	make JENNIC_CHIP=JN5168 JENNIC_CHIP_FAMILY=JN516x PDM_BUILD_TYPE=_EEPROM TRACE_ALL=0 TARGET=END all

Compiling can be long. If you did something wrong, you will have an error message. Otherwise, the files in Mesh_Bee/build/output will be updated (check the date to be sure).

You should now know how the compile a program and flash it. It is time now to interact with the MeshBee.

### AT MODE

We are ready to communicate with the MeshBee. This communication uses **UART1**, so any USB-to-TTL working at **3.3V (this is super important)** device would work. We are going to use the programmer for now. Just switch the left switch to **UART1** (up).

We need a tool to communicate. I used [realterm](http://realterm.sourceforge.net/) on windows but feel free to use to one you like. [Download](https://sourceforge.net/projects/realterm/files/Realterm/) the latest version and install it.

![realterm](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/realterm1.png)

When you open it, they ask you to choose a display mode. In **AT mode**, we simply communicate with ascii so there is nothing to do here now. Go to the second tab, **PORT**. This is were we configure the communication. The Baud is **115200**, the is **no parity bit**, **8 data bits**, **1 stop bit** and **no flow control**. Do you remember the number after **COM** when you programmed the Meshbee ? You have to put this number in the Port field.

![Port](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/Ports1.png)

We can now send and receive data. Go to the **Send** tab. You have two boxes with *Send Numbers* and *Send Ascii* on the right. This is where you have to type the messages you want to send. When you send an **AT** command, it is important to have to have **CR** (which is \r) at the end of the line. To do so, **check the CR box**. I will not specify anymore this <CR> symbole at the end of each AT command.

![send](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/sendchars1.png)

Let's send our first command : **+++**. This allows us to go to AT mode. If you were in MCU mode before and the loop is too long, you might have problems to enter in AT mode. Just spam +++ while reseting the MeshBee (button on the side of the programmer). You will receive a *Enter AT Mode* if you were not in AT mode yet or a *Error, invalid command* if you were in AT mode. At least normally you receive something.

AT commands are composed as follow : **ATXX[YYYY]**. It always begins by  AT. Then follow two caracters to identify the command, for example IF or AJ. Then follow up to 4 optional numbers which are parameters of the command. For more about it, go to the AT commands section in the  [User's Manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_User_Manual_v0.3.pdf). Note that all commands are not avalible for all device type. For instance, an end-node cannot create a network and thus will not have this command.

Let's try some commands. Begin with **ATIF**. This will print information about the MeshBee. Check that the Device Type is the one you think it is. If you are the coordinator, send **ATPA1**. This command will create a new network. Reset the MeshBee and normally you will see the **ASSOC** led on the programmer light up. Send **ATIF** again and read the **PANID** field, which is the id of your network. You can now go to an other mode, for example data mode with **ATDT**.

![ATIF](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/400px-Configure_coo2.png)

Let's try to connect an other node to the network. Do not forget that you **always** need the coordinator to be on. Unplug it from the programmer and give it power, for example with the raspberry pi (see below). If you want to be sure that it is connected, you can also connect a LED (and a resistor !) to the ASSOC pin (you have the names of the pins below the MeshBee or see the picture above). Plug in the programmer a node that you have flashed with the router or end-node code (or flash it now).

As before, go to AT mode with **+++** and get information with **ATIF**. If the MeshBee was not configured before, the PANID should be 0x0000 or a number different from the one of the router. Let's say to our device to automatically join a network with **ATAJ1**. Then rescan the network with **ATRS** and normally after some seconds, the **ASSOC** led will light up. Check information with **ATIF** and now you should have the same PANID than the coordinator. To know what are the other nodes on the network, send the **ATLA** command. That will send a request to all nodes and you will receive information about all of them. If you go to data mode with **ATDT**, everything you type is broadcast on the network. If the coordinator is also linked to a serial port, for example the raspberry pi one (see below), you will see your message appear in the coordinator console.

We now know the basis of AT commands how to simply configure a MeshBee and how to send messages.

### MCU MODE

The MCU mode is the "arduino like" mode. To go to this mode from the AT mode, just type **ATMC**. Then, the arduino-loop will start.

#### The Arduino-like functions

The source code is located in Mesh_Bee/src. One of the file is called **ups_arduino_sketch.c** . This file contains the two arduino-like functions : **arduino_setup** and **arduino_loop**.

The **arduino_setup** function is called when we enter in MCU mode, through the function **ups_init** you can find in **firmware_aups.c** (AUPS stands for Arduino User Programming Space). This function also calls **suli_init**, so you do no need to initialize suli (see below) if you are in MCU mode. In the **arduino_setup** function, you need to execute everything that needs to be done at the beginning of your program, for example initialization of peripherals.

The **arduino_loop** is a function which is called  periodically. The time between two loops can be defined, in AT mode, by **ATMFXXXX** where XXXX is a number between 0 and 3000 and is the time in milliseconds between two **Arduino_Loop**. This Arduino_Loop function is a task, defined in **firmware_aups.c**,  and either calls **arduino_loop** or exits MCU mode to AT mode if **+++** is read.

You can notice that here we use the C preprocessor to know for which device we are compiling the program for.

	#ifdef TARGET_COO
	// Code for the coordinator
 	#elif TARGET_ROU
	// Code for the router
	#else
	// Code for the end node
	#endif

We know now where to write code. You need to know what code to put in these two functions.

#### The SULI Library

SULI is a library written by SeeedStudio to easily use the peripherals. To import it, just add :

	#include "suli.h"

If you are in MCU mode, you do not need to initialize suli. Otherwise, call :

	suli_init();

To know the pin disposition, see the picture above, in **IMPORTANT DOCUMENTS**.

##### Simple GPIO

Let's begin with simple input/output. First, we need to initialize the pin. We need to use the **void suli_pin_init(IO_T *pio, PIN_T pin)** function. It takes two arguments : an **IO_T** variable that contains the properties of the pin and the pin number.

	IO_T led_io;
	suli_pin_init(&led_io, D9);

For the pin number, is either an int or you can also use pin names (there are defined in suli/suli.h) : 

	D0 = 0, D1=1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15, D16, D17, D18, D19, D20, DO0=33, DO1=34, A3=0, A4=1, A2=50, A1, TEMP, VOL

Now, we need to set the direction, input or output, of the pin. We use **void suli_pin_dir(IO_T *pio, DIR_T dir)** function, which takes a IO_T argument and a direction, which can be either **HAL_PIN_OUTPUT** or **HAL_PIN_INPUT**.

	suli_pin_dir(&led_io, HAL_PIN_OUTPUT);

We can know read the state of a pin with **int16 suli_pin_read(IO_T *pio)** and write the state of a pin with **void suli_pin_write(IO_T *pio, int16 state)**. The state can be either **HAL_PIN_HIGH** or **HAL_PIN_LOW**

	suli_pin_write(&led_io, HAL_PIN_HIGH);

Note that we can also read a pulse with uint32 suli_pulse_in(IO_T *pio, uint8 state, uint32 timeout), which return the length of the pulse in microseconds.

##### ADC

Like for simple input/output, we need to initialize the pin for analog reading first. We use void **suli_analog_init(ANALOG_T * aio, PIN_T pin)** where ANALOG_T is the equivalent of IO_T. For the pin, choose between **A3=0, A4=1, A2=50, A1, TEMP, VOL**. You can now read with **int16 suli_analog_read(ANALOG_T *aio)**.

	ANALOG_T temp_pin;
	suli_analog_init(&temp_pin, TEMP);
	int16 temper = suli_analog_read(temp_pin);

##### Time functions

You have two fonctions in Suli to wait a certain amount of time : **void suli_delay_us(uint32 us); ** for microseconds waiting and **void suli_delay_ms(uint32 ms);** for milliseconds waiting. You can also know for how long the program have been running with : **uint32 suli_millis(void);** for milliseconds and **uint32 suli_micros(void);** for microseconds. Take care with overflow, after 50 days for suli_millis but after 70 minutes for suli_micros.

##### I2C

I2C works the same way than ADC and input/output. You have an init function **void suli_i2c_init(void * i2c_device);**, a write function **uint8 suli_i2c_write(void * i2c_device, uint8 dev_addr, uint8 *data, uint8 len);** and a read function **uint8 suli_i2c_read(void * i2c_device, uint8 dev_addr, uint8 *buff, uint8 len);**. For I2C, as you have no choice for the pin, specify the i2c_device is useless, just give NULL.

	suli_i2c_init(NULL);
	uint8 data = TRIGGER_HUMD_MEASURE_NOHOLD;
	suli_i2c_write(NULL, HTDU21D_ADDRESS, &data, 1);
	uint8 msb;
	suli_i2c_read(NULL, HTDU21D_ADDRESS, &msb, 1);

If you want a use example of I2C, go to **src/humidity.c**. This is a driver to read data from a humidity and temperature sensor.

##### UART

Like I2C, you have no choice for the UART Port, it has to be UART1. So, in the init function **void suli_uart_init(void * uart_device, int16 uart_num, uint32 baud);**, the is no need to specify uart_device and uart_num. The baud can be : 4800, 9600, 19200, 38400, 57600 or 115200. Note that by default, the baud rate is initialized to 115200, so there is no special need to initialize it.

For the writing part, you have a general function, **void suli_uart_send(void * uart_device, int16 uart_num, uint8 *data, uint16 len);** which just take an array of data and the length of this array. Then, you have more specific functions : void suli_uart_send_byte(void * uart_device, int16 uart_num, uint8 data);, void suli_uart_write_float(void *uart_device, int16 uart_num, float data, uint8 prec);, void suli_uart_write_int(void * uart_device, int16 uart_num, int32 num);. However, you surely want to use printf like function, much easier to use. So, I recommand you use **void suli_uart_printf(void *uart_device, int16 uart_num, const char *fmt, ...); ** (the three dots are a notation of C to say that the number of argument is undetermine, as it is in printf).

	suli_uart_printf(NULL, NULL, "<HeartBeat%d>\r\n", random());

For the reading part, you have a function to know if you have something to read : **uint16 suli_uart_readable(void *uart_device, int16 uart_num);** which returns one if uart has received readable data. Then you can read a byte with **uint8 suli_uart_read_byte(void *uart_device, int16 uart_num);**

That's it for Suli. There is nothing hard here and if we want more advanced functions, we will have to dig into API provided by NXP. However, **suli/suli.c** can be useful as you can see example of use of the API.

### API MODE

The API is a simple way to communicate with the MCU from outside. The messages have a particular form, which is explained in details in the [User's Manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_User_Manual_v0.3.pdf). It is also possible to call API commands from MCU mode.

#### API Frames

TODO, for now read the [User's Manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_User_Manual_v0.3.pdf).

#### Send API Commands from MCU Mode

To send an API Command, we need to build a **tsApiSpec**, which is a representation of an API Frame. Let's see what is inside :

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

Let's go through it. We do not care about **startDelimiter**, it is always 0x7e and will be set for us. Then comes the **length** of the payload, which is the useful data in the Frame. The **teApiIdentifier** is the id to identify a packet. I can be one of the following option :

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

Depending of what you are sending, you will have to choose the correct id. They are describe in the [User's Manual](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/MeshBee_User_Manual_v0.3.pdf). Now, you have a union, **payload**. In C, it simply means that you have to choose one of the following options. The _ _atttribute_ _ ((packed)) means that the compiler have to be all the fields together, without a hole, in the memory : they are "packed"". Notice that in the payload, you have for example **localAtReq** to do, as the name says, a local At request. If you want the exact definition of a payload, go to **include/firmware_at_api.h** Then, finally, you have a checksum to be a bit more sure that data are transmitted without problems (like a bit switch).

Most of the time, you do not have to complete the tsApiSpec yourself. Functions are provided to make it easier. It is also the case for **tsLocalAtResp** and **tsRemoteAtResp**.

For example, if we want to send data to another node in the network we can do it by sending a API command.

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

To fill tsApiSpec, we used, for data, **void PCK_vApiSpecDataFrame(tsApiSpec *apiSpec, uint8 frameId, uint8 option, void *data, int len);** in **firmware_api_pack.h**. The frameId just identify the frame, put whatever you want. The option 0 for UNICAST, 1 for BROADCAST. It is also important to create the actual frame inside an array with **int i32CopyApiSpec(tsApiSpec *spec, uint8 *dst);** also in **firmware_api_pack.h**. It returns the size of the frame, which is usefull when we we want to send our frame with **bool API_bSendToAirPort(uint16 txMode, uint16 unicastDest, uint8 *buf, int len);** in **firmware_at_api.h**. The txMode can be either UNICAST (to only one node) or BROADCAST (to all nodes). Then you have to specify an address which will be used if you are in UNICAST mode (otherwise it does not matter), the frame array you have just created and its length.

You can also call AT commands thanks to **int API_i32AtCmdProc(uint8 *buf, int len);** in **firmware_at_api.h**. Just give it your command and its length and it returns you if the function succeeded or not.

	char * aj = "ATAJ1";
	API_i32AtCmdProc(aj, 5);

You have other functions in firmware_at_api.h and firmware_api_pack.h but for now I do not find them useful. Explore the code if you have to go in more details.

### First Experiments

Here I will try to explain the first experiments I did when I received the MeshBees. I began with an simple hello word, with a LED.

#### Hello World !

The circuit is really simple : a Led and a resistor connected to D9.

![led](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/led_scheme.png)

The code we use is simple.

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

The measure the brightness, with use a photoresistor : the higher the brightness the lower the resitance. So, to measure it, with do a voltage divider with a resitor of 5k ohms.

![photoresistor](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/photores.png)

We connect the middle point with ADC3, i.e. D0. Here is the code, we read the brightness and send it to the coordinator :

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

Note that you could also read the internal temperature the same way : just replace the initialization of the pin by **suli_analog_init(&temp_pin, TEMP);**.

#### A driver for HTU21D

HTU21D is a humidity sensor which can also output a temperature. It is communicating by I2C with the MCU. You can find the code for it in **src/humidity.c**. You have three functions. The first one is **void init_humidity(void)**. It simply initialize everything to read the sensor, here it is only initializing I2C.

Then you have two functions. One to read humidity : **unsigned int read_humidity(void)**. It returns you a raw_humidity. To get the real one, just compute : 

	-6 + (125 * rawHumidity / (float)65536);

The function asks to read humidity. Then, it waits for the result to be avalible and read it. The message read is composed of three parts : the most and least significative bytes of the humidity and a checksum to be sure of what we read. Note that inside the humidity value, there are two status bits we need to erase.

The temperature reading works the same way, with **unsigned int read_temperature(void)**. It returns the raw temperature value, so to get the real one, you need to compute : 

	(float)(-46.85 + (175.72 * rawTemperature / (float)65536))

