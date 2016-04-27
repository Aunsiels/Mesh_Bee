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

![MeshBee pins](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/600px-Mesh_Bee_Pin.jpg)

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

Let's try to connect an other node to the network. Do not forget that you **always** need the coordinator to be on. Unplug it from the programmer and give it power, for example with the raspberry pi (see below). If you want to be sure that it is connected, you can also connect a LED (and a resistor !) to the ASSOC pin (you have the names of the pins below the MeshBee or see the picture above).