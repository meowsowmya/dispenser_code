#include <Arduino.h>
# include <Wifi.h>


//wifi 
const char* ssid = "spongebot"; //name of hotspot
const char* password =  "squarepants"; //password
String table_char = "";
String parked = "Parked";
WiFiServer wifiServer(80);

//Motor Driver 
const int PIN_STEP = 16; //pwm, number of steps by stepper motor
const int PIN_DIR = 4; //digital, motor rotation direction
const int PIN_EN = 17; //digital, low = motor enabled
const int PIN_RST = 0; //digital, low = motor enabled
const int steps_per_rev = 200; //200 steps make 1 rev for our stepper
const int num_rev = 13; //to move 70mm, need 35 rev
const int inner_delay = 2000; //stepper pulse duration
const int outer_delay = 10; //time between each revolution
const bool dir_forward = HIGH; //can holder moves forward
const bool dir_backward = LOW; //can holder moves backward
void motor_driver_setup(); //code to setup motor driver in setup() 
void motor_driver_dispense(); //code for dispensing can


//Button Panel
//For ESP32 we can use all GPIO pins for external interrupt except for GPIO6, GPIO7, GPIO8, GPIO9, GPIO10, and GPIO11
const int TABLE1_PIN = 5; //defining button pin to table mapping
const int TABLE2_PIN = 18;
const int TABLE3_PIN = 19;
const int TABLE4_PIN = 21;
const int TABLE5_PIN = 22;
const int TABLE6_PIN = 23; 

int LED_PIN;
int led_pin_array[6] = {14,27,26,25,33,32}; 
unsigned long int prev_dispense_time = millis(); 
unsigned long int current_dispense_time; 
volatile static int table_number = 0; //changed when button pressed through interrupt
volatile static unsigned long int button_press_time = 0; //changed when button pressed through interrupt
void button_panel_setup(); //setup button panel + led in setup()
void display_led(); //to display which button pressed on led row
void reset_led(); //to turn off all leds indicating no button pressed yet


//Ultrasonic
const int TRIG_PIN = 13; // Trigger Pin of Ultrasonic Sensor
const int ECHO_PIN = 12; // Echo Pin of Ultrasonic Sensor
bool object_detected; //high if turtlebot is docked
long duration, distance_cm;
void ultrasonic_sensor_setup(); //setup ultrasonic sensor in setup()
bool check_obstacle(); //checks for turtlebot during alignment and docking phase

//Interrupt Service Routine Functions that change table number and record button press time
void IRAM_ATTR ISR_T1()
{ if (table_number !=0){digitalWrite(led_pin_array[table_number-1], LOW);}
  table_number = 1;
  button_press_time = millis();
  digitalWrite(led_pin_array[table_number-1], HIGH);
}
void IRAM_ATTR ISR_T2()
{ if (table_number !=0){digitalWrite(led_pin_array[table_number-1], LOW);}
  table_number = 2;
  button_press_time = millis();
  digitalWrite(led_pin_array[table_number-1], HIGH);
}
void IRAM_ATTR ISR_T3()
{ if (table_number !=0){digitalWrite(led_pin_array[table_number-1], LOW);}
  table_number = 3;
  button_press_time = millis();
  digitalWrite(led_pin_array[table_number-1], HIGH);

}
void IRAM_ATTR ISR_T4()
{ if (table_number !=0){digitalWrite(led_pin_array[table_number-1], LOW);}
  table_number = 4;
  button_press_time = millis();
  digitalWrite(led_pin_array[table_number-1], HIGH);

}
void IRAM_ATTR ISR_T5()
{ if (table_number !=0){digitalWrite(led_pin_array[table_number-1], LOW);}
  table_number = 5;
  button_press_time = millis();
  digitalWrite(led_pin_array[table_number-1], HIGH);
}
void IRAM_ATTR ISR_T6()
{ if (table_number !=0){digitalWrite(led_pin_array[table_number-1], LOW);}
  table_number = 6;
  button_press_time = millis();
  digitalWrite(led_pin_array[table_number-1], HIGH);
}




void motor_driver_setup(){
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_EN, OUTPUT);
  pinMode(PIN_RST, OUTPUT);
  digitalWrite(PIN_RST, LOW);
  delayMicroseconds(500);
  digitalWrite(PIN_RST, HIGH);
  digitalWrite(PIN_EN, HIGH);
}

void button_panel_setup(){
  pinMode(TABLE1_PIN, INPUT_PULLUP);
  pinMode(TABLE2_PIN, INPUT_PULLUP);
  pinMode(TABLE3_PIN, INPUT_PULLUP);
  pinMode(TABLE4_PIN, INPUT_PULLUP);
  pinMode(TABLE5_PIN, INPUT_PULLUP);
  pinMode(TABLE6_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TABLE1_PIN), ISR_T1, FALLING); //interrupt detects high to low change
  attachInterrupt(digitalPinToInterrupt(TABLE2_PIN), ISR_T2, FALLING);
  attachInterrupt(digitalPinToInterrupt(TABLE3_PIN), ISR_T3, FALLING);
  attachInterrupt(digitalPinToInterrupt(TABLE4_PIN), ISR_T4, FALLING);
  attachInterrupt(digitalPinToInterrupt(TABLE5_PIN), ISR_T5, FALLING);
  attachInterrupt(digitalPinToInterrupt(TABLE6_PIN), ISR_T6, FALLING);
  for (int i=0; i < 6; i++){ //initialize LEDs and set to LOW 
    pinMode(led_pin_array[i], OUTPUT);
    digitalWrite(led_pin_array[i], LOW);
  }
}

void ultrasonic_sensor_setup(){
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}


//Execution Functions

void display_led(){
    for (int i = 0; i < 6; i++){
    digitalWrite(led_pin_array[i], LOW);
    }
    if (table_number > 0 && table_number <7){
    LED_PIN = led_pin_array[(table_number-1)]; //maps table number to led pin
    digitalWrite(LED_PIN, HIGH); //light up correct led
  }
}

void reset_led(){
  for (int i = 0; i < 6; i++){
    digitalWrite(led_pin_array[i], LOW); //turn off all led till button pressed in next dispense round
  }
}

bool check_obstacle(){
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance_cm = duration*0.017; //spd of sound = 0.034 cm/s 
  if (distance_cm < 15){
    object_detected = HIGH;
  }
  else{
    object_detected = LOW;
  }
  return object_detected;
}

  //Limit switch should stop all future motor driver remaining steps, 
  //so enable should be high and driver should be reset
  //rest of the code can then run without having any effect
void ping_bot(String send_data, WiFiClient client){
  for (int i = 0; i < send_data.length(); i++){
    client.write(send_data[i]);
  }
  client.write(" ");
}

void motor_driver_dispense(WiFiClient client){
  digitalWrite(PIN_EN, LOW); //enables motor
  Serial.println("Starting Forward Movement"); //moving forward
  digitalWrite(PIN_DIR, dir_forward); 
  ping_bot((String)table_number, client);
  for(int x = 0; x < num_rev; x++){
    for(int y = 0; y < steps_per_rev; y++){
      digitalWrite(PIN_STEP, HIGH); //sending pulse to turn motor by 1 step
		  delayMicroseconds(inner_delay);
		  digitalWrite(PIN_STEP, LOW);
		  delayMicroseconds(inner_delay);
    }
    delay(outer_delay);
    ping_bot((String)table_number, client);
  }
  delay(1000);
  Serial.println("Starting Backward Movement"); //moving backward
  digitalWrite(PIN_DIR,dir_backward); 
  for(int x = 0; x < num_rev; x++){ 
    for(int y = 0; y < steps_per_rev; y++){
      digitalWrite(PIN_STEP, HIGH);
		  delayMicroseconds(inner_delay);
		  digitalWrite(PIN_STEP, LOW);
		  delayMicroseconds(inner_delay);
    }
  delay(outer_delay);
  ping_bot((String)table_number, client);
  } 
}



void wifi_setup(){
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  wifiServer.begin();//begin wifi connection
}


void main_code_segment(WiFiClient client) { 
  Serial.println(table_number); //Table Number recording is done through interrupts (ISR_T1 to ISR_T6)
  display_led(); 
  object_detected = check_obstacle();
  //digitalWrite(PIN_RST, HIGH);
  if (object_detected == HIGH){
    Serial.println("object detected once");
    delay(3000);
    object_detected = check_obstacle();
    if (object_detected == HIGH){
    Serial.println("object detected again");
    ping_bot((String)"P", client);
    }
  }

  if (object_detected == HIGH){
    current_dispense_time = millis();
    delay(1000);
    if (table_number != 0){ 
      motor_driver_dispense(client); //can gets dispensed
      prev_dispense_time = millis();
      table_number = 0;
      display_led();
      Serial.println("Dispensed once");
      //digitalWrite(PIN_RST, LOW); //reset stepper, active low
      delayMicroseconds(500);
      //digitalWrite(PIN_RST, HIGH);
    }   
    else{ 
      Serial.println("disable motor");
      delay(100);
      digitalWrite(PIN_EN, HIGH); //disable motor
    }
  }
  else{
    Serial.println("disable motor");
    delay(100);
    digitalWrite(PIN_EN, HIGH); //disable motor
  }
}

void setup() {
  Serial.begin(115200); //establish serial communication
  wifi_setup();
  motor_driver_setup();
  button_panel_setup();
  ultrasonic_sensor_setup();
  Serial.println("Setup complete");
}

void loop(){
  WiFiClient client = wifiServer.available();
  if (client) {
    while(client.connected()){  
      ping_bot((String)"H",client);
      main_code_segment(client);
    }
  client.stop();
  }
  else{
    Serial.println("Meow");
    Serial.println(WiFi.localIP());
    delay(1000);
  }
}