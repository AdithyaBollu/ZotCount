#include <Arduino.h>
#include <HttpClient.h>
#include <WiFi.h>
#include <inttypes.h>
#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

// This example downloads the URL "http://arduino.cc/"
// The following part is our Wifi credentials which were flashed to our ESP32 board
// We ended up using mobile data for this portion

char ssid[50]; // your network SSID (name)
char pass[50]; // your network password (use for WPA, or use
// as key for WEP)
const int kNetworkTimeout = 30 * 1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;
int people = 0;
int ledPin = 25;      // choose the pin for the LED
int inputPin = 26;     // choose the input pin (for PIR sensor)
int button = 22;
int previousPirVal = LOW;
int pirVal;
int buttonVal;
bool printOut = true;

int button2 = 17;
int buttonVal2;

// The following portion is from Lab 4 and it connects to the Wifi by retrieving the SSID and password
void nvs_access() {
  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
    err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  // Open
  Serial.printf("\n");
  Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  } else {
    Serial.printf("Done\n");
    Serial.printf("Retrieving SSID/PASSWD\n");
    size_t ssid_len;
    size_t pass_len;
    err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
    err |= nvs_get_str(my_handle, "pass", pass, &pass_len);
    switch (err) {
      case ESP_OK:
        Serial.printf("Done\n");
        Serial.printf("SSID = %s\n", ssid);
        Serial.printf("PASSWD = %s\n", pass);
        break;
      case ESP_ERR_NVS_NOT_FOUND:
        Serial.printf("The value is not initialized yet!\n");
        break;
      default:
      Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
  }
  // Close
  nvs_close(my_handle);
}


void setup() {
  Serial.begin(9600);
  // Retrieve SSID/PASSWD from flash before anything else
  nvs_access();
  // We start by connecting to a WiFi network
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input
  pinMode(button, INPUT);
  pinMode(button2, INPUT);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
}



void loop() {
  // put your main code here, to run repeatedly:
  
  // Reading values from both buttons as well as the PIR sensor
  pirVal = digitalRead(inputPin);
  buttonVal = digitalRead(button);
  buttonVal2 = digitalRead(button2);

  // If either of the buttons senses motion, it will run the code
  if (buttonVal == HIGH || buttonVal2 == HIGH) {
    if (pirVal == HIGH)  // check if the input is HIGH
    {           
      digitalWrite(ledPin, HIGH);  // turn LED ON
      if (previousPirVal == LOW) {
        // Serial.println("Motion detected!"); // print on output change
        previousPirVal = HIGH;
      }
    }
    else {
      digitalWrite(ledPin, LOW);
      if (previousPirVal == HIGH) {
        // Serial.println("Motion ended!"); 
        previousPirVal = LOW;
        if (buttonVal == HIGH) {
          people++;
        }
        else if (buttonVal2 == HIGH) {
          people--;
        }
      }
    }
    if (millis() % 1000 == 0) {
      if (printOut) {
        Serial.print("People: ");
        Serial.print(people);
        Serial.println();
        printOut = false;
      } 
    }
    if (millis() % 1000 == 1) {
      printOut = true;
    }
  }
    

  // The following portion is also taken from Lab 4 and sends the information to our AWS EC2 instance.
  int err = 0;
  WiFiClient c;
  HttpClient http(c);
  String values = "/?var=" + String(people);
  Serial.print(values);
  Serial.println();

  // Sending the actual value of the people count that was retrieved (This is meant for 1 bus)
  err = http.get("18.227.13.52", 5000, values.c_str(), NULL);
  if (err == 0) {
    Serial.println("startedRequest ok");
    err = http.responseStatusCode();
    if (err >= 0) {
      Serial.print("Got status code: ");
      Serial.println(err);
      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get
      err = http.skipResponseHeaders();
      if (err >= 0) {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ((http.connected() || http.available()) && ((millis() - timeoutStart) < kNetworkTimeout)) {
          if (http.available()) {
            c = http.read();
            // Print out this character
            Serial.print(c);
            bodyLen--;
            // We read something, reset the timeout counter
            timeoutStart = millis();
          } else {
            // We haven't got any data, so let's pause to allow some to
            // arrive
            delay(kNetworkDelay);
          }
        }
        } else {
          Serial.print("Failed to skip response headers: ");
          Serial.println(err);
        }
    } else {
        Serial.print("Getting response failed: ");
        Serial.println(err);
    }
  } else {
      Serial.print("Connect failed: ");
      Serial.println(err);
    }
  http.stop();
}
