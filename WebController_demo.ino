#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>
#include <Preferences.h>

Preferences preferences;

#define MAX_STRING_LEN  20

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10 #define XPOS 0 #define YPOS 1 #define DELTAY 2

const char* ssid = "xxxx"; const char* password = "xxxx";

unsigned int ref_time;

int demo_variable_1 = 0;
int demo_variable_2 = 0;
int demo_variable_3 = 0;

String html_header = F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n"
                       "<html><head>  <title>ESP Webgui</title>  <meta charset=\"utf-8\">  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">  "
                       "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">  "
                       "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/bootstrap-slider/9.8.1/css/bootstrap-slider.css\">  "
                       "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js\"></script>  "
                       "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script> "
                       "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/bootstrap-slider/9.8.1/bootstrap-slider.js\"></script>"
                       
                       "<script src=\"https://simonyipeter.github.io/ESP32_WebController/esp32_script.js\"></script>"
                       "</head><body>");
String html_footer = F("<body></html>\r\n\r\n");

int encoder0PinA = 16; int encoder0PinB = 17; int encoderButton = 19; int encoder0PinALast = LOW; int n = LOW;


// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);


String getValue(String data, char separator, int index) {    int found = 0;    int strIndex[] = { 0, -1 };    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {            found++;            strIndex[0] = strIndex[1] + 1;            strIndex[1] = (i == maxIndex) ? i+1 : i;        }
    }    return found > index ? data.substring(strIndex[0], strIndex[1]) : ""; }


byte current_menu_id=2;
byte max_menu_id=3;
boolean is_in_menu=false;

void menu_handler(String menu_command) {
  display.clearDisplay();display.setTextSize(2);

  if(menu_command=="Push" && !is_in_menu){ is_in_menu=true;    }

  else if(menu_command=="Push" && is_in_menu){     is_in_menu=false;   
  //Save the variables, when exit from menu
  preferences.putUInt("demo_variable_1", demo_variable_1);
  preferences.putUInt("demo_variable_2", demo_variable_2);
  preferences.putUInt("ref_time", ref_time);
  Serial.println(F("Saved to Flash"));

  }
  

  //Rotate the menus, if not in menu!
  if(menu_command=="Left" && is_in_menu==false){   if(current_menu_id==1){current_menu_id=max_menu_id;}else{current_menu_id--;}    }
  if(menu_command=="Right" && is_in_menu==false){    if(current_menu_id==max_menu_id){current_menu_id=1;}else{current_menu_id++;}    }

  //Adjust the variable in menu
  
  if(menu_command=="Left" && is_in_menu){   
    
     switch (current_menu_id) {
    
        case 1:
              demo_variable_1--;
              break;
              
        case 2:
              demo_variable_2--;
              break;

        case 3:
              ref_time--;
              if(ref_time<6){ref_time=5;}
              break;
     }
    }

  
  if(menu_command=="Right" && is_in_menu){    
      
     switch (current_menu_id) {
    
        case 1:
              demo_variable_1++;
              break;
              
        case 2:
              demo_variable_2++;
              break;

        case 3:
              ref_time++;
              break;
    }
    
    }
  

  //Just display the variables
  switch (current_menu_id) {
    
    case 1:
          display.setCursor(4, 0); display.print(F("Variable 1"));
          if(is_in_menu){   display.setCursor(0, 18); display.print(F(">")); } //if the variable is adjusted
          display.setCursor(50, 18); display.print(demo_variable_1); 
          display.display();
          break;

    case 2:
          display.setCursor(4, 0); display.print(F("Variable 2")); 
          if(is_in_menu){   display.setCursor(0, 18); display.print(F(">")); } //if the variable is adjusted
          display.setCursor(50, 18); display.print(demo_variable_2); 
          display.display();
          break;

     case 3:
          display.setCursor(4, 0); display.print(F("Refresh")); 
          if(is_in_menu){   display.setCursor(0, 18); display.print(F(">")); } //if the variable is adjusted
          display.setCursor(50, 18); display.print(ref_time); 
          display.display();
          
          break;
    
    }
  
  
  }

void WebserverTask( void * pvParameters ){ 
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {    delay(500);    Serial.print(".");  }
  
  display.clearDisplay();display.setTextSize(1);
  Serial.println("");
  Serial.print(F("Connected to ")); display.setCursor(0, 0); display.print(F("Connected!")); display.display();
  Serial.println(ssid);
  Serial.print(F("IP address: ")); display.setCursor(0, 10); display.print(WiFi.localIP()); display.display();
  Serial.println(WiFi.localIP());    delay(1500);
  display.clearDisplay(); display.setCursor(0, 0);

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("esp32")) {    Serial.println("Error setting up MDNS responder!");    while (1) {      delay(1000);    }  }
  Serial.println("mDNS responder started");

  // Start TCP (HTTP) server
  server.begin();    Serial.println("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  
while(true){

  
 // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {    continue;  }
  //Serial.println("");    Serial.println("New client");
  
  // Wait for data from client to become available
  while (client.connected() && !client.available()) {    delay(1);  }

  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    Serial.print(F("Invalid request: "));    Serial.println(req);    continue; 
  }
  req = req.substring(addr_start + 1, addr_end);
  Serial.print(F("Request: "));  Serial.println(req);

  //Serial.print(F("Request1: "));    Serial.println(getValue(req, '/', 0));
  //Serial.print(F("Request2: "));    Serial.println(getValue(req, '/', 1));
  //Serial.print(F("Request3: "));    Serial.println(getValue(req, '/', 2));
  //Serial.print(F("Request4: "));    Serial.println(getValue(req, '/', 3));
  

  client.flush();

 if (req == "/")
  {
    client.print(html_header);
    
    client.print(F("<div class=\"container\" style=\"max-width: 700px;\">            "));
    client.print(F("<div class=\"row\">            "));
    client.print(F("<div class=\"page-header\"> <center><h2>ESP WebController</h2></center>  </div>  "));
      client.print(F("<div class=\"col-md-4\">Relay switch: <button id=\"button_one\" class=\"btn btn-success\" onclick=\"button_one()\">GPIO-5</button>          </div>"));
      client.print(F("<div class=\"col-md-8\">Demo variable: <span id=\"slider_val\"></span> &nbsp;&nbsp <input type=\"text\"  name=\"csuszka\" id=\"csuszka\" data-slider-min=\"0\" data-slider-max=\"20\" data-slider-value=\""));
      client.print(demo_variable_1);
      client.print(F("\">     <button id=\"button_demo_var_1\" class=\"btn btn-success\" onclick=\"button_demo_1()\">Update</button>   </div>"));
    client.print(F("</div>"));
    
    client.print(F("<div class=\"row\">   "));
       client.print(F("<div class=\"col-md-6 input-group\">"
                          "<span class=\"input-group-addon\">Refresh time</span><input type=\"text\" class=\"form-control\" id=\"ref_time\" value=\""));
      
       client.print(ref_time);
       client.print(F("\"><span class=\"input-group-btn\"><button id=\"ref_time_ok\" class=\"btn btn-success\" onclick=\"ref_time()\">Update</button>"
                            "<button id=\"refresh\" class=\"btn btn-success\" onclick=\"window.location.reload(1)\">Refresh</button> </span>         </div>"));
    client.print(F("</div>"));
    
    client.print(F("</div>"));
    client.print(html_footer);

  }

  

  else if (getValue(req, '/', 1) == "gpio")  // URL: /gpio/5/on or /gpio/5/off
  {
    
    int pin_number=getValue(req, '/', 2).toInt();
    pinMode(pin_number, OUTPUT); 
    if(getValue(req, '/', 3) == "on") { pinMode(pin_number, OUTPUT); digitalWrite(pin_number, HIGH);     }
    if(getValue(req, '/', 3) == "off"){ pinMode(pin_number, OUTPUT);  digitalWrite(pin_number, LOW);     }
    client.print( F("OK"));
    
  }

  else if (getValue(req, '/', 1) == "ref_time")  // Update refresh time and save to EEPROM
  {

    ref_time=getValue(req, '/', 2).toInt();
    if(ref_time<6){ref_time=5;}
    menu_handler("Refresh"); //Refresh the display
    preferences.putUInt("ref_time", ref_time);
    client.print( F("OK")); 
  }

  else if (getValue(req, '/', 1) == "demo_1")  // Update demo_variable_1 variable and save to EEPROM
  {

    demo_variable_1=getValue(req, '/', 2).toInt();
    preferences.putUInt("demo_variable_1", demo_variable_1);
    menu_handler("Refresh"); //Refresh the display
    client.print( F("OK")); 
  }


  else
  {
    client.print(F("HTTP/1.1 404 Not Found\r\n\r\n"));
    Serial.println(F("Sending 404"));
  }
  

  
}}

void main_loop_Task( void * pvParameters ){    

      unsigned long pushtime;
      long previousMillis = 0;  
  
      while(true){
  
           delay(2);
            n = digitalRead(encoder0PinA);  if ((encoder0PinALast == LOW) && (n == HIGH)) {
            //if (digitalRead(encoder0PinB) == LOW) {         encoder0Pos++;           }             else {       encoder0Pos--;          }
            if (digitalRead(encoder0PinB) == LOW) {         menu_handler("Left");           }             else {      menu_handler("Right");               }

            //Serial.println(encoder0Pos);
            //display.clearDisplay();    display.setTextSize(2); display.setTextColor(WHITE);    display.setCursor(0, 10);
            //display.print(encoder0Pos); display.display();
      
            }  encoder0PinALast = n;

            
            if (digitalRead(encoderButton) == LOW && (millis()-pushtime)>170) {
             Serial.println("Push");              pushtime = millis();             menu_handler("Push");
             }


            

            
            unsigned long currentMillis = millis(); 
             if(currentMillis - previousMillis > ref_time*1000) {
               // save the last time 
               previousMillis = currentMillis;   
               //Serial.print("Refresh:");   Serial.println(ref_time);   
               menu_handler("Refresh");
             }

            
       }}
 
void setup() {
 
  Serial.begin(112500);  delay(100);
  
  pinMode (encoder0PinA, INPUT_PULLUP);  pinMode (encoder0PinB, INPUT_PULLUP); pinMode (encoderButton, INPUT_PULLUP);

  preferences.begin("storage", false);

  demo_variable_1 = preferences.getUInt("demo_variable_1", 0);
  demo_variable_2 = preferences.getUInt("demo_variable_2", 0);
  ref_time = preferences.getUInt("ref_time", 6);

  //Serial.println(ref_time);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display(); display.clearDisplay();
  display.setTextSize(1); display.setTextColor(WHITE);

  display.setCursor(0, 0); display.print(F("Boot")); display.display();

  TaskHandle_t webserver;
 
  xTaskCreatePinnedToCore(
                    WebserverTask,   /* Function to implement the task */
                    "Webserver", /* Name of the task */
                    10000,      /* Stack size in words */
                    NULL,       /* Task input parameter */
                    0,          /* Priority of the task */
                    &webserver,  /* Task handle. */
                    0);         /* Core where the task should run */

  TaskHandle_t main_loop;
 
  xTaskCreatePinnedToCore(
                    main_loop_Task,   /* Function to implement the task */
                    "main_loop", /* Name of the task */
                    10000,      /* Stack size in words */
                    NULL,       /* Task input parameter */
                    1,          /* Priority of the task */
                    &main_loop,  /* Task handle. */
                    1);         /* Core where the task should run */
 
  Serial.println("Tasks created...");
}
 
void loop() {}
