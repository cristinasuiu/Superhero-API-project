# Superhero-API-Project  
## ESP32 Bluetooth & WiFi Controller  

A lightweight and user-friendly application for managing ESP32 connectivity via Bluetooth Serial and WiFi. This project allows the ESP32 to scan WiFi networks, connect to a network, and retrieve superhero data from an API.  

## Features  
- **Scan WiFi Networks**: Discover available networks and send results via Bluetooth.  
- **Connect to WiFi**: Connect the ESP32 to a specified WiFi network.  
- **Fetch Superhero Data**: Retrieve a list of superheroes from an API.  
- **Get Superhero Details**: Fetch detailed information about a specific superhero.  

## How It Works  

### WiFi Scanning  
1. The ESP32 scans for available WiFi networks.  
2. It sends the list of networks via Bluetooth.  

### WiFi Connection  
1. The ESP32 connects to a WiFi network based on user input.  
2. Connection status is sent via Bluetooth.  

### Superhero Data Retrieval  
1. The ESP32 fetches superhero data from a remote API.  
2. The data is sent via Bluetooth in JSON format.  
