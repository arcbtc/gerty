
//////////////LOAD LIBRARIES////////////////

#include "M5Atom.h"

#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>

#include "FS.h"
#include <WiFiManager.h> 
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include "SPIFFS.h"

/////////////////SOME DEFINES///////////////////

#define LED_PIN    27
#define LED_COUNT 25
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(5, 5, LED_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);
  
/////////////////SOME VARIABLES///////////////////

int new_balance = 0;
int balance = 0;
int new_confirmed_balance = 0;
int confirmed_balance = 0;
bool synced_to_chain = false;
bool first_check = true;
bool first_check1 = true;
const char* lnd_check;

char lnd_server[40];
char lnd_port[6]  = "443";
char lnd_macaroon[500] = "";
char static_ip[16] = "10.0.1.56";
char static_gw[16] = "10.0.1.1";
char static_sn[16] = "255.255.255.0";

bool down = false;
bool synching = false;
bool shouldSaveConfig = false;

const char* spiffcontent = "";
String spiffing; 

uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0, 255,0);
uint32_t blue = strip.Color(0,0,255);
uint32_t white = strip.Color(255,255,255);

/////////////////////IMAGES////////////////////////

int smile[] = {
  0,0,0,0,0,
  0,1,0,1,0,
  0,0,0,0,0,
  1,0,0,0,1,
  0,1,1,1,0,
};

int frown[] = {
  0,0,0,0,0,
  0,1,0,1,0,
  0,0,0,0,0,
  0,1,1,1,0,
  1,0,0,0,1,
};

int sad[] = {
  0,0,0,0,0,
  1,1,0,1,1,
  0,0,0,0,0,
  0,1,1,1,0,
  1,0,0,0,1,
};

int serious[] = {
  0,0,0,0,0,
  0,1,0,1,0,
  0,0,0,0,0,
  1,1,1,1,1,
  0,0,0,0,0,
};

int thinking0[] = {
  0,0,0,0,0,
  0,1,0,1,0,
  0,0,0,0,0,
  1,0,0,0,0,
  0,1,1,0,0,
};

int thinking1[] = {
  0,0,0,0,0,
  0,1,0,1,0,
  0,0,0,0,0,
  0,0,0,0,1,
  0,0,1,1,0,
};

int arrow0[] = {
  0,1,1,1,0,
  0,0,1,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
};

int arrow1[] = {
  0,0,1,0,0,
  0,1,1,1,0,
  0,0,1,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
};

int arrow2[] = {
  0,0,1,0,0,
  0,0,1,0,0,
  0,1,1,1,0,
  0,0,1,0,0,
  0,0,0,0,0,
};

int arrow3[] = {
  0,0,0,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,1,1,1,0,
  0,0,1,0,0,
};


/////////////////////SETUP////////////////////////

void setup() {
  pinMode (25, OUTPUT);
  digitalWrite(25, LOW); 
  Serial.begin(115200);
  
  M5.begin(true, false, false);
  
  strip.begin();
  strip.clear();
  strip.show();
  strip.setBrightness(200);

  matrix.begin();
  matrix.clear();
  matrix.setTextWrap(false);
  matrix.setBrightness(200);

// START PORTAL 

  processing_ux();
  gerty_portal();
  
}
int x = matrix.width();
int pass = 0;

///////////////////MAIN LOOP//////////////////////

void loop() {


  getinfo();
  if(down){
  getinfo();
  delay(5000);
  }
  delay(5000);
  getonchainbalance();
  delay(5000);
  getlnbalance();
  delay(5000);
}


//////////////////DISPLAY///////////////////


void up_ux(){
    show_image(smile, green, 2);
    delay(2000);
}

void down_ux(){
  for (int i = 0; i < 3; i++) {
    show_image(sad, red, 2);
    delay(500);
    show_image(frown, red, 2);
    delay(500);
  }
}

void processing_ux(){
  for (int i = 0; i < 3; i++) {
    show_image(thinking0, blue, 2);
    delay(500);
    show_image(thinking1, blue, 2);
    delay(500);
  }
  show_image(smile, blue, 2);
  delay(500);
}

void synching_ux(){
  for (int i = 0; i < 5; i++) {
    show_image(arrow0, blue, 2);
    delay(300);
    show_image(arrow1, blue, 2);
    delay(300);
    show_image(arrow2, blue, 2);
    delay(300);
    show_image(arrow3, blue, 2);
    delay(300);
  }
}

void transaction_ux(int wait) {
  digitalWrite(25, HIGH); 
  for(int i=0; i<5; i++) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { 
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); 
    delay(wait);
  }
  }
  
  strip.clear();
  digitalWrite(25, LOW); 
}

//////DISPLAY HELPERS


void text_print(String text, uint32_t color){
  int no_chars = text.length() * 6;
  for(int i=0; i<no_chars+1; i++) {
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(text.c_str());
  if(--x < -no_chars) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(color);
  }
  matrix.setRotation(1);
  matrix.show();
  delay(100);
  }
  matrix.clear();
}

void show_image(int* pic, uint32_t color, int wait) {
  for(int i=0; i<25; i++) { 
    if (pic[i] == 1){
     strip.setPixelColor(i, color);
     strip.show();
     delay(wait);  
    }
    else{
     strip.setPixelColor(i, strip.Color(0,0,0));
     strip.show();
     delay(wait);       
    }
  }
}

//////////////////NODE CALLS///////////////////

void getinfo() {
  WiFiClientSecure client;
  const char* lndserver = lnd_server;
  const char* macaroon = lnd_macaroon;
  int lndport = atoi( lnd_port );

  Serial.println(lndserver);
  Serial.println(lndport);
  if (!client.connect(lndserver, lndport)){
    down_ux();
    down = true;
    return;   
  }
  client.print(String("GET ")+ "https://" + lndserver +":"+ lndport + "/v1/getinfo HTTP/1.1\r\n" +
                 "Host: "  + lndserver +":"+ lndport +"\r\n" +
                 "User-Agent: ESP322\r\n" +
                 "Grpc-Metadata-macaroon:" + macaroon + "\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Connection: close\r\n" +
                 "\n");
    String line = client.readStringUntil('\n');
    while (client.connected()) {
     String line = client.readStringUntil('\n');
     if (line == "\r") {    
       break;
     }
    }
    String content = client.readStringUntil('\n');
    client.stop();
    Serial.print(content);
    const size_t capacity = JSON_OBJECT_SIZE(3) + 620;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, content); 
    if (!doc["alias"]){
      down_ux();
      down = true;
      return;
      }
    
    lnd_check = doc["alias"];
    synced_to_chain = doc["synced_to_chain"]; 
    
    if(synced_to_chain == true){
      up_ux();
      down = false;
    }
    else{
      synching_ux();
      down = true;
    }
}

void getonchainbalance() {
  WiFiClientSecure client;
  const char* lndserver = lnd_server;
  const char* macaroon = lnd_macaroon;
  int lndport = atoi( lnd_port );
  if (!client.connect(lndserver, lndport)){
    down_ux();
    down = true;
    return;   
  }
  client.print(String("GET ")+ "https://" + lndserver +":"+ lndport + "/v1/balance/blockchain HTTP/1.1\r\n" +
                 "Host: "  + lndserver +":"+ lndport +"\r\n" +
                 "User-Agent: ESP322\r\n" +
                 "Grpc-Metadata-macaroon:" + macaroon + "\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Connection: close\r\n" +
                 "\n");
    String line = client.readStringUntil('\n');
    while (client.connected()) {
     String line = client.readStringUntil('\n');
     if (line == "\r") {    
       break;
     }
    }
    String content = client.readStringUntil('\n');
    client.stop();
    const size_t capacity = JSON_OBJECT_SIZE(3) + 620;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, content);
    if (!doc["confirmed_balance"]){
      down_ux();
      down = true;
      return;
    }
      
    new_confirmed_balance  = doc["confirmed_balance"];
    if(new_confirmed_balance > confirmed_balance){
      if(first_check == false){
        confirmed_balance = new_confirmed_balance;
        Serial.println(new_confirmed_balance);
        transaction_ux(2);
      }
      else{
        confirmed_balance = new_confirmed_balance;
        first_check = false;
      }
    }
    Serial.println("confirmed_balance " + String(confirmed_balance));
    Serial.println("new_confirmed_balance " + String(new_confirmed_balance));
}


void getlnbalance() {
  WiFiClientSecure client;
  const char* lndserver = lnd_server;
  const char* macaroon = lnd_macaroon;
  int lndport = atoi( lnd_port );
  if (!client.connect(lndserver, lndport)){
    down_ux();
    down = true;
    return;   
  }
  client.print(String("GET ")+ "https://" + lndserver +":"+ lndport + "/v1/balance/channels HTTP/1.1\r\n" +
                 "Host: "  + lndserver +":"+ lndport +"\r\n" +
                 "User-Agent: ESP322\r\n" +
                 "Grpc-Metadata-macaroon:" + macaroon + "\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Connection: close\r\n" +
                 "\n");
    String line = client.readStringUntil('\n');
    while (client.connected()) {
     String line = client.readStringUntil('\n');
     if (line == "\r") {    
       break;
     }
    }
    String content = client.readStringUntil('\n');
    client.stop();
    const size_t capacity = JSON_OBJECT_SIZE(3) + 620;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, content);

    if (!doc["balance"]){
      down_ux();
      down = true;
      return;
    }
    
    new_balance = doc["balance"];
    if(new_balance > balance){
      if(first_check1 == false){
        balance = new_balance;
        transaction_ux(2);
      }
      else{
        balance = new_balance;
        first_check1 = false;
      }
    }
    Serial.println("balance " + String(balance));
    Serial.println("new_balance " + String(new_balance));
}


void gerty_portal(){

  WiFiManager wm;
  Serial.println("mounting FS...");
  while(!SPIFFS.begin(true)){
    Serial.println("failed to mount FS");
    delay(200);
   }

//CHECK IF RESET IS TRIGGERED/WIPE DATA
  for (int i = 0; i <= 100; i++) {
    if (M5.Btn.wasPressed()){
    show_image(smile, white, 2);
    delay(1000);
    File file = SPIFFS.open("/config.txt", FILE_WRITE);
    file.print("placeholder");
    wm.resetSettings();
    i = 100;
    processing_ux();
    }
    delay(50);
    M5.update();
  }

//MOUNT FS AND READ CONFIG.JSON
  File file = SPIFFS.open("/config.txt");
  
  spiffing = file.readStringUntil('\n');
  spiffcontent = spiffing.c_str();
  DynamicJsonDocument json(1024);
  deserializeJson(json, spiffcontent);
  if(String(spiffcontent) != "placeholder"){
    strcpy(lnd_server, json["lnd_server"]);
    strcpy(lnd_port, json["lnd_port"]);
    strcpy(lnd_macaroon, json["lnd_macaroon"]);
  }

//ADD PARAMS TO WIFIMANAGER
  wm.setSaveConfigCallback(saveConfigCallback);
  
  WiFiManagerParameter custom_lnd_server("server", "LND server", lnd_server, 40);
  WiFiManagerParameter custom_lnd_port("port", "LND port (Default 443)", lnd_port, 6);
  WiFiManagerParameter custom_lnd_macaroon("macaroon", "LND readonly macaroon", lnd_macaroon, 500);
  wm.addParameter(&custom_lnd_server);
  wm.addParameter(&custom_lnd_port);
  wm.addParameter(&custom_lnd_macaroon);
  
//IF RESET WAS TRIGGERED, RUN PORTAL AND WRITE FILES
  if (!wm.autoConnect("GERTY :)", "password1")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  Serial.println("connected :)");
  strcpy(lnd_server, custom_lnd_server.getValue());
  strcpy(lnd_port, custom_lnd_port.getValue());
  strcpy(lnd_macaroon, custom_lnd_macaroon.getValue());
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument json(1024);
    json["lnd_server"] = lnd_server;
    json["lnd_port"]   = lnd_port;
    json["lnd_macaroon"]   = lnd_macaroon;

    File configFile = SPIFFS.open("/config.txt", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
      }
      serializeJsonPretty(json, Serial);
      serializeJson(json, configFile);
      configFile.close();
      shouldSaveConfig = false;
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.subnetMask());
}

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
