# 2024-2025-4GP-Racheleifrg-Krndnd

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
   
3) **Android App**: The Androïd App folder contains all the informations concerning the two apps, made with MIT App Inventor, that are receiving all the datas sent by bluetooth. With these apps, compatible with Android, you can either plot the resistances of both the flex and graphite sensors simultaneously, or choose to plot only one of them, depending on the code you chose.      
4) **Datasheet**
5) **Test Bench**
6) **Final Presentation**: The Final Presentation folder contains a PowerPoint in which we explain the differents steps of our projects, the difficulties we faced and the results we obtained.
 
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
* Resistances: 1kΩ, 10kΩ, 100kΩ, 100kΩ
* Capacitors: 100nF, 100nF, 1µF

<figure>
  <img src="pictures/complete_project.jpg" alt="complete_project" width="450"/>
  <figcaption>Figure 1: Image of the complete project.</figcaption>
</figure>


## How to use

### Project Assembly

1) Create the PCB using the file [mettre le nom] found in the **KiCad** folder. NB: you can also use a breadboard and follow the schematic from the following file:[mettre le nom]
2) Solder the components following the PCB and schematic that can be found in the same **KiCad** folder.
3) Connect the PCB to the Arduino, and then connect the Arduino to a PC via USB.
4) Choose one of the two codes from the **Arduino** folder based on your needs.      
5) Once the program is open in the Arduino IDE, select **Arduino Uno** as the board.
6) Run the code
7) If you chose: *code_Felex_OR_graphite_datas_sent_via_bluetooth.ino* select the datas you want to plot on the app, following the instruction that appear on the Serial.


### Oled's Menus
When you upload the code to the Arduino, the OLED screen lights up. On the OLED display, you can access **three different menus**:

1. **Potentiometer Selection Menu**  
   Allows you to choose between **four different potentiometer values**.

2. **Flex Sensor Data Menu**  
   Displays real-time data from the **flex sensor**.

3. **Graphite Sensor Data Menu**  
   Displays real-time data from the **graphite sensor**.<br> <br>
To switch between menus, press the rotary encoder.Inside each menu, rotate the encoder to scroll through the available options. To select an option (if applicable), press the rotary encoder's switch.

### Android App
1. **Pair your smartphone with the HC-05 Bluetooth module.** If a code is requested, enter **0000** or **1234**.

2. **Download the `.exe` file to your smartphone** by scanning the **QR code** found in the Android App` folder.

3. **Open the application**
4. **Grant the necessary permissions** to the application so it can detect Bluetooth devices connected to your Android.This can be done via your phone’s **Bluetooth or app settings**.

5.  **Click on the Bluetooth button**.

6. **Select the HC-05 module** from the list of available devices.

7. **Select the potentiometer value** that you previously chose using the **OLED menu** on the Arduino.

8. **If you are using the Flex or Graph app**,select the resistance value you chose at the beginning of the program from the **drop-down menu**.

9. **Start data acquisition** by pressing the **Start** button.

10. **Stop data acquisition** by pressing the **Stop** button.

<figure>
  <img src="Androïd_app/Flex_OR_graph/on_the_phone/step1.jpg" alt="app" width="200"/>
  <figcaption>Figure 2: Image of screen when we open the app.</figcaption>
</figure>

### Test Bench



