# FilmTactileSensor
FilmTactileSensor Based on ESP32

Explanation of film tactile sensor. Some sample codes for different situations.

## Common
PC side is developed by Processing.
ESP32 side is developed by Arduino IDE

### Preparation
Check serial port number, and write program to ESP32 by Arduino IDE

### Processing program common operation 
1,2,3,4: Change measurement range
s: start and stop recording. A new csv file is generated.

## Sample1: wired communication, 3 fingers measurement.
- ESP32: Measurement5x10_3FingersESP32.ino
- Processing: Measurement5x10_3Fingers8bit.pde

This is the most basic sample using USB connection. 500 Hz measurement of all three fingers.
Before start, change serial port number in Processing program.

## Sample2: wired communication, 1 finger measurement.
- ESP32: Measurement5x10_1Fingers8bitESP32.ino
- Processing: Measurement5x10_1Fingers8bitESP32.pde

This measures only 1 finger, to make it faster. 1kHz sampling is achieved.
Before start, change serial port number in Processing program.
Press "f" to change finger.

## Sample3: wireless communication, 3 fingers measurement.

- ESP32: Measurement5x10_3Fingers8bitWifiESP32.ino
- Processing: Measurement5x10_3Fingers8bitWiFi.pde

This is wireless communication sample, using ESP32 as access point (AP). 400 to 500 Hz measurement of three fingers.
Measurements are conducted 15 times before transmitting data, for stable communication, resulting in 30ms delay.
If you need to do measurement in real-time, change BUFFER_NUM in the code.

Power on ESP32 through USB port, by either connecting to PC or mobile battery.
Mobile battery might be better for practical wireless applications.

ESP32 SSID can be seen as "ESP32Wifi", and its password is "password" (you should change those).
Connect your PC to this access point. It means that the PC cannot connect to other network.
If you need to connect to the other network, you need to change the program and use wireless router as an access point.

