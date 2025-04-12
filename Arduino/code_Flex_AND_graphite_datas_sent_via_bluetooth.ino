/*  
Complete code for the course: "Projet capteur"
Students: Karine Dandy, Rachele Iffrig
4th year/Genie Physique

Project's description:  
This Arduino code reads the resistance of an industrial strain gauge sensor as well 
as a graphite-based sensor fabricated following the paper: "Pencil Drawn Gauges and Chemiresistors"
by Cheng-Wei et al.(2014). The resistance values from both sensors are accessible from one of the
several menus displayed on an Oled screen.To go through these menus (flex sensor datas/Graphite sensor datas
/select one of the four potentiometre values proposed) a rotary encoder is used. Its switch allows te user to confirm
their selection.
The acquired data from both sensors are sent via bluetooth by an HC-05 module, and received by an application created using MIT App Inventor.
*/



// ----- The libraries used:--------------------------------------------------------------------------------------------------------------------------
#include <SoftwareSerial.h>//serial bluetooth
#include <Adafruit_SSD1306.h> // oled
#include <SPI.h> // potentiometre

//Tests to see if the code works -----------------------------------------------------------------------------------------------------------------------

//#define DEBUG_pot
#define DEBUG_sensors_graph
//#define DEBUG_sensors_flex
//#define DEB_POT

// ----- important numbers:----------------------------------------------------------------------------------------------------------------
#define baudrate 9600 // Baud rate: the same everywhere
int counter=0;//Count in which loop we are in order to know if it is an even or odd number

// Bluetooth: need to define the pins for pin mode in the setup-----------------------------------------------------------------------------------------
#define rxPin 7 // Bluetooth reception (TXD Bluetooth) on Arduino pin 7 (Ours: 6)
#define txPin 8 // Bluetooth transmission (RXD Bluetooth) on Arduino pin 8 (Ours: 7)

SoftwareSerial bluetooth_Serial(rxPin ,txPin); // Software serial to send/receive information



// Rotary encodeur----------------------------------------------------------------------------------------------------------------------------------------
#define encoder0PinA  2  //CLK Output A Do not use other pin for clock as we are using interrupt
#define encoder0PinB  4  //DT Output B
#define Switch 5 // Switch connection if available
// To avoid the mechanical rebound effects of the rotary potentiometer, a software filter is used. 
volatile unsigned long lastinterrupttime=0;
const unsigned long debouncetime=60;
bool button_on=0; // state of the switch
static bool lastStateA;
bool stateA;
bool stateB;
volatile unsigned int encoderPos=0;// store if the position of the encodeur increase or decrease

// Potentiometre-----------------------------------------------------------------------------------------------------------------------------------------
//opcodes --> see datasheet MCP4150
#define MCP_NOP 0b00000000 //no operations
#define MCP_WRITE 0b00010001 //send a resitance value between 0 and 255
#define MCP_SHTDWN 0b00100001// Set to high impedance state
const int ssMCPin = 10; // Define the slave select for the digital pot (pin cs)
volatile float Rpotentiometre=1000; // Value we want to assign to the potentiometer, 1kohms by default



//flex sensor-------------------------------------------------------------------------------------------------------------------------------------------
const int flexPin = A1;      // Pin connected to voltage divider output
//What we want to calcultate:
float angle=0.0; //sensor's flexion 
float Rflex=0.0; //flex's resistance
byte ADC_flex_bluetooth=0; //information sent to bluetooth

//graphen sensor----------------------------------------------------------------------------------------------------------------------------------------
float Rgraph_sensor=0.0; // Calculated from the voltage value retrieved by the Arduino
int V_graphsensor=0.0;//given by Arduino
byte Vgraphsensor_bluetooth=0; //information sent to bluetooth

//Oled ------------------------------------------------------------------------------------------------------------------------------------------------
#define nombreDePixelsEnLargeur 128         // OLED screen size, in pixels, at its width
#define nombreDePixelsEnHauteur 64          // OLED screen size, in pixels, at its height
#define brocheResetOLED         -1          //Reset of the OLED shared with the Arduino (hence the value of -1, and not a pin number)
#define adresseI2CecranOLED     0x3C        // Address of 'my' OLED screen on the I2C bus (usually equal to 0x3C or 0x3D)
#define taillecaractere 1  // Character size on the OLED screen
Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED); // initialise oled

//Menu Oled
const char *menus[][6] = {
    {"Potentiometre", "Flex Sensor", "Graph Sensor","Menu Principal"},
    {"1 Kohms", "2 Kohms", "3 Kohms", "4 Kohms", "Retour","Potentiometre"},
    {"Resistance", "Angle", "Retour","Flex Sensor"},
    {"V graph", "R graph", "Retour","Graph Sensor"}};
const uint8_t menuSizes[] = {3, 5, 3, 3};// Titles at the end are not counted as part of the menu size
uint8_t menuIndex = 0, menuSelection = 0;


// ----- Function declarations--------------------------------------------------------------------------------------------------------------------------

//encodeur
void doEncoder(); // Detects a movement of the rotary encoder and its direction and update the position
void updateEncodeur(); // Update the index according to the menu we are in
void switch_button();//Detects whether the switch has been pressed or not

//bluetooth
void send_bluetooth(float data);// send a byte to the bluetooth

//menu ecran oled
void display_menu();//Displays the menus on the OLED, allows reading sensors values, and selecting potentiometer values  

// Graphene sensor:
void graphen_mesures(); // retrieve the voltage value, calculate and send resistance value to oled + convert and stock ADC value for bluetooth
//flex sensor
void flex_measure(); // retrieve the voltage value, calculate and send resistance + angle values to oled + convert and stock ADC value for bluetooth
//potentiometre
void SPIWrite(uint8_t cmd, uint8_t data, uint8_t ssPin);//sends a command and data to the potentiometre via a slave pin

//----------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------

void setup() { 
  // Initialize the pins and specify their functions, as well as configure the serial communication for data transfer

  //-----Bluetooth-----
  pinMode(rxPin,INPUT);
  pinMode(txPin,OUTPUT);
  bluetooth_Serial.begin(baudrate);

  //----Graphene sensor----
  // Analog pins, so input by default

  //---flex sensor-----
  pinMode(flexPin, INPUT);


  //------Initialize oled------ cf tds
  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED))
   while(1);                                                // Program stop (infinite loop) if initialization fails
  ecranOLED.clearDisplay();                               // Clear the entire buffer
  ecranOLED.setTextSize(taillecaractere );               // Character size
  ecranOLED.setCursor(0,0);                             // Move the cursor to position (0,0)=>the top-left corner
  ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);// Text color and background color
  //------encodeur rotatoir---
  pinMode(encoder0PinA, INPUT_PULLUP); 
  pinMode(encoder0PinB, INPUT_PULLUP); 
  pinMode(Switch,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder, CHANGE); // Calls the doencoder function every time a change is detected on pin A

  //-------Potentiometre------
  pinMode (ssMCPin, OUTPUT); //select pin output
  digitalWrite(ssMCPin, HIGH); //SPI chip disabled
  SPI.begin(); 

  Serial.begin (baudrate);
  

  
} 

//----------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------


void loop(){
  counter++;
  // The Bluetooth receives only one byte at a time; we send a different data based on the parity of the counter(ADC flex or graphite sensor):
  (counter % 2 == 1)?send_bluetooth(ADC_flex_bluetooth):send_bluetooth(Vgraphsensor_bluetooth);
  flex_measure();
  graphen_mesures();
  updateEncodeur();
  delay(100); //Best delay for the switch
  
  //Serial.println("test");
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------

// Detects a movement of the rotary encoder and its direction and update the position
void doEncoder() {
  
    stateA = digitalRead(encoder0PinA);//indicates whether there has been a change in state
    stateB = digitalRead(encoder0PinB);//To determine the direction of rotation
    unsigned long interrupttime=millis();
    if(interrupttime - lastinterrupttime > debouncetime ){//Checks that it is not a bounce
      if (lastStateA != stateA) {  // Change detected
        if (stateB != stateA) {
            encoderPos++;  // Clockwise direction
        } else {
            encoderPos--;  //Counterclockwise direction
        }
        
        lastinterrupttime=interrupttime;//as it is not a rebound
    }

    lastStateA = stateA;//Update the state of the clock
    
    }

    
}

// Update the index according to the menu we are in

void updateEncodeur(){
      
  if (menuSelection ==0){ //Main menu
  // We convert the position to an index based on the menu size, and turn negative numbers into positive ones
    menuIndex = (encoderPos % menuSizes[0] + menuSizes[0]) % menuSizes[0];
    
  } else if(menuSelection ==1){//Potentiometre menu
    menuIndex=(encoderPos % menuSizes[1] + menuSizes[1])% menuSizes[1];
     }else if(menuSelection ==2){//flex menu
    menuIndex=(encoderPos % menuSizes[2] + menuSizes[2])% menuSizes[2];
    }else if(menuSelection ==3){//graphene menu
    menuIndex=(encoderPos % menuSizes[3] + menuSizes[3] ) % menuSizes[3];
      

  }
  display_menu();//Once we have our positions in the menus, we call the display function for the OLED
  

}

//Detects whether the switch has been pressed or not
void switch_button() {
  static bool lastState = HIGH;//main state of the switch
  bool currentState = digitalRead(Switch);
  if (lastState == HIGH && currentState == LOW) {  // Detection of a button press (falling edge)
    button_on = true;
  } else {
    button_on = false;
  }}


//Displays the menus on the OLED, allows reading sensors values, and selecting potentiometer values 

void display_menu(){
  // Clear the screen at each loop to reset the 'cursor'
    ecranOLED.clearDisplay();
    int memoire=0;//Remember the selected option

   ecranOLED.setCursor(0, 0);  // Cursor position for the first line to write at the correct location
   switch_button();//switch button to change menu

   //Main MENU---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   if (menuSelection ==0){ 

    // TITLE: it cannot be selected
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // text color and background color
    ecranOLED.println(menus[menuSelection][3]);
    
    //Manage the options that can be selected
   for (uint8_t i = 0; i < menuSizes[menuSelection]; i++) {
        // Use of ternary operators because they take up less memory space
        //we change the text and background color of the selected option
        ecranOLED.setTextColor(i == menuIndex ? SSD1306_BLACK : SSD1306_WHITE, i == menuIndex ? SSD1306_WHITE : SSD1306_BLACK);
        ecranOLED.println(menus[menuSelection][i]);
        memoire = (i == menuIndex) ? i : memoire;//memorise the selected option      
        if(button_on){
      menuSelection=memoire+1; //We go to the selected menu. Caution:index pot=0, but the menu selection of the potentiometre is 1=>shift by 1
      encoderPos=0;
      }}}

      //MENU POTENTIOMETRE----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
      else if(menuSelection ==1){
        // TITLE: it cannot be selected
        ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK); 
        ecranOLED.println(menus[menuSelection][5]);

       //Manage the options that can be selected
       for (uint8_t j = 0; j < menuSizes[menuSelection]; j++) {
       
        switch_button();
        if (menuIndex == (menuSizes[menuSelection] - 1) && button_on) {
          // If I'm on the return menu and the switch is pressed, I go back to the main menu
         menuSelection = 0;
         encoderPos = 0;
         }else {        //we change the text and background color of the selected option

             ecranOLED.setTextColor(j == menuIndex ? SSD1306_BLACK : SSD1306_WHITE, 
                                    j == menuIndex ? SSD1306_WHITE : SSD1306_BLACK);
            
            if(j == menuIndex && button_on){
              //Change the value of the potentiometer to the selected value
              Rpotentiometre=(j+1)*1000;//update display on Oled
              //send the right data to the potentiometre=>see the datasheet of MCP4150
              uint8_t data = (j + 1) * 5;
              SPIWrite(MCP_WRITE,data,ssMCPin);
              

              }
            
            
        }
        ecranOLED.println(menus[menuSelection][j]);//display the menu on the Oled
        #ifdef DEBUG_pot
         Serial.print(F("Rpot="));
         Serial.print(Rpotentiometre); 
         Serial.println(F(" Volts"));
        #endif

        
    }
    //Show the current value of the potentiometre
    ecranOLED.setTextColor(SSD1306_WHITE,SSD1306_BLACK );
    ecranOLED.print(F("Rval= "));
            ecranOLED.print(Rpotentiometre);
            ecranOLED.println(F("ohms"));

    

    }else if(menuSelection ==2){ //MENU FLEX----------------------------------------------------------------------------------------------------------------------------------------------

    // TITLE: it cannot be selected
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK); 
    ecranOLED.println(menus[menuSelection][3]);
    
    for (uint8_t k = 0; k < menuSizes[menuSelection]; k++) {        //we change the text and background color of the selected option

        
            ecranOLED.setTextColor(k == menuIndex ? SSD1306_BLACK : SSD1306_WHITE, 
                                   k == menuIndex ? SSD1306_WHITE : SSD1306_BLACK);
            ecranOLED.println(menus[menuSelection][k]); // display the menu
            memoire =( menuIndex==k) ? k : memoire;//We keep the information of where the cursor is stored in memory
            button_on=(menuIndex <menuSizes[menuSelection] - 2)?0:button_on;//To ensure that these menus cannot be selected 
        
    }
    //We display the datas based on the cursor position
    if (memoire== 0) {
            ecranOLED.println(F(""));
            ecranOLED.print(F("Rflex= "));
            ecranOLED.print(Rflex);
            ecranOLED.println(F("ohms"));
            
        } else if (memoire == 1) {
          ecranOLED.println(F(""));
          ecranOLED.print(F("Angle="));
          ecranOLED.print(angle);
          ecranOLED.println(F("degres"));
          
        }

    // Button handling and return to the main menu
    if (menuIndex == 2 && button_on) {
        menuSelection = 0;
        encoderPos = 0;
    }
    

    }else if(menuSelection == 3){ //MENU graphene--------------------------------------------------------------------------------------------------------------------------------------
      // TITLE: it cannot be selected
      int Vgraphen_volts=map(V_graphsensor,0,1024,0,5000);// To be able to display a value in mV on the OLED that makes sense
      ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK); 
      ecranOLED.println(menus[menuSelection][3]);



      for (uint8_t l = 0; l < menuSizes[menuSelection]; l++) {        //we change the text and background color of the selected option

        
            ecranOLED.setTextColor(l == menuIndex ? SSD1306_BLACK : SSD1306_WHITE, 
                                   l == menuIndex ? SSD1306_WHITE : SSD1306_BLACK);
            ecranOLED.println(menus[menuSelection][l]);
            button_on=(menuIndex <menuSizes[menuSelection] - 2)?0:button_on;//To ensure that these menus cannot be selected 
            
            
            memoire =( menuIndex==l) ? l : memoire;//memorise the selected option
              
            }
            //We display the datas based on the cursor position
            if (memoire== 0) {
            ecranOLED.println("");
            ecranOLED.println(F("Tension graph="));
            ecranOLED.print(Vgraphen_volts); 
            ecranOLED.println(F(" mV"));
            
        } else if (memoire == 1) {
          ecranOLED.println(F(""));
          ecranOLED.println(F("Rgraphene="));
          ecranOLED.print(Rgraph_sensor);
          ecranOLED.println(F(" ohms"));
        }
    // return to the main menu if the switch is on
    if (menuIndex == 2 && button_on) {
        menuSelection = 0;
        encoderPos = 0;
    }  }
  ecranOLED.display();
}



// Graphene Sensor: Read and send to bluetooth the tension value
void graphen_mesures(){
// We retrieve the voltage values from the sensor after the amplifier 
// We define variables locally, as it takes up less memory space than global definitions
  const float Vcc=5.0;
  const float R5=10000;
  const float R1=100000;
  const float R3=100000;
  
  V_graphsensor=analogRead(A0);
  int V= map(V_graphsensor,0,1024,0,5000); // To convert to 0-5000mV, it's necessary; if we do it directly in volts, 0 => r = infinity
  Rgraph_sensor=abs(((R1*(1+R3/Rpotentiometre)*(Vcc/(V*0.001))-R1-R5)));
  Vgraphsensor_bluetooth=(map(V_graphsensor,0,1024,0,127)<<1)|0x00;//The voltage information is stored in a byte 
  //Serial.print(F("Vgraph_sensor=" ) + String(V_graphsensor) + "Volts"); -->use too much sapce in RAM
// The 'F' allows storing in flash memory instead of RAM  #ifdef DEBUG_sensors_graph
 #ifdef DEBUG_sensors_graph
 Serial.println(V_graphsensor);
  Serial.print(F("ADC_graph=")); 
  Serial.print(Vgraphsensor_bluetooth); 
  Serial.println(F("/255"));
  Serial.print(F("Rgraph_r=")); 
  Serial.print(Rgraph_sensor); 
  Serial.println(F(" ohms"));

  #endif
  
 
}

// send a byte to the bluetooth
void send_bluetooth(byte data){
  
  bluetooth_Serial.write(data);
}

// retrieve the voltage value, calculate and send resistance + angle values to oled + convert and stock ADC value for bluetooth
void flex_measure() {
  
 
  // We define variables locally, as it takes up less memory space than global definitions
  const float VCC = 5;      // voltage at Ardunio 5V line
  const float R_DIV = 47000.0;  // resistor used to create a voltage divider
  const float flatResistance = 25000.0; // resistance when flat
  const float bendResistance = 100000.0;  // resistance at 90 deg
  // Read the ADC, and calculate voltage and resistance from it
  int ADCflex = analogRead(flexPin);
  float Vflex = ADCflex * VCC / 1023.0;
  Rflex = R_DIV * (VCC / Vflex - 1.0);
  ADC_flex_bluetooth=(map(ADCflex,0,1024,0,127)<<1)|0x01;
  // Use the calculated resistance to estimate the sensor's bend angle:
  angle = map(Rflex, flatResistance, bendResistance, 0, 90.0);

  #ifdef DEBUG_sensors_flex
  
  Serial.print(F(" Rflex: ")); 
  Serial.print(Rflex); 
  Serial.println(F(" ohms"));

  /*Serial.print(F("Bend: ")); 
  Serial.print(angle); 
  Serial.println(F(" degrees"));*/
  #endif

  #ifdef DEB_POT
   Serial.print(F("Vflex ")); 
   Serial.print(Vflex); 
   Serial.println(F(" V")); 
   Serial.print(F("ADCflex ")); 
   Serial.print(ADC_flex_bluetooth); 
   Serial.println(F(" ADC")); 
  #endif

}


//sends a command and data to the potentiometre via a slave pin
void SPIWrite(uint8_t cmd, uint8_t data, uint8_t ssPin) // SPI write the command and data to the MCP IC connected to the ssPin
{
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0)); //https://www.arduino.cc/en/Reference/SPISettings
  
  digitalWrite(ssPin, LOW); // SS pin low to select chip
  
  SPI.transfer(cmd);        // Send command code
  SPI.transfer(data);       // Send associated value
  
  digitalWrite(ssPin, HIGH);// SS pin high to de-select chip
  SPI.endTransaction();
}





 