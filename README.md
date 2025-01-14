# 🐾 Pet Care System: Automated Food and Water Dispensing

## 📋 Overview

The **Pet Care System** is an automated solution designed to take care of your pets by managing both food and water dispensing. Using **ESP32**, **ultrasonic sensors**, and **servo motors**, this system ensures that your pets are well-fed and hydrated. You can control and monitor the system remotely using **Firebase** for real-time updates.

---

## 🐕 Key Features

### 🍽️ Automated Food Dispensing:

- Automatically dispenses the requested amount of food into your pet’s bowl.
- **Ultrasonic sensor** monitors the amount of food remaining in the container.
- **Servo motor** is used to dispense the food when required.

### 💧 Water Level Monitoring:

- **Ultrasonic sensor** detects the water level in the pet's water bowl.
- **LED indicator** alerts you when the water level is low.
- **Firebase updates** notify you about the water level status in real-time.

---

## 🛠️ Hardware Components

- **ESP32/ESP8266**: The main controller of the system.
- **HC-SR04 Ultrasonic Sensors**: Used for measuring food and water levels.
- **Servo Motor**: Controls the dispensing of food.
- **LED**: Used to indicate the water level status (low or OK).
- **Wi-Fi**: Enables communication with Firebase.

---

## 📚 Software Libraries Used

- **ESP32Servo**: Library to control the servo motor.
- **WiFi**: Manages the Wi-Fi connection for cloud communication.
- **Firebase_ESP_Client**: Integrates Firebase for real-time data management.

---

## 🚀 Setup Instructions

1. **Create `firebase_credentials.h` File**:
   To securely store your Firebase credentials, you need to create a header file called `firebase_credentials.h` in your project. This file will contain your **API_KEY** and **DATABASE_URL**, which you will get from the Firebase console.

   ### Steps to Create the `firebase_credentials.h` File:

   - In your project folder, create a new file named `firebase_credentials.h`.

     **For example (in Windows)**:

     - Open a text editor (such as Notepad).
     - Save the file as `firebase_credentials.h` in your project folder.

   - Inside the `firebase_credentials.h` file, add the following template:

     ```cpp
     // firebase_credentials.h

     #ifndef FIREBASE_CREDENTIALS_H
     #define FIREBASE_CREDENTIALS_H

     // Replace the following values with your Firebase project details
     #define API_KEY "YOUR_API_KEY"             // Replace with your Firebase API Key
     #define DATABASE_URL "YOUR_DATABASE_URL"   // Replace with your Firebase Database URL

     #endif // FIREBASE_CREDENTIALS_H
     ```

   - **Important**: You need to replace the placeholders `"YOUR_API_KEY"` and `"YOUR_DATABASE_URL"` with the actual API Key and Database URL from your Firebase project.

   ### How to Find Your Firebase API Key and Database URL:

   - **API_KEY**: Go to the [Firebase Console](https://console.firebase.google.com/), select your project, and navigate to **Project Settings**. You will find the **Web API Key** under the **General** tab.
   - **DATABASE_URL**: In the Firebase Console, under your project’s **Realtime Database** section, copy the URL from the browser’s address bar. It should look something like `https://<your-project-id>.firebaseio.com/`.

2. **Include `firebase_credentials.h` in Your Code**:
   After creating the `firebase_credentials.h` file and filling it with your credentials, include it in your main code. This allows you to securely link your Firebase project with your ESP32.

   In your Arduino sketch, add the following line at the top of the file to include the credentials header:

   ```cpp
   #include "firebase_credentials.h"  // Include your Firebase credentials
   ```

3. **Connect the hardware**:

   - **Food Dispensing**:
     - Connect the ultrasonic sensor to **TRIG_PIN 5** and **ECHO_PIN 4**.
     - Connect the servo motor to **SERVO_PIN 9**.
   - **Water Monitoring**:
     - Connect the ultrasonic sensor to **TRIG_PIN 5** and **ECHO_PIN 18**.
     - Connect the LED to **PIN 33** (or another available GPIO pin for water level indication).

4. **Configure Firebase**:

   - Create a Firebase project in the Firebase console.
   - Enable **Realtime Database** and obtain your **API_KEY** and **DATABASE_URL** as described in step 1.
   - Replace these values in the `firebase_credentials.h` file and upload your sketch to the ESP32.

5. **Upload the code**:

   - Install the required libraries: **ESP32Servo**, **WiFi**, and **Firebase_ESP_Client** using the Arduino Library Manager.
   - Upload both the **Food Dispensing** code and the **Water Monitoring** code to your ESP32.

6. **Monitor the system**:
   - Open the **Serial Monitor** in Arduino IDE to see live updates on the feeding and water status.
   - Firebase updates will reflect in the serial output as well as in your Firebase database.

---

## 📱 Firebase Data Structure

### Food Dispensing

```json
/feedingRequests/
  requestedFood: (int, grams)

/feedingStatus/
  actionMessage: (string, dispensing action message)
```

### Water Monitoring

```json
/waterLevel: (string, "low" or "ok")
/distance: (float, water level distance in cm)
```

---

## 💡 Notes & Troubleshooting

- **Wi-Fi issues?** Double-check your Wi-Fi credentials. Ensure that your ESP32 is within range of your router.
- **Servo motor not working?** Verify the wiring and test the servo with a simple sketch to ensure it functions correctly.
- **Firebase errors?** Check the **API_KEY** and **DATABASE_URL**. Ensure they are correctly entered in your code and the Firebase project is correctly set up.
- **Water Level Issues?** If the water level is not being accurately measured, verify the ultrasonic sensor’s placement and calibration.

## 📜 License

This project is open-source and licensed under the [MIT License](LICENSE).
