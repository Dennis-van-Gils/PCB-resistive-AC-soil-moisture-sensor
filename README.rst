.. image:: https://knowledge.autodesk.com/sites/default/files/product-logo-sm/eagle-2017-badge-75x75.png
.. image:: https://img.shields.io/badge/License-MIT-purple.svg
    :target: https://github.com/Dennis-van-Gils/PCB-resistive-AC-soil-moisture-sensor/blob/master/LICENSE.txt

Resistive AC soil moisture sensor
=================================

This project involves a printed circuit board design to measure the resistance
between two metallic probes that can be placed inside soil as a measure of the
soil humidity. It uses AC current to sense the resistance between the probes,
preventing corrosion of the probes which otherwise would be the case when a DC
current would be used instead. The design is based on the
`SMX sensor <https://www.emesystems.com/smx/main.html>`__ by EME Systems.

The sensor works on both 3.3 V and 5.0 V supply voltage. The output is a pulse
train with an amplitude equal to the supply voltage and of a variable frequency
that correlates to the measured resistance. The pulses are at a 50% duty cycle.

Contains EAGLE CAD, Fusion360 and LTSpice simulation files.

- Github: https://github.com/Dennis-van-Gils/PCB-resistive-AC-soil-moisture-sensor

PCB design
==========

The PCB production Gerber files can be found `here. <EAGLE/AC_soil_sensor_v1.0_2022-05-01.zip>`__

.. image:: Fusion360/AC_soil_sensor_v1.0.png

.. image:: EAGLE/AC_soil_sensor_v1.0_schematic.png

Bill of materials
=================

+-----+-------------------+--------------------------------------+------------------------------------+
| IC1 | CMOS low-dropout voltage regulator, 3.0V 250mA           | MCP1700-3002E/TO                   |
+-----+-------------------+--------------------------------------+------------------------------------+
| IC2 | CMOS 555 timer, input 2V-15V                             | TLC555CP                           |
+-----+-------------------+--------------------------------------+------------------------------------+
| R1  | 4.7 kΩ  1/4W   5% | Resistor, carbon film                |                                    |
+-----+-------------------+--------------------------------------+------------------------------------+
| R2  | 100  Ω  1/4W   5% | Resistor, carbon film                |                                    |
+-----+-------------------+--------------------------------------+------------------------------------+
| R3  | 150 kΩ  1/4W   5% | Resistor, carbon film                |                                    |
+-----+-------------------+--------------------------------------+------------------------------------+
| R4  | 390  Ω  1/4W   5% | Resistor, carbon film                |                                    |
+-----+-------------------+--------------------------------------+------------------------------------+
| C1  | 1   μF   50V  10% | Capacitor, ceramic                   | KEMET C320C105K5N5TA91707301       |
+-----+-------------------+--------------------------------------+------------------------------------+
| C2  | 0.1 μF   50V  10% | Capacitor, ceramic                   | KEMET C320C104K5R5TA7301           |
+-----+-------------------+--------------------------------------+------------------------------------+
| C3  | 1   μF   50V      | Capacitor, aluminium electrolytic    |                                    |
+-----+-------------------+--------------------------------------+------------------------------------+
| C4  | 0.1 μF   63V   5% | Capacitor, metallized polyester film | TDK B32529C0104J                   |
+-----+-------------------+--------------------------------------+------------------------------------+
| C5  | 4.7 μF   25V  10% | Capacitor, ceramic                   | KEMET C320C475K3N5TA91707301       |
+-----+-------------------+--------------------------------------+------------------------------------+
| C6  | 4.7 μF   25V  10% | Capacitor, ceramic                   | KEMET C320C475K3N5TA91707301       |
+-----+-------------------+--------------------------------------+------------------------------------+

LTspice simulation
==================

The frequency-resistance relationship of the simulated idealized circuit:

.. image:: LTspice_simulation/SMX_simulation_v3_frequency_response.png

Dennis van Gils, 2022