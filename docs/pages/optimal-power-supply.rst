Choosing the Optimal Power Supply 
=================================
The voltage of the power supply should be selected to be good compromise between heat dissipation on the one side 
and rise time and maximum reachable current on the other side.  
Below are some example calculations given that can be adapted to your concrete situation.  

--------------------------------------------
Necessary Voltage for a certain Coil Current
--------------------------------------------  
The voltage drop across the coil can be easily calculated using Ohm's law. However, since the OPA549 need around 
5V head room to VCC and GND, another 10V should be added to the required voltage. SPICE simulations have shown, 
that 5V headroom is not always sufficient to reach maximum current, therefore another 5V should be added as head 
room marging.  
Example: 

.. math:: 
    R_{coil} = 500m\Omega \\
    I_{max} = 10A \\
    \Rightarrow \\
    U_{vcc} = U_{coil} + U_{head room} + U_{head room margin} \\
    = 5V + 10V + 5V = 20V

----------------
Heat Dissipation
---------------- 
The heat dissipation is closely related to the :math:`U_{vss}` voltage. Since the OPA549 are resistive regulators, 
the entire voltage drop that is necessary to regulate the current will generate heat :math:`P_{diss} = U_{opa} * I`.  
Let us examine one scenario with :math:`I = 1A` and another scenario with :math:`I = 10A`. The supply voltage is 20 V in both 
cases.  

If :math:`I = 1A`, the voltage drop across the coil is :math:`U_{coil} = 1A * 0.5\Omega = 0.5V`. Let us assume the push pull 
configuration of OPA549 positions the coil potential in the middle between :math:`U_{vcc}` and GND. The voltage drop across 
every OPA549 would then be :math:`\frac{20 - 1}{2} = 9.5V`, which causes a heat dissipation of :math:`9.5V * 1A = 9.5W`.  

If :math:`I = 10A`, the voltage drop across the coil is :math:`U_{coil} = 10A * 0.5\Omega = 5V`. Let us assume the push pull 
configuration of OPA549 positions the coil potential in the middle between :math:`U_{vcc}` and GND. The voltage drop across 
every OPA549 would then be :math:`\frac{20 - 5}{2} = 7.5V`, which causes a heat dissipation of :math:`7.5V * 10A = 75W`.

