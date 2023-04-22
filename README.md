# dispenser_code
To Dispense a can to TurtleBot3 for delivery in a mock restaurant

As part of a university-level course, a task to dispense a can to TurtleBot3 was assigned, and send the table number to Turtlebot3 so that it may deliver it accordingly in a mock restaurant. This code interfaces an ultrasonic sensor, 6 push buttons, 6 LEDs and a stepper motor (A4988 Driver) through Doit-ESP32-Devkit1, and also uses  WiFi to communicate the table number to any client connected to it. Interrupts are used to immediately save new table number.

Headers needed : <Arduino.h> and <WiFi.h> 
Coding platform : PlatformIO (Core 6.1.6, Home 3.4.3)

main.cpp contains the code that is uploaded to the ESP32
esp_client.py contains the code to become a client and receive information via WiFi from ESP32

