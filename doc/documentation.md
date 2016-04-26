#Documentation of MeshBee Project by Julien Romero

This document will present and explain my work during my semester project. The goal is to build a wireless network of sensors for performance analysis. I began the project in February 2016. I hope that thanks to this document you will be able to understand what I did, why I did it and to continue my work.

## Why MeshBee ?

### Choosing a Wireless Protocole

The first thing I had to think about was how to communicate. There are plenty of availible protocols and I have to choose among them the one which will best fit to my needs. I considered eight protocols : ZigBee, Bluetooth, BLE, Rubee, Zwave, ANT/ANT+, EnOcean and WiFi. To evaluate them, I chose four caracteristics : the data rate, the size,  the consumption and the range. Of course, this properties depends of a lot of parameters : hardware, environment,... So, I tried to choose values which appear in most of the description. The use cases are also useful to know what kind of things are possible with a protocol.

#### Zigbee

Zigbee is based on 802.15.4 specification, at 2.6 GHz. It is used in smart home, industrial control, medical tools, fire sensors... It is also used for low energy sensors, which is useful for us. A Ad Hoc network is created around a coordinator. Different architecture of the network are possible, we shall see it later. However, there can be problems with inferences as 2.4GHz is often used.

#### Bluetooth

Bluetooth is based on IEEE 802.15.1 and works at 2.6GHz. It is used in smartphones, keyboards, mice, sensors,... The fact that it is in all smartphone popularized in. The consumption is medium.

#### BLE

The Bluetooth Low Energy, just like Bluetooth, works at 2.4GHz. We can see it as a low consumption version of Bluetooth, which exists to answer modern problems like consumption. So, it is used in healthcare, sport, sensors...

#### Rubee

Rubee is a 450kHz protocol designed to transmit information in harsh environment (with a lot of steel for example), with high security. So, it is often used for military applications. It works with tags, a bit like NFC but the range can be higher. However, it is not that easy that find chip for Rubee development.

#### Zwave

Zwave is a protocol around 900MHz, designed for home automation applications. It can be integrated to a lot of existing of house products. It has a small consumption and a lower data rate than Zigbee.

#### ANT/ANT+

ANT/ANT+ works on 2.4GHz. It is a very low consumption protocol. It is mainly used in health, sport, smart home and industry.

#### EnOcean

EnOcean works on 902 MHz, 928.35 MHz, 868.3 MHz and 315 MHz and is a very low energy protocol (can also work without battery, inside a switch for example). The data rate is low. It can be used in smart house and in sensors.

#### WiFi

WiFi is a very popular protocol in personal application and is based on IEEE 802.11 . It operates on 2.4, 3.6, 5, and 60 GHz frequency bands. The speeds that can be acheive are high but the consumptions are also high. It is also used for the internet of things and a lot of ship are now design with wifi integrated (Rapsberry Pi 3, Arduino Genuino MKR1000, Red Pitaya, Electric Imp...).

#### Putting Everything Together

I summarized everything in an [odt file](https://github.com/Aunsiels/Mesh_Bee/blob/master/doc/semester_project_protocols.ods). The results are on sheet 1 and 2. I tried to plot some results.

![conso-vs-data-rate](https://raw.githubusercontent.com/Aunsiels/Mesh_Bee/master/doc/conso_vs_data_rate.png) 

We can see that each protocole are in different areas and so, we need to choose the one which will be good for us. For that, we have to design a cost function according to our need. My function has four parameters (for the four parameters) that can be tuned. I also considered the chips availible (some are hard it get), how easy it is to program it, to get help, and the prices. Finally I decided to go for **Zigbee**

### Choosing a Zigbee Chip

There are plenty of Zigbee chips availible. What we would like to do is a chip which control both Zigbee and a sensor (it does not require power). So, we need to be able to program the prototyping chip directly, without having to use an additional board like an arduino. We also need to be able to read sensors thanks to i2c and ADC. We found a nice design, based on a NXP chip : MeshBee. It is open source (both software and hardware) and easy to prototype with. Thus, thanks to it, we can first try to write a program to test if our prototype is working and then design a new chip in which sensors are directly integrated. That will make the final result smaller.

## The System Architecture

For now, the system architecture is composed of three parts :

* The MeshBee network
* An interface between MeshBee and a server
* A Server which collects information and display it for the user

## The Embedded Software

### The MeshBee Framework


## The Server