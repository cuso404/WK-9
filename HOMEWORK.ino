#include "WiFi.h"
#include "WebServer.h"
#define led_1  12
#define led_2  14

// SSID & Password
const char* ssid = "HLM";
const char* password = "0978319358";

WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)

void setup() {
  Serial.begin(115200);
  Serial.println("Try Connecting to ");
  Serial.println(ssid);

  // Connect to your wi-fi modem
  WiFi.begin(ssid, password);

  // Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

  //建立server的路徑
  createWebServer();
  
  //server啟動
  server.begin();
  Serial.println("HTTP server started");
  pinMode(led_1,OUTPUT);
  ledcSetup(0, 5000, 8);   //頻率5000，頻道0，解析度8;
  ledcAttachPin(led_2, 0);  //將上述頻道0附加到ledPin腳位14上
}

void loop() {
  server.handleClient();  //handleClient要寫在loop
}


//----設定HTML資料----
String htmlContent = "\
  <!DOCTYPE html>\
   <html>\
    <head>\
      <style>\
        body{ display: flex; flex-direction: column;background:#EDF3DC;}\
         a {  margin: 20px; font-size: 17pt; text-decoration: none; justify-content: center; }\
         p{color: #433F5A; margin: 5px; justify-content: center; }\
         submit{margin:6px;}\
      </style>\
    </head>\
    <body>\
    <h1>Congratulations! </h1>\
    <p>You are the best God in this world, </p>\
    <p>we will give you the power to save the world,</p>\
    <p>you can choose whether to turn on the light for human. </p>\
    <br>\
    <form method='get' action='/led/on'>\
    <input type='submit' value='I WANT SAVE THE WORLD!!!'>\
      </form>\
      <br>\
     <form method='get' action='/led/off'>\
    <input type='submit' value='Who cares?'>\
      </form>\
      <br>\
      <form method='get' action='/led/setLightBrightness'>\
        <label>Brightness for Humans</label>\
        <input type='number' name='lightBrightness'><input type='submit' value='take it'>\
      </form>\
    </body>\
  </html>";

String LEDOpenPage = "\
  <!DOCTYPE html>\
   <html>\
    <head>\
      <style>\
        body{ display: flex; flex-direction: column;background:#FFF590;}\
         a { background: #CEF2F4; width: 200px; height: 50px; margin: 20px; font-size: 30pt; text-decoration: none;}\
      </style>\
    </head>\
    <body>\
      <h1>OMG~You are so kind~</h1>\
      <p>BEST GOD FOREVER!</p>\
    <form method='get' action='/'>\
    <input type='submit' value='BACK'>\
      </form>\
    </body>\
  </html>";

String LEDCLOSEPage = "\
  <!DOCTYPE html>\
   <html>\
    <head>\
      <style>\
        body{ display: flex; flex-direction: column;background:#433C58;}\
        a { background: #CEF2F4; width: 200px; height: 50px; margin: 20px; font-size: 30pt; text-decoration: none;}\
        h1{color:#FFFFFF;}\
        p{color: #FFFFFF; margin: 5px; justify-content: center; }\
      </style>\
    </head>\
    <body>\
      <h1>DARKSIDE</h1>\
      <p>OH YEAH IS DARK~</p>\
      <p>Human beings are not qualified to possess the light of God</p>\
      <form method='get' action='/'>\
    <input type='submit' value='BACK'>\
      </form>\
    </body>\
  </html>";
  
//------設定路徑------
void createWebServer()
{
  //初始頁面
  server.on("/", []() {
      server.send(200, "text/html", htmlContent);
  });
  server.on("/led/on", []() {
    Serial.println("LED On");
    digitalWrite(led_1,HIGH);
    //add your code here
    
    String response = "{\"LED On Success\"}";
    server.send(200, "text/html", LEDOpenPage);
  });
  server.on("/led/off", []() {
    Serial.println("LED Off");

    //add your code here
     digitalWrite(led_1,LOW);
    String response = "{\"LED Off Success\"}";
    server.send(200, "text/html", LEDCLOSEPage);
  });
  server.on("/led/setLightBrightness", []() {
    //以server.arg&引數名稱，取得form傳來的資料
    int lightBrightness = server.arg("lightBrightness").toInt();
    //判斷條件    
    if(lightBrightness>=0 && lightBrightness<=1024){
      Serial.print("Set Brightness: ");
      Serial.println(lightBrightness);

      //add your code here
      ledcWrite(0, lightBrightness);  //頻道0
      
      String response = "Set Brightness: " + (String)lightBrightness;
      server.send(200, "application/json", response);
    } 
    else {
      String response = "{\"Error\":\"over range 0-1024\"}";
      server.send(404, "application/json", response);
    }
  });
}
