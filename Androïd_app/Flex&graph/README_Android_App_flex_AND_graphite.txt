This Android app receives data from both the flex sensor and the graphite sensors, and displays the resistance values of each sensor on the phone screen. On the main interface, a dropdown menu allows the user to select the potentiometer value that was previously chosen on the OLED screen using the rotary encoder.

The user can start and stop data acquisition at any time. To differentiate between the two types of sensor data, the app uses low bit as an identifier. However, this method reduces the precision of the resistance values.

For more accurate resistance readings, a separate version of the app (along with its corresponding Arduino code) is available, which collects data from only one sensor at a time.