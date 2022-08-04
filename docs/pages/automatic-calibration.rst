Automatic calibration using the Arduino code
============================================
1. Connect the GPA to power supply, Arduino and gradient coil (or dummy load)
2. Programm the Arduino with the supplied testcode using the Arduino software environmet (https://www.arduino.cc/)
3. Open a terminal programm (i.e. putty) and connect to the COM-Port of the Arduino with 115200 baud
4. press 'q' to get an info
5. press 'f' to find the DAC values for zero amp current
6. press 'g' to perform the calibration

---------------------------------------
Details about the calibration procedure
---------------------------------------
Calibration is based on measuring the ratio of DAC value to output current for different points. The measurement points are 
evenly distributed over the whole DAC input range (0x0000 - 0xFFFF). The default setting in the testcode uses 33 measurement 
points (0x0000, 0x0500, 0x1000, ...). The measured ratios are stored in the eeprom of the Arduino. However it is recommended 
to repeat the calibration after every power on, since it is dependent on temperature and gradient coil.
If the GPA is set to a specific output voltage, first order lagrangian polynomials are used for interpolation 
(linear interpolation). Second order interpolation would probably further improve linearity - however since linearity is 
already very good, this has not been implemented yet. 

The 33-point calibration for one channel takes currently 3.3ms, because there is a 100 us delay for current settling before 
reading the current from the ADC. A 4-channel calibration therefore takes around 13 ms. Since the calibration is super fast, 
it can be easily repeated withing imaging sequences to compensate temperature dependant nonlinearities and offsets. 

Theoretically the DAC value 0x8000 should correspond to 0A output current. However, due to tolerances of the used compents, 
this is not 100% true. There is a routine in the testcode (command 'f') that finds the DAC value that corresponds to 0A output 
current. This value will be used in the calibration instead of the 0x8000 base point. The rational behind this is, that the gpa 
should output as little as possible current if its set to 0A.