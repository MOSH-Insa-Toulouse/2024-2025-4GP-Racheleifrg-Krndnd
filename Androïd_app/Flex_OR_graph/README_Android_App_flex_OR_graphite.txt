This Android app receives data from either the flex sensor or the graphite sensor and displays the resistance values of the selected sensor. 
On the main interface, a first dropdown menu allows the user to select the potentiometer value that was previously chosen on the OLED screen using the rotary encoder.
A second dropdown menu allows the user to select which sensor's resistance will be plotted, based on with the choice made in the Arduino code.
The user can start and stop data acquisition at any time.
If you want to plot both the flex and graphite sensor resistances at the same time, a separate version of the app (along with its corresponding Arduino code) is available. 
However, in this version, the precision of the resistance values is reduced due to the use of the least significant bit to distinguish between the two different data types.