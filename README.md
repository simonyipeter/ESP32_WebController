# ESP32_WebController
ESP32 WebController multitask webserver, oled and rotary encoder driver
Responsive web interface based on Bootstrap. The variables can be modified through the web site or the rotary encoder
Tha variables are stored in the non-volatile

# Install
- Install the reqired libaries
- Fill the ssid and password fields to connect to the Wifi network
- Connect the OLED display to SDA and SCL pins and modify the I2C address
- Connect the rotary encoder to digital pins and modify encoder0PinA encoder0PinB and encoderButton variables

# Modify
- Move the esp32_script.js file to another host provider and edit.
