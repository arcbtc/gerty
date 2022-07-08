# Gerty

## LND node monitor

> <i>Join our <a href="https://t.me/makerbits">telegram support/chat</a>.</i>

Gerty will monitor your LND node is online, down or has received a transaction. 

Includes access point for easy setup!

| Gerty thinking  | Gerty connected to LND | Gerty not connected to LND |
| ------------- | ------------- | ------------- |
| ![](https://i.imgur.com/QJCcR24.gif)  | ![](https://i.imgur.com/pBZ5tcf.gif)  | ![](https://i.imgur.com/T7RISGT.gif) |

| Received a transaction  | Activate access point | Gerty access point |
| ------------- | ------------- | ------------- |
| ![](https://i.imgur.com/tctmGmz.gif)  | ![](https://i.imgur.com/rQmJQty.gif)  | ![](https://i.imgur.com/kAFxcCY.gif) |

### Hardware
M5Atom 
https://m5stack.com/products/atom-matrix-esp32-development-kit

### Software

Arduino IDE
https://www.arduino.cc/en/software

Install ESP32 boards, https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md

Install additional Arduino libraries (Sketch>Include Library>Manage Libaries),
* M5Atom 
* Adafruit_NeoMatrix
* Adafruit_NeoPixel
* ArduinoJson
* Adafruit_GFX
* FastLED
* WiFiManager

## Install instructions
Once you have all the above, download this sketch, open gerty.ino, select Tools > Board > M5 ATOM, flash to M5Atom 😘






