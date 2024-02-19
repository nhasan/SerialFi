# SerialFi
SerialFi allows you to stream serial data from a micro-controller board to browser clients using any `ESP8266` based board. The firmware is developed and tested on a NodeMCU board but should work on any `ESP8266` based board with little or no modifications. The firmware also allows to send textual data back to the device from the wen interface, allowing two-way communication with the micro-controller board.

The figure below shows a NodeMCU board with SerialFi firmware connected to an Arduino Uno. Since the Arduino is a 5v device, a Zener diode (1N4148) is used on the TX pin of the arduino for level shifting as the NodeMCU is only 3.3v tolerant.

 ![Schematic](serialfi_bb.png)

The figure below shows the screenshot of the web interface of SerialFi.

 ![Screenshot](serialfi_web.png)
