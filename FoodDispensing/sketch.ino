#include <Arduino.h>
#include <ESP32Servo.h>  // Include the ESP32Servo library
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

// Include token generation and RTDB helper
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Define your Wi-Fi credentials
#define WIFI_SSID "Wokwi-GUEST"  // Replace with your SSID
#define WIFI_PASSWORD ""         // Replace with your Wi-Fi password

// Firebase project credentials
#define API_KEY "AIzaSyBFDl3OfOqj8lU9vN4ygmDYBD7XJG_xO1I"  // Replace with your Firebase API Key
#define DATABASE_URL "https://pettify-af8a3-default-rtdb.firebaseio.com/"  // Replace with your Firebase RTDB URL

// Pin definitions for ultrasonic sensors
#define MAIN_CONTAINER_TRIG_PIN 5   // Trigger pin for main container ultrasonic sensor
#define MAIN_CONTAINER_ECHO_PIN 4   // Echo pin for main container ultrasonic sensor
#define BOWL_TRIG_PIN 12           // Trigger pin for bowl ultrasonic sensor
#define BOWL_ECHO_PIN 13           // Echo pin for bowl ultrasonic sensor

// Servo Pin
#define SERVO_PIN 9  // Connect your servo motor to this pin

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long lastRequestMillis = 0;
bool signupOK = false;

// Sensor data variables
float mainContainerDistance = 0.0;  // Distance to the food in the main container
float bowlDistance = 0.0;           // Distance to the bowl
float maxContainerHeight = 30.0;    // Maximum height of food in the container (cm)
float maxBowlHeight = 10.0;         // Height at which the bowl is considered "full" (cm)

// Create a Servo object using ESP32Servo library
Servo foodServo;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the API key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long-running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Set up ultrasonic sensor pins
  pinMode(MAIN_CONTAINER_TRIG_PIN, OUTPUT);
  pinMode(MAIN_CONTAINER_ECHO_PIN, INPUT);
  pinMode(BOWL_TRIG_PIN, OUTPUT);
  pinMode(BOWL_ECHO_PIN, INPUT);

  // Attach the servo motor to the defined pin
  foodServo.attach(SERVO_PIN);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - lastRequestMillis > 15000 || lastRequestMillis == 0)) {
    lastRequestMillis = millis();

    // Read the requested food quantity from Firebase
    if (Firebase.RTDB.getInt(&fbdo, "/feedingRequests/requestedFood")) {
      int requestedFood = fbdo.intData();
      Serial.print("Requested Food Quantity: ");
      Serial.println(requestedFood);

      // If there is no food requested, do nothing
      if (requestedFood <= 0) {
        return; // Skip this loop cycle if no food is requested
      }

      // Read the sensor data for the main container and bowl
      mainContainerDistance = measureDistance(MAIN_CONTAINER_TRIG_PIN, MAIN_CONTAINER_ECHO_PIN);
      bowlDistance = measureDistance(BOWL_TRIG_PIN, BOWL_ECHO_PIN);

      // Calculate the food available in the main container (in grams)
      int foodInContainer = mapDistanceToFoodAmount(mainContainerDistance);

      // Check the bowl status (full or empty)
      String bowlStatus = (bowlDistance < maxBowlHeight) ? "full" : "empty";

      String actionMessage = "";

      if (bowlStatus == "full") {
        // If the bowl is full, no food should be dispensed
        actionMessage = "The bowl is full. No food will be dispensed.";
      }
      else {
        if (foodInContainer >= requestedFood) {
          // If there’s enough food in the container, dispense the requested amount
          actionMessage = dispenseFood(requestedFood);
          // After dispensing, reset the requested food to 0 in Firebase
          Firebase.RTDB.setInt(&fbdo, "/feedingRequests/requestedFood", 0);
        }
        else if (foodInContainer > 0) {
          // If there’s less food than requested but some food is available
          actionMessage = dispenseFood(foodInContainer);
          actionMessage += " (Only " + String(foodInContainer) + " grams available, dispensing the remainder)";
          // After dispensing, reset the requested food to 0 in Firebase
          Firebase.RTDB.setInt(&fbdo, "/feedingRequests/requestedFood", 0);
        }
        else {
          // If the container is empty
          actionMessage = "The container is empty. No food available to dispense.";
        }
      }

      // Print the action message to serial output
      Serial.println(actionMessage);

      // Send the action message to Firebase
      Firebase.RTDB.setString(&fbdo, "/feedingStatus/actionMessage", actionMessage);
    } else {
      Serial.println("Error reading requested food quantity from Firebase: " + fbdo.errorReason());
    }
  }
}

// Function to measure distance using ultrasonic sensor
float measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = (duration / 2.0) * 0.0343; // Calculate distance in cm
  return distance;
}

// Function to map the main container distance to food amount (in grams)
int mapDistanceToFoodAmount(float distance) {
  // Assuming the container is 30 cm tall and holds 100 grams of food.
  // This is a rough mapping; you should calibrate it based on your actual container and food.
  float heightRemaining = maxContainerHeight - distance;
  int foodAmount = map(heightRemaining, 0, maxContainerHeight, 0, 100);
  return foodAmount;
}

// Function to simulate dispensing food and return a message
String dispenseFood(int amount) {
  String message = "Dispensing " + String(amount) + " grams of food.";
  
  // Simulate the food dispensing action by opening the servo (move to a dispensing position)
  foodServo.write(90);  // Move the servo to 90 degrees (or desired angle for dispensing)
  delay(1000);  // Wait for the dispensing action (you can adjust this duration)
  
  // Return the servo to its original position (to stop dispensing)
  foodServo.write(0);  // Move the servo back to 0 degrees (or original position)
  delay(500);  // Allow the servo to return to the start position
  
  // Update the food amount in the main container (decrease food level)
  mainContainerDistance += amount;  // Decrease the food level
  
  // Return a message indicating what action was taken
  return message;
}
