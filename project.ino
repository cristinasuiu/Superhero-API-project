#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif

String ssid = "";
String password = "";
String teamId = "A06";
bool connected_WiFi;
#define CONNECT_TIMEOUT 15000 // milliseconds
long connectStart = 0;
bool connected;

#define btcServerName "SUPERHERO_API_A06"
BluetoothSerial SerialBT;

String data = "";

void deviceConnected(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    Serial.println("Device is Connected");
    connected = true;
  }

  if (event == ESP_SPP_CLOSE_EVT) {
    Serial.println("Device is disconnected");
    connected = false;
  }
}

void receivedData() {
  while (SerialBT.available()) {
    data = SerialBT.readStringUntil('\n');
  }
  Serial.println(data); 
  
  String url = "http://proiectia.bogdanflorea.ro/api/superhero-api/characters";
  String url1 = "http://proiectia.bogdanflorea.ro/api/superhero-api/character?id=";
  String api_key = "";

  DynamicJsonDocument jsonDoc(2048);
  DeserializationError error = deserializeJson(jsonDoc, data);
  if (error) {
    Serial.print("JSON deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }

  String message_action = jsonDoc["action"];

  if (message_action == "getNetworks") {
    String message_teamId = jsonDoc["teamId"];
    teamId = message_teamId;
  
    Serial.println("Start scan");
    int networksFound = WiFi.scanNetworks();
    Serial.println("Scan complete");

    if (networksFound == 0) {
      Serial.println("No networks found");
    } else {
      Serial.print(networksFound);
      Serial.println(" networks found");

      for (int i = 0; i < networksFound; i++) {
        DynamicJsonDocument network(1024);
        network["ssid"] = WiFi.SSID(i);
        network["strength"] = WiFi.RSSI(i);
        network["encryption"] = WiFi.encryptionType(i);
        network["teamId"] = teamId;
        String jsonString;
        serializeJson(network, jsonString);
        Serial.println(jsonString);
        SerialBT.println(jsonString);
        delay(100);
      }
    }
  } else if (message_action == "connect") {
    String message_ssid = jsonDoc["ssid"];
    String message_password = jsonDoc["password"];
    ssid = message_ssid;
    password = message_password;

    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.println("Connecting");
    
    connectStart = millis();
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
      if (millis() - connectStart > CONNECT_TIMEOUT) {
        break;
      }
    }
    Serial.println("");
    connected_WiFi = false;

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Connection failed");
    } else {
      Serial.print("Connected to network: ");
      Serial.println(WiFi.SSID());
      Serial.print("Local IP address: ");
      Serial.println(WiFi.localIP());
      connected_WiFi = true;
    }
    
    DynamicJsonDocument Connected(200);
    Connected["ssid"] = ssid;
    Connected["connected"] = connected_WiFi;
    Connected["teamId"] = teamId;
    String output1;
    serializeJson(Connected, output1);
    SerialBT.println(output1);
   //parte network
  
   } else if (message_action == "getData") {
    Serial.println("Retrieving data from server...");

    HTTPClient http;
    http.begin(url);
    http.setConnectTimeout(3000); 
    http.setTimeout(3000); 

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument jsonDoc(15000);
      DeserializationError error = deserializeJson(jsonDoc, payload);
      if (error) {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        return;
      }

      JsonArray recordsArray = jsonDoc.as<JsonArray>();
      DynamicJsonDocument jsonDoc2(15000);

      for (JsonObject record : recordsArray) {
        String name = record["name"];
        String id = record["id"];
        String image = record["imageUrl"];

        jsonDoc2["name"] = name;
        jsonDoc2["id"] = id;
        jsonDoc2["image"] = image;
        jsonDoc2["teamId"] = teamId;

        String jsonResponse2;
        serializeJson(jsonDoc2, jsonResponse2);
        SerialBT.println(jsonResponse2);
      }
    } else {
      Serial.println("Failed to retrieve data from server");
}
http.end();
} else if (message_action == "getDetails") {
    String selected_id = jsonDoc["id"];
    Serial.print(F("ID Details: "));
    Serial.println(selected_id);

    Serial.println("Sending GET request to remote server...");

    HTTPClient http1;
    String full_url = String(url1) + String(selected_id);
    Serial.println(full_url);

    http1.begin(full_url);
    http1.setConnectTimeout(30000); 
    http1.setTimeout(30000);
    int httpResponseCode1 = http1.GET();

    if (httpResponseCode1 > 0) {
        String payload1 = http1.getString();
        DynamicJsonDocument doc3(4096);
        deserializeJson(doc3, payload1);

        String image = doc3["imageUrl"].as<String>();

        JsonObject powerstats = doc3["powerstats"];
        JsonObject biography = doc3["biography"];
        JsonObject appearance = doc3["appearance"];
        JsonObject work = doc3["work"];
        JsonObject connections = doc3["connections"];

        String id = doc3["id"];
        String name = doc3["name"];
        int intelligence = powerstats["intelligence"];
        int strength = powerstats["strength"];
        int speed = powerstats["speed"];
        int durability = powerstats["durability"];
        int power = powerstats["power"];
        int combat = powerstats["combat"];
        String full_name = biography["full-name"];
        String alter_egos = biography["alter-egos"];
        JsonArray aliasesArray = biography["aliases"];
        String aliases;
        for (JsonVariant alias : aliasesArray) {
            if (!aliases.isEmpty()) {
                aliases += ", ";
            }
            aliases += alias.as<String>();
        }
        String place_of_birth = biography["place-of-birth"];
        String first_appearance = biography["first-appearance"];
        String publisher = biography["publisher"];
        String alignment = biography["alignment"];
        String gender = appearance["gender"];
        String race = appearance["race"];
        JsonArray heightArray = appearance["height"];
        String height = heightArray[0].as<String>() + " (" + heightArray[1].as<String>() + ")";
        JsonArray weightArray = appearance["weight"];
        String weight = weightArray[0].as<String>() + " (" + weightArray[1].as<String>() + ")";
        String eye_color = appearance["eye-color"];
        String hair_color = appearance["hair-color"];
        String occupation = work["occupation"];
        String base = work["base"];
        String group_affiliation = connections["group-affiliation"];
        String relatives = connections["relatives"];
        Serial.println(image);

        DynamicJsonDocument jsonDoc3(4096);
        jsonDoc3["id"] = id;
        jsonDoc3["name"] = name;
        jsonDoc3["powerstats"] = powerstats;
        jsonDoc3["biography"] = biography;
        jsonDoc3["appearance"] = appearance;
        jsonDoc3["work"] = work;
        jsonDoc3["connections"] = connections;
        jsonDoc3["imageUrl"] = image;
        jsonDoc3["teamId"] = teamId;
        jsonDoc3["description"] = 
    "ID: " + String(id) + "\n" +
    "Name: " + String(name) + "\n" +
    "Powerstats:\n" +
    "  Intelligence: " + String(intelligence) + "\n" +
    "  Strength: " + String(strength) + "\n" +
    "  Speed: " + String(speed) + "\n" +
    "  Durability: " + String(durability) + "\n" +
    "  Power: " + String(power) + "\n" +
    "  Combat: " + String(combat) + "\n" +
    "Biography:\n" +
    "  Full Name: " + String(full_name) + "\n" +
    "  Alter Egos: " + String(alter_egos) + "\n" +
    "  Aliases: " + String(aliases) + "\n" +
    "  Place of Birth: " + String(place_of_birth) + "\n" +
    "  First Appearance: " + String(first_appearance) + "\n" +
    "  Publisher: " + String(publisher) + "\n" +
    "  Alignment: " + String(alignment) + "\n" +
    "Appearance:\n" +
    "  Gender: " + String(gender) + "\n" +
    "  Race: " + String(race) + "\n" +
    "  Height: " + String(height) + "\n" +
    "  Weight: " + String(weight) + "\n" +
    "  Eye-color: " + String(eye_color) + "\n" +
    "  Hair-color: " + String(hair_color) + "\n" +
    "Work:\n" +
    "  Occupation: " + String(occupation) + "\n" +
    "  Base: " + String(base) + "\n" +
    "Connections:\n" +
    "  Group-affiliation: " + String(group_affiliation) + "\n" +
    "  Relatives: " + String(relatives) + "\n";
      jsonDoc3["image"] = image;
      
        String jsonResponse3;
        serializeJson(jsonDoc3, jsonResponse3);
        SerialBT.println(jsonResponse3);
    } else {
        Serial.println("Failed to get details from server");
    }

    http1.end();
}
data = "";
}

void setup() {
Serial.begin(115200);
SerialBT.begin(btcServerName); 
Serial.println("The device started, now you can pair it with bluetooth!");
SerialBT.register_callback(deviceConnected);
Serial.println("The bluetooth device is ready to pair");
WiFi.mode(WIFI_STA);
WiFi.disconnect();
delay(2000);
}

void loop() {
if (SerialBT.available()) {
Serial.println("Bluetooth Active");
receivedData();}
}
