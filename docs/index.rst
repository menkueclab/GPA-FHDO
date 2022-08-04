.. GPA-FHDO documentation master file, created by
   sphinx-quickstart on Sat May  7 17:26:42 2022.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to GPA-FHDO's documentation!
====================================

.. image:: images/GPA_PCB_1.2.jpg

This Gradient Power Amplifier (GPA) has 4 channels, of which each can drive a current of +- 10 A. The current is controlled 
via an SPI interface. The GPA has a build in current sensor for each channel that is also controlled via SPI. The information 
from the current sensor can be used for automatic calibration of the GPA.

.. warning:: **Disclaimer:**
   Not intended for medical use!

|

Contents:
---------
.. toctree::
   :maxdepth: 2
   
   pages/general-overview
   pages/function-overview
   pages/connections
   pages/dac-adc-control
   pages/adapting-resistor-capacitor
   pages/optimal-power-supply
   pages/automatic-calibration
   pages/measured-performance
   pages/maximum-ratings
   pages/bom
   pages/assembly-instructions

|

Contributing
------------
* Jason Stockmann
* Nicolas Arango
* Odin Holmes


References
----------
* https://tabletop.martinos.org/index.php/Hardware:GPA
* https://rflab.martinos.org/index.php/Current_driver:Current_driver


Source Code
-----------
* https://github.com/menkueclab/GPA-FHDO


Indices and tables
------------------

* :ref:`genindex`
* :ref:`search`
