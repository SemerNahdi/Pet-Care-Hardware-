#include <WiFi.h>
#include <Firebase_ESP_Client.h> // Include Firebase ESP32 library

// Include the Token and RTDB helpers
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "firebase_credentials.h"
// Define your Wi-Fi credentials
#define WIFI_SSID "Wokwi-GUEST" // Replace with your SSID
#define WIFI_PASSWORD ""        // Replace with your Wi-Fi password

// Define your Firebase data object
FirebaseData fbdo;

// Firebase authentication and configuration
FirebaseAuth auth;
FirebaseConfig config;

// Sensor pins
#define TRIG_PIN 5  // HC-SR04 TRIG pin
#define ECHO_PIN 18 // HC-SR04 ECHO pin
#define LED_PIN 33  // LED pin to indicate low water level

// Variables for water level detection
long duration;
float distance;
float waterLevelThreshold = 350.0; // Threshold for low water level in cm

// Time control
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

void setup()
{
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to Wi-Fi. IP Address: ");
  Serial.println(WiFi.localIP());

  // Firebase configuration setup
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Sign up the Firebase user
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Firebase sign-up successful!");
    signupOK = true;
  }
  else
  {
    Serial.printf("Firebase sign-up failed: %s\n", config.signer.signupError.message.c_str());
  }

  // Set callback for token generation
  config.token_status_callback = tokenStatusCallback;

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // Measure water level using HC-SR04
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = (duration * 0.034) / 2; // Convert duration to cm

    Serial.print("Water level: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Check water level and update Firebase
    if (distance > waterLevelThreshold)
    {
      digitalWrite(LED_PIN, HIGH); // Turn on LED for low water level
      Serial.println("Low water level! LED ON.");
      if (Firebase.RTDB.setString(&fbdo, "/waterLevel", "low"))
      {
        Serial.println("Firebase updated: Low water level.");
      }
      else
      {
        Serial.println("Failed to update Firebase.");
        Serial.println(fbdo.errorReason());
      }
    }
    else
    {
      digitalWrite(LED_PIN, LOW); // Turn off LED for normal water level
      Serial.println("Water level OK. LED OFF.");
      if (Firebase.RTDB.setString(&fbdo, "/waterLevel", "ok"))
      {
        Serial.println("Firebase updated: Water level OK.");
      }
      else
      {
        Serial.println("Failed to update Firebase.");
        Serial.println(fbdo.errorReason());
      }
    }

    // Update the distance value to Firebase
    if (Firebase.RTDB.setFloat(&fbdo, "/distance", distance))
    {
      Serial.println("Firebase updated: Distance recorded.");
    }
    else
    {
      Serial.println("Failed to update Firebase.");
      Serial.println(fbdo.errorReason());
    }

    // Optionally, send additional test data every 15 seconds
    if (Firebase.RTDB.setInt(&fbdo, "test/int", count))
    {
      Serial.println("PASSED: Sent integer to Firebase.");
    }
    else
    {
      Serial.println("FAILED to send integer.");
      Serial.println(fbdo.errorReason());
    }

    count++;
  }
}
