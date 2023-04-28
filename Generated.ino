#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include <Arduino.h>

// Define I2C LCD screen properties
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define pins for buttons, temperature sensor, and pH sensor and switch
const int upButtonPin = 7;
const int selectButtonPin = 6;
const int oneWireBusPin = 2;
const int PHSensorPin = A0;
const int switchPin = 8;

//pH sensor
float calibration_value = 21.34;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;

// Define menu items
const int numMenuItems = 3;
const String menuItems[] = {"Oosty", "Coosa", "Etowah"};

// Define variable to keep track of current menu item
int currentMenuItem = 0;

// Create a OneWire object and a DallasTemperature object
OneWire oneWire(oneWireBusPin);
DallasTemperature sensors(&oneWire);

// Define SD card properties
const int chipSelectPin = 10;

// Create a file object for the log file
File dataFile;


void setup() {
  // Initialize temperature sensor
  sensors.begin();
  Serial.begin(9600);
  // Initialize LCD screen and buttons
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  //lcd.setContrast(100);  // Set the contrast to 100
  pinMode(upButtonPin, INPUT_PULLUP);
  pinMode(selectButtonPin, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP); // This is for the switch// internal resistor 

  

  // Initialize SD card
  Serial.begin(9600);
  while (!Serial) {}
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelectPin)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialization done.");

  // Display first menu item on LCD screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("> " + menuItems[currentMenuItem]);
}


void loop () {
   // Read the state of the switch
  int switchState = digitalRead(switchPin);

  if (switchState == LOW) {
    // Turn off the LCD if the switch is pressed
    //lcd.off();
  }
  else {
    // Turn on the LCD if the switch is not pressed
    //lcd.on();
    //delay(100);
}
  // Check for button presses
  if (digitalRead(upButtonPin) == HIGH) {
    // Increment current menu item and wrap around if necessary
    currentMenuItem = (currentMenuItem + 1) % numMenuItems;
    // Update LCD screen to show new menu item
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("> " + menuItems[currentMenuItem]);
    delay(300);
  }
  else if (digitalRead(selectButtonPin) == HIGH) {
    // Handle selection of current menu item
    switch (currentMenuItem) {
      case 0:
        // Option 1 selected, takes pH and temp reading
        testFunction("Oostanaula");
        break;
      case 1:
        // Option 2 selected, do something
        testFunction("Coosa");
        break;
      case 2:
        // Option 3 selected, take temperature and pH measurements and log them to SD card
        testFunction("Etowah");
        break;
      default:
        // Should never happen
        break;
    }
    // Display selected option for 2 seconds
    delay(6000);
    // Return to menu display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("> " + menuItems[currentMenuItem]);
  }
}


void testFunction(String loc) {
  lcd.clear();
  lcd.print("Measuring...");
  float temperature = getTemperature();
  float pH = getPH();
 logData(loc, temperature, pH);
  lcd.clear();
  lcd.print("Temp: " + String(temperature, 1) + " F");
  lcd.setCursor(0, 1);
  lcd.print("pH: " + String(pH, 1));
}

void logData(String menuItem, float temperature, float pH) {
  // Create a file name for the log file based on the menu item selected
  String fileName = menuItem + ".txt";
  
  // Open the log file in append mode
  dataFile = SD.open(fileName, FILE_WRITE);
  
  // Write the temperature and pH values to the log file
  dataFile.print(temperature, 1);
  dataFile.print(",");
  dataFile.print(pH, 1);
  dataFile.println();
  
  // Close the log file
  dataFile.close();
}

float getPH() {
  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(A0);
    delay(30);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1024 / 6;
  float ph_act = -5.70 * volt + calibration_value;
   return ph_act;
}

float getTemperature() {
  // Request temperature reading from sensor
  sensors.requestTemperatures();
  // Calculate and print the temperature in Fahrenheit
  float tempC = sensors.getTempCByIndex(0);
  float tempF = (tempC * 9.0 / 5.0) + 32.0;
  Serial.print(tempF);
  Serial.print((char)176); // shows degrees character
  Serial.println("F");
  // Return the temperature in Fahrenheit
  return tempF;
}
