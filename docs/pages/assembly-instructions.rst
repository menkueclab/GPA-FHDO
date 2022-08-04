Assembly Instructions
=====================

It is advised to assemble the GPA in functional groups and test every group before continuing.

1. Assemble the 5 V and 12 V LDOs plus their passive components first and test the result. 
    - Test: The device should draw less than 100 mA. Measure +12V, +5V and VCC. 
2. Assemble the 2.5 V reference voltage.
    - Test: Measure the 2.5V reference voltage on pin 3 of the SV2 connector.
3. Assemble the SPI opto-isolator and its passive components. Connect a SPI master to the device and check if the signals go through the opto-isolator.
    - Test: Connect a SPI master to the device via the RJ45 connector and check if the signals go through the opto-isolator.
4. Assemble the DAC and its passive components. It can be done with solderpaste and hot air. 
    - Test: There should be 2.5 V on pin 1 of the SV2 connector. Connect an Arduino running the supplied test code to the device. Select channel 1 by typin "c1" in the terminal / serial monitor. Set the output current to 0 A by typing "s0" in the terminal. There should be 2.5 V SV1 pin 8. Repeat the test for the other channels.
5. Assemble the ADC and its passive components. 
    - Test: The result can be tested with an Arduino running the supplied test code. Type "c1" in the terminal to select channel 1. Type in "d" in the terminal / serial monitor to enter read mode. The ADC value for channel 1 will be continously output on the terminal. Connect a know voltage to the input of ADC channel 1, for example the 2.5 V reference voltage. The ADC code should be approximately 0x8000 (at 2.5V).
6. Assemble the power output stages and run the calibration procedure.