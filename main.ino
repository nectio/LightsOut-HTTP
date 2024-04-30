#include <WiFi.h>

const char * ssid = "ssid";
const char * password = "pwd";

WiFiServer server(80);

String header;

String K1Std = "off";
String K2Std = "off";
String K3Std = "off";
String dummyStd = "off";

const int K[] = {26,27,14,0};
const int S[] = {32,34,35,25,33};
int knopStd[] = {0,0,0,0,0};
bool kStd[] = {false,false,false};
int hrkKnopStd[] = {HIGH,HIGH,HIGH,HIGH,HIGH};

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 200;

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 5; i++) {
    pinMode(S[i], INPUT);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(K[i], OUTPUT);
  }

  for (int i = 0; i < 4; i++) {
    digitalWrite(K[i], LOW);
  }

  Serial.print("Aan het verbinden met ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Verbonden.");
  Serial.println("IP adres: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {

  int hdgKnopStd[5];
  for (int i = 0; i < 5; i++) {
    hdgKnopStd[i] = digitalRead(S[i]);
  }

  if (hdgKnopStd[0] != hrkKnopStd[0]) {

    if (hdgKnopStd[0] == HIGH) {
      kStd[0] = !kStd[0];
      digitalWrite(K[0], kStd[0]);
      if (kStd[0]) {
        K1Std = "on";
      } else {
        K1Std = "off";
      }
    }
  }

  hrkKnopStd[0] = hdgKnopStd[0];

  if (hdgKnopStd[1] != hrkKnopStd[1]) {

    if (hdgKnopStd[1] == HIGH) {
      kStd[1] = !kStd[1];
      digitalWrite(K[1], kStd[1]);
      if (kStd[1]) {
        K2Std = "on";
      } else {
        K2Std = "off";
      }
    }
  }

  hrkKnopStd[1] = hdgKnopStd[1];

  if (hdgKnopStd[2] != hrkKnopStd[2]) {

    if (hdgKnopStd[2] == HIGH) {
      kStd[2] = !kStd[2];
      digitalWrite(K[2], kStd[2]);
      if (kStd[2]) {
        K3Std = "on";
      } else {
        K3Std = "off";
      }
    }
  }

  hrkKnopStd[2] = hdgKnopStd[2];

  if (hdgKnopStd[3] != hrkKnopStd[3] && kStd[0] == HIGH || kStd[1] == HIGH || kStd[2] == HIGH) {
    if (hdgKnopStd[3] == HIGH) {
      kStd[2] = LOW;
      kStd[1] = LOW;
      kStd[0] = LOW;
      digitalWrite(K[2], LOW);
      digitalWrite(K[1], LOW);
      digitalWrite(K[0], LOW);
      K1Std = "off";
      K2Std = "off";
      K3Std = "off";
    }
  }
  hrkKnopStd[3] = hdgKnopStd[3];

  if (hdgKnopStd[4] != hrkKnopStd[4] && kStd[0] == LOW || kStd[1] == LOW || kStd[2] == LOW) {
    if (hdgKnopStd[4] == HIGH) {
      kStd[2] = HIGH;
      kStd[1] = HIGH;
      kStd[0] = HIGH;
      digitalWrite(K[2], HIGH);
      digitalWrite(K[1], HIGH);
      digitalWrite(K[0], HIGH);
      K1Std = "on";
      K2Std = "on";
      K3Std = "on";
    }
  }
  hrkKnopStd[4] = hdgKnopStd[4];

  WiFiClient client = server.available();

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("Nieuwe Client.");
    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Verbinding: gebroken");
            client.println();

            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              kStd[0] = !kStd[0];
              K1Std = "on";
              digitalWrite(K[0], HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              K1Std = "off";
              kStd[0] = !kStd[0];
              digitalWrite(K[0], LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              kStd[1] = !kStd[1];
              K2Std = "on";
              digitalWrite(K[1], HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              kStd[1] = !kStd[1];
              K2Std = "off";
              digitalWrite(K[1], LOW);
            } else if (header.indexOf("GET /14/on") >= 0) {
              Serial.println("GPIO 14 on");
              kStd[2] = !kStd[2];
              K3Std = "on";
              digitalWrite(K[2], HIGH);
            } else if (header.indexOf("GET /14/off") >= 0) {
              Serial.println("GPIO 14 UIT");
              kStd[2] = !kStd[2];
              K3Std = "off";
              digitalWrite(K[2], LOW);
            } else if (header.indexOf("GET /dummy/on") >= 0) {
              Serial.println("DUMMY GPIO AAN");
              dummyStd = "on";
              digitalWrite(K[3], HIGH);
            } else if (header.indexOf("GET /dummy/off") >= 0) {
              Serial.println("DUMMY GPIO UIT");
              dummyStd = "off";
              digitalWrite(K[3], LOW);
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<title>17nct ESP32 Bedieningen</title>");
            client.println("<style>html {background-color: #000012; color: #66b0ff; font-family: Helvetica; min-height: 99vh ;display: flex; flex-direction: column; overflow: hidden; text-align: center;}");
            client.println(".E1 button {color: white; font-size: 50px; background: #9bc53d; padding: 10px 20px 10px 20px; outline: none; border: none;}");
            client.println(".E1 {font-size: 40px; position: absolute; left: 25%; top: 40%; transform: translate(-50%, -50%);}");
            client.println(".E2 button {color: white; font-size: 50px; background: #9bc53d; padding: 10px 20px 10px 20px; outline: none; border: none;}");
            client.println(".E2 {font-size: 40px; position: absolute; left: 50%; top: 40%; transform: translate(-50%, -50%);}");
            client.println(".E3 button {color: white; font-size: 50px; background: #9bc53d; padding: 10px 20px 10px 20px; outline: none; border: none;}");
            client.println(".E3 {font-size: 40px; position: absolute; left: 75%; top: 40%; transform: translate(-50%, -50%);}");
            client.println(".refresh button {color: white; font-size: 50px; background: #66B0FF; padding: 10px 20px 10px 20px; outline: none; border: none;}");
            client.println(".refresh {font-size: 40px; position: absolute; left: 50%; top: 75%; transform: translate(-50%, -50%);}");
            client.println("footer {color: #d0d0fa; position: fixed; left: 0; bottom: 0; width: 100%; text-align: center; font-size:16px;}");
            client.println("@media screen and (max-width: 600px){.E1{font-size:20px;position:absolute;left:50%;top:23%;transform:translate(-50%,-50%)}.E1 button{color:#fff;font-size:25px;background-color:#9bc53d;padding:10px 20px;outline:none;border:none}.E2{font-size:20px;position:absolute;left:50%;top:46%;transform:translate(-50%,-50%)}.E2 button{color:#fff;font-size:25px;background:#9bc53d;padding:10px 20px;outline:none;border:none}.E3{font-size:20px;position:absolute;left:50%;top:68%;transform:translate(-50%,-50%)}.E3 button{color:#fff;font-size:25px;background:#9bc53d;padding:10px 20px;outline:none;border:none}.refresh{font-size:20px;position:absolute;left:50%;top:88%;transform:translate(-50%,-50%)}.refresh button{color:#fff;font-size:25px;background:#66B0FF;padding:10px 20px;outline:none;border:none}}");
            client.println("</style>");
            client.println("</head>");

            client.println("<body><h1 style=\"font-size: 32px\">ESP32 BEDIENINGEN</h1>");
            client.println("<div class=\"E1\">");
            client.println("<h1>E1</h1>");
            if (K1Std == "off") {
              client.println("<p><a href=\"/26/on\"><button id=\"E1\">UIT</button></a></p>");
              client.println("<script>document.getElementById('E1').style.background = \"#F72C25\";</script>");
            } else {
              client.println("<p><a href=\"/26/off\"><button id=\"E1\">AAN</button></a></p>");
            }
            client.println("</div>");
            client.println("<div class=\"E2\">");
            client.println("<h1>E2</h1>");
            if (K2Std == "off") {
              client.println("<p><a href=\"/27/on\"><button id=\"E2\">UIT</button></a></p>");
              client.println("<script>document.getElementById('E2').style.background = \"#F72C25\";</script>");
            } else {
              client.println("<p><a href=\"/27/off\"><button id=\"E2\">AAN</button></a></p>");
            }
            client.println("</div>");

            client.println("<div class=\"E3\">");
            client.println("<h1>E3</h1>");
            if (K3Std == "off") {
              client.println("<p><a href=\"/14/on\"><button id=\"E3\">UIT</button></a></p>");
              client.println("<script>document.getElementById('E3').style.background = \"#F72C25\";</script>");
            } else {
              client.println("<p><a href=\"/14/off\"><button id=\"E3\">AAN</button></a></p>");
            }
            client.println("</div>");

            client.println("<div class=\"refresh\">");
            if (dummyStd == "off") {
              client.println("<p><a href=\"/dummy/on\"><button id=\"dummy\">Verfris</button></a></p>");
            } else {
              client.println("<p><a href=\"/dummy/off\"><button id=\"dummy\">Verfris</button></a></p>");
            }
            client.println("</div>");
            client.println("<footer style=\"text-align: center;\">");
            client.println("<p>2024 @ 17nct</p>");
            client.println("</footer>");
            client.println("</body></html>");

            client.println();

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
  }
}
