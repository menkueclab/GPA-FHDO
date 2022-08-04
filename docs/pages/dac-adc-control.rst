Controlling the DAC and ADC via SPI
===================================
Arduino sample code in C++ is provided, which can do simple tasks like performing a calibration, doing a ramp 
for linearity measurements or setting the output current to a specific value.
We have tested the code on an Arduino MEGA. The program is controlled via Terminal with 115200 baud. 
The commands are as follows:

+----------+---------------------------------------------------------------------------------------------+
| command  | description                                                                                 |
+==========+=============================================================================================+
| f        | searches the DAC value that corresponds to 0A current on the active channel. Always run     |
|          | this command before doing a calibration.                                                    |
+----------+---------------------------------------------------------------------------------------------+
| g        | perform calibration on gain factors on the active channel.                                  | 
+----------+---------------------------------------------------------------------------------------------+
| r        | runs a ramp on the active channel. The ramp min (rampMinAmp) and max (rampMaxAmp) current   |
|          | to be set in the source code. If the ramp is going over high currents (i.e. > 3A), a duty   |
|          | cycle can be switched on, to prevent OP amp from overheating. The sleep time after each     |
|          | output can be defined in the source code (rampCyclePause).                                  |
+----------+---------------------------------------------------------------------------------------------+
| i        | sets output current for the active channel to 0A and goes into idle mode.                   |
+----------+---------------------------------------------------------------------------------------------+
| d        | reads current amplitude for the active channel and outputs it to terminal.                  |
+----------+---------------------------------------------------------------------------------------------+
| q        | outputs configuration on terminal.                                                          |
+----------+---------------------------------------------------------------------------------------------+
| p        | outputs current pulses for rise time measurements on the active channel. The high           |
|          | (pulseHighAmp) and low (pulseHighAmp) values of the pulse can be defined in the source code.|
|          | The ON time is 1/100 of a 100 ms cycle.                                                     |
+----------+---------------------------------------------------------------------------------------------+
| c[x]     | changes active channel to ‘x’. x can be a number between 1 and 4. Example “c4” sets the     |
|          | active channel to 4.                                                                        |
+----------+---------------------------------------------------------------------------------------------+
| s[x]     | sets the output current of the active channel to ‘x’. x can be only a single digit signed   |
|          | integer number. Example “s-2” sets the active channel current to -2A.                       |
+----------+---------------------------------------------------------------------------------------------+

