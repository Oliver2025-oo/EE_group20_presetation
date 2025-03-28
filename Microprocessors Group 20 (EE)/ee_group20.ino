#include <LiquidCrystal.h>
#include <TimerOne.h>

// Initialize LCD with pin configuration
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// Define pin assignments
const int PULSE_PIN = 4, BUTTON_PIN = 2, TEMP_PIN = A1;

// Global variables for measurements
int pulseCount = 0, heartRate = 0, elapsedSec = 0, dotStep = 0;
float temperature = 0;
bool measuring = false;
unsigned long lastDotTime = 0;
int HBCheck = 0; // Tracks heartbeat detection state

void setup() {
  pinMode(PULSE_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.begin(20, 4); // Initialize 20x4 LCD display

  // Configure Timer1 to trigger every 0.8 seconds
  Timer1.initialize(800000);
  Timer1.attachInterrupt([] {
    if (measuring && ++elapsedSec >= 5) { // Stop measurement after 5 seconds
      measuring = false;
      heartRate = pulseCount * 12; // Convert pulse count to BPM
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Heart rate: "); lcd.print(heartRate); lcd.print(" BPM");
      lcd.setCursor(0, 1);
      lcd.print("Temp: "); lcd.print(temperature); lcd.print(" C");
      lcd.setCursor(0, 2);  
      lcd.print("Press button to");
      lcd.setCursor(0, 3); 
      lcd.print("check again");
    }
  });

  displayWelcome(); // Show welcome message
}

void loop() {
  // Read temperature sensor value and convert to degrees Celsius
  temperature = analogRead(TEMP_PIN) * 0.48876;
  
  // Check if the button is pressed to start measurement
  if (digitalRead(BUTTON_PIN) == LOW && !measuring) {
    delay(200); // Debounce delay
    measuring = true;
    pulseCount = elapsedSec = dotStep = 0; // Reset measurement variables
    lcd.clear();
    lcd.print("Measuring...");
  }

  if (measuring) {
    // Detect heartbeats by checking pulse pin transitions
    if (digitalRead(PULSE_PIN) == HIGH && HBCheck == 0) { 
      pulseCount++; 
      HBCheck = 1; 
    }
    if (digitalRead(PULSE_PIN) == LOW && HBCheck == 1) 
      HBCheck = 0;

    // Display animated dots while measuring
    if (millis() - lastDotTime >= 500) {
      lastDotTime = millis();
      lcd.setCursor(10, 0);
      lcd.print("   "); // Clear previous dots
      lcd.setCursor(10, 0);
      for (int i = 0; i < ++dotStep % 4; i++) 
        lcd.print(".");
    }
  }
}

// Displays initial welcome message on LCD
void displayWelcome() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  EE Vitals Monitor  ");
  lcd.setCursor(0, 2);
  lcd.print("  Press the button  ");
  lcd.setCursor(0, 3);
  lcd.print(" to check your vitals");
}
