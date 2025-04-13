# 2024-2025-4GP-Rachele Iffrig, Karine Dandy

# Graphite based sensor project

**Students**: Rachele Iffrig, Karine Dandy

Département Génie Physique

INSA Toulouse

## Project Description

The aim of this project is to determine the main characteristics of a low-tech constraint sensor, as introduced in the paper “Pencil Drawn Strain Gauges and Chemiresistors on Paper”  by Cheng-Wei Lin, Zhibo Zhao, Jaemyung Kim & Jiaxing Huang, published on Nature in 2014, and to compare them with those of an industrial flex sensor. To do so, both sensors are managed by an Arduino Uno board. On this board we also have an OLED screen in order to display the datas, and we navigate through the menus using a rotary encoder. The data are sent to an Android application via a Bluetooth® module.<br> <br>
You can recreate this project from scratch, as we have provided everything — from the PCB design using KiCad, the Arduino code, and the Android app, to the test bench setup used to compare both sensors and create the graphite sensor’s datasheet. 

## Content

In this github repository you wil find independent folders for each part of our project.

1) **Shield and Kicad**
2) **Arduino code**: The Arduino folder contains 2 different codes: 
     *code_Felex_OR_graphite_datas_sent_via_bluetooth.ino* and *code_Flex_AND_graphite_datas_sent_via_bluetooth*. Both codes calculate the resistance of the industrial strain gauge sensor as well the graphite-based one, and are using the rotary encoder to navigate through the different menus displayed on the Oled screen. The difference between the two codes comes with what datas we are sending via bluetooth. In the first code, at the beginning, the user chooses which sensor’s information they want to plot in the app. The second code sends data from both sensors simultaneously: both resistances are plotted, but with less precision. 
   
4) **Android App**: The Androïd App folder contains all the informations concerning the two apps, made with MIT App Inventor, that are receiving all the datas sent by bluetooth. With these apps, compatible with Android, you can either plot the resistances of both the flex and graphite sensors simultaneously, or choose to plot only one of them, depending on the code you chose.      
5) **Datasheet**
6) **Bench test**
7) **Final presentation**: The Final Presentation folder contains a PowerPoint in which we explain the differents steps of our projects, the difficulties we faced and the results we obtained.
 
## Physic behind the low tech strain gauges


## Necessary components to carry out this project

* 1 graphite-based paper sensor
* 1 flex sensor
* 1 LTC1050 operational amplifier
* 1 MCP41050 digital potentiometer
* 1 KY-040 rotary encoder
* 1 Arduino UNO
* 1 OLED screen
* 1 HC-05 Bluetooth® module
* Resistances
* Capacitors


## How to use






