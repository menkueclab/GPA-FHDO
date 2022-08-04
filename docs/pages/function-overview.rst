Function Overview
=================

The 16-bit DAC80504 DAC controls a differential push-pull Howland current source. The Howland current 
source uses a 200 milliohms shunt resistor, which is also used to measure the current. The voltage across 
the shunt resistor is amplified by an INA149 OP amp, which is connected to a 2.5V offset voltage, to enable 
measuring bidirectional currents. The output of the INA149 is connected to an ADS8684 ADC. 
With the comparison between the set current by the DAC and the measured current by the ADC an automatic calibration is possible.

To write and read the values from the DAC and ADC, the GPA can be connected to a microcontroller via SPI. 
The SPI interface is galvanically isolated achieved by the digital isolator ADUM4150 from Analog Devices. 
The calibration is performed by setting the DAC to evenly distributed values over the whole range. The 
output current is then measured for each of these values. To output a user specific current, first order 
interpolation from the recorded values is done, to obtain the DAC value for the specific current.
