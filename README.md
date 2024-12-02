# turn-on-pylontechUS2000C
Code for an ESP32 with max485 and SPI SD Card module to Read the state of a pylontech LiFePo4 Battery and turn it on in the morning


Read the Modbus Protocol with the help of a max485 module. Connect it as follows:

VCC -> 5V | on ESP32
GND -> GND | on ESP32

DI -> Tx | on ESP32
DE/RE -> GPIO25 | on ESP32 (connect both DE and RE pins to GPIO25)
RO -> Rx | on ESP32

A -> Pin5 of RS485 | on FSP Dual Modbus Box
B -> Pin4 of RS485 | on FSP Dual Modbus Box
