#include <WiFi.h>

const char * ssid = "ssid";
const char * password = "pwd";
char ingang;

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
  pinMode(2, OUTPUT);

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
  Serial.print("IP adres: \033[35m");
  Serial.println(WiFi.localIP());
  Serial.println("\033[39;49mAlles OK! Druk op H voor hulpmenu.");
  rdy();
  server.begin();
}

void loop() {

  if (Serial.available() > 0) {
    char ingang = Serial.read();
    cliHandler(ingang);
  }

  int hdgKnopStd[5];
  for (int i = 0; i < 5; i++) {
    hdgKnopStd[i] = digitalRead(S[i]);
  }

  if (hdgKnopStd[0] != hrkKnopStd[0]) {

    if (hdgKnopStd[0] == HIGH) {
      Serial.println("S1");
      blink();
      kStd[0] = !kStd[0];
      digitalWrite(K[0], kStd[0]);
      if (kStd[0]) {
        Serial.println("\033[32;49m[S1] E1 AAN");
        K1Std = "on";
        rdy();
      } else {
        Serial.println("\033[31;49m[S1] E1 UIT");
        K1Std = "off";
        rdy();
      }
    }
  }

  hrkKnopStd[0] = hdgKnopStd[0];

  if (hdgKnopStd[1] != hrkKnopStd[1]) {

    if (hdgKnopStd[1] == HIGH) {
      Serial.println("S2");
      blink();
      kStd[1] = !kStd[1];
      digitalWrite(K[1], kStd[1]);
      if (kStd[1]) {
        Serial.println("\033[32;49m[S2] E2 AAN");
        K2Std = "on";
        rdy();
      } else {
        Serial.println("\033[31;49m[S2] E2 UIT");
        K2Std = "off";
        rdy();
      }
    }
  }

  hrkKnopStd[1] = hdgKnopStd[1];

  if (hdgKnopStd[2] != hrkKnopStd[2]) {

    if (hdgKnopStd[2] == HIGH) {
      Serial.println("S3");
      blink();
      kStd[2] = !kStd[2];
      digitalWrite(K[2], kStd[2]);
      if (kStd[2]) {
        Serial.println("\033[32;49m[S3] E3 AAN");
        K3Std = "on";
        rdy();
      } else {
        Serial.println("\033[31;49m[S3] E3 UIT");
        K3Std = "off";
        rdy();
      }
    }
  }

  hrkKnopStd[2] = hdgKnopStd[2];

  if (hdgKnopStd[3] != hrkKnopStd[3] && kStd[0] == HIGH || kStd[1] == HIGH || kStd[2] == HIGH) {
    if (hdgKnopStd[3] == HIGH) {
      Serial.println("S4");
      kStd[2] = LOW;
      kStd[1] = LOW;
      kStd[0] = LOW;
      digitalWrite(K[2], LOW);
      digitalWrite(K[1], LOW);
      digitalWrite(K[0], LOW);
      K1Std = "off";
      K2Std = "off";
      K3Std = "off";
      Serial.println("\033[31;49m[S4] Alle lampen UIT");
      rdy();
    }
    hrkKnopStd[3] = hdgKnopStd[3];
  }

  if (hdgKnopStd[4] != hrkKnopStd[4] && kStd[0] == LOW || kStd[1] == LOW || kStd[2] == LOW) {
    if (hdgKnopStd[4] == HIGH) {
      Serial.println("S5");
      kStd[2] = HIGH;
      kStd[1] = HIGH;
      kStd[0] = HIGH;
      digitalWrite(K[2], HIGH);
      digitalWrite(K[1], HIGH);
      digitalWrite(K[0], HIGH);
      K1Std = "on";
      K2Std = "on";
      K3Std = "on";
      Serial.println("\033[32;49m[S5] Alle lampen AAN");
      rdy();
    }
    hrkKnopStd[4] = hdgKnopStd[4];
  }

  WiFiClient client = server.available();

  if (client) {
    IPAddress ip = client.remoteIP();
    currentTime = millis();
    previousTime = currentTime;

    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        // Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            if (header.indexOf("GET /26/aan") >= 0) {
              Serial.print("\033[35;49;1m[HTTP GET /26/AAN @ ");
              Serial.print(ip);
              Serial.println("]");
              Serial.println("\033[35;49;1m[HTTP]\033[32;49m E1 AAN");
              rdy();
              kStd[0] = !kStd[0];
              K1Std = "on";
              digitalWrite(K[0], HIGH);
              netBlink();
            } else if (header.indexOf("GET /26/uit") >= 0) {
              Serial.print("\033[35;49;1m[HTTP GET /26/uit @ ");
              Serial.print(ip);
              Serial.println("]");
              Serial.println("\033[35;49;1m[HTTP]\033[31;49m E1 UIT");
              rdy();
              K1Std = "off";
              kStd[0] = !kStd[0];
              digitalWrite(K[0], LOW);
              netBlink();
            } else if (header.indexOf("GET /27/aan") >= 0) {
              Serial.print("\033[35;49;1m[HTTP GET @ /27/aan ");
              Serial.print(ip);
              Serial.println("]");
              Serial.println("\033[35;49;1m[HTTP]\033[32;49m E2 AAN");
              rdy();
              kStd[1] = !kStd[1];
              K2Std = "on";
              digitalWrite(K[1], HIGH);
              netBlink();
            } else if (header.indexOf("GET /27/uit") >= 0) {
              Serial.print("\033[35;49;1m[HTTP GET /27/uit @ ");
              Serial.print(ip);
              Serial.println("]");
              Serial.println("\033[35;49;1m[HTTP]\033[31;49m E2 UIT");
              rdy();
              kStd[1] = !kStd[1];
              K2Std = "off";
              digitalWrite(K[1], LOW);
              netBlink();
            } else if (header.indexOf("GET /14/aan") >= 0) {
              Serial.print("\033[35;49;1m[HTTP GET /14/aan @ ");
              Serial.print(ip);
              Serial.println("]");
              Serial.println("\033[35;49;1m[HTTP]\033[32;49m E3 AAN");
              rdy();
              kStd[2] = !kStd[2];
              K3Std = "on";
              digitalWrite(K[2], HIGH);
              netBlink();
            } else if (header.indexOf("GET /14/uit") >= 0) {
              Serial.print("\033[35;49;1m[HTTP GET /14/uit @ ");
              Serial.print(ip);
              Serial.println("]");
              Serial.println("\033[35;49;1m[HTTP]\033[31;49m E3 UIT");
              rdy();
              kStd[2] = !kStd[2];
              K3Std = "off";
              digitalWrite(K[2], LOW);
              netBlink();
            } else if (header.indexOf("GET /dummy/aan") >= 0) {
              Serial.print("\033[35;49;1m[HTTP VERFRIS @ ");
              Serial.print(ip);
              Serial.println("]");
              rdy();
              dummyStd = "on";
              digitalWrite(K[3], HIGH);
            } else if (header.indexOf("GET /dummy/uit") >= 0) {
              Serial.print("\033[35;49;1m[HTTP VERFRIS @ ");
              Serial.print(ip);
              Serial.println("]");
              rdy();
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
              client.println("<p><a href=\"/26/aan\"><button id=\"E1\">UIT</button></a></p>");
              client.println("<script>document.getElementById('E1').style.background = \"#F72C25\";</script>");
            } else {
              client.println("<p><a href=\"/26/uit\"><button id=\"E1\">AAN</button></a></p>");
            }
            client.println("</div>");
            client.println("<div class=\"E2\">");
            client.println("<h1>E2</h1>");
            if (K2Std == "off") {
              client.println("<p><a href=\"/27/aan\"><button id=\"E2\">UIT</button></a></p>");
              client.println("<script>document.getElementById('E2').style.background = \"#F72C25\";</script>");
            } else {
              client.println("<p><a href=\"/27/uit\"><button id=\"E2\">AAN</button></a></p>");
            }
            client.println("</div>");

            client.println("<div class=\"E3\">");
            client.println("<h1>E3</h1>");
            if (K3Std == "off") {
              client.println("<p><a href=\"/14/aan\"><button id=\"E3\">UIT</button></a></p>");
              client.println("<script>document.getElementById('E3').style.background = \"#F72C25\";</script>");
            } else {
              client.println("<p><a href=\"/14/uit\"><button id=\"E3\">AAN</button></a></p>");
            }
            client.println("</div>");

            client.println("<div class=\"refresh\">");
            if (dummyStd == "off") {
              client.println("<p><a href=\"/dummy/aan\"><button id=\"dummy\">Verfris</button></a></p>");
            } else {
              client.println("<p><a href=\"/dummy/uit\"><button id=\"dummy\">Verfris</button></a></p>");
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

void cliHandler(char ingang) {
  if (ingang == '1') {
    Serial.println(ingang);
    kStd[0] = !kStd[0];
    digitalWrite(K[0], kStd[0]);
    if (kStd[0]) {
      Serial.println("\033[32;49m[1] E1 AAN");
      K1Std = "on";
      rdy();
    } else {
      Serial.println("\033[31;49m[1] E1 UIT");
      K1Std = "off";
      rdy();
    }
  }

  if (ingang == '2') {
    Serial.println(ingang);
    kStd[1] = !kStd[1];
    digitalWrite(K[1], kStd[1]);
    if (kStd[1]) {
      Serial.println("\033[32;49m[2] E2 AAN");
      K2Std = "on";
      rdy();
    } else {
      Serial.println("\033[31;49m[2] E2 UIT");
      K2Std = "off";
      rdy();
    }
  }

  if (ingang == '3') {
    Serial.println(ingang);
    kStd[2] = !kStd[2];
    digitalWrite(K[2], kStd[2]);
    if (kStd[2]) {
      Serial.println("\033[32;49m[3] E3 AAN");
      K3Std = "on";
      rdy();
    } else {
      Serial.println("\033[31;49m[3] E3 UIT");
      K3Std = "off";
      rdy();
    }
  }

  if (ingang == '4' || ingang == '0') {
    Serial.println(ingang);
    kStd[2] = LOW;
    kStd[1] = LOW;
    kStd[0] = LOW;
    digitalWrite(K[2], LOW);
    digitalWrite(K[1], LOW);
    digitalWrite(K[0], LOW);
    K1Std = "off";
    K2Std = "off";
    K3Std = "off";
    Serial.println("\033[31;49m[4/0] Alle lampen UIT");
    rdy();
  }

  if (ingang == '5') {
    Serial.println(ingang);
    kStd[2] = HIGH;
    kStd[1] = HIGH;
    kStd[0] = HIGH;
    digitalWrite(K[2], HIGH);
    digitalWrite(K[1], HIGH);
    digitalWrite(K[0], HIGH);
    K1Std = "on";
    K2Std = "on";
    K3Std = "on";
    Serial.println("\033[32;49m[5] Alle lampen AAN");
    rdy();
  }

  if (ingang == 'h' || ingang == 'H') {
    Serial.println(ingang);
    Serial.println("\033[91;49m========== HELPMENU ==========");
    Serial.println("");
    Serial.println("\033[91;49m==== ALGEMENE BEDIENINGEN ====");
    Serial.println("\033[0m[1] \033[32;49mE1 AAN \033[0m/\033[31;49m UIT");
    Serial.println("\033[0m[2] \033[32;49mE2 AAN \033[0m/\033[31;49m UIT");
    Serial.println("\033[0m[3] \033[32;49mE3 AAN \033[0m/\033[31;49m UIT");
    Serial.println("\033[0m[0 / 4] \033[31;49mAlle lampen UIT");
    Serial.println("\033[0m[5] \033[32;49mAlle lampen AAN");
    Serial.println("");
    Serial.println("\033[91;49m==== OPDRACHTREGELINTERFACE BEDIENINGEN ====");
    Serial.println("\033[0m[C] Opdrachtregelinterface wissen");
    Serial.println("[H] Toon deze hulpmenu");
    Serial.println("[A] Toon IP adres");
    Serial.println("[I] Informatie");
    rdy();
  }

  if (ingang == 'c' || ingang == 'C') {
    Serial.print("[\033[2J\033[H");
    rdy();
  }

  if (ingang == 'i' || ingang == 'I') {
    Serial.println(ingang);
    Serial.println("\033[39;49mLights Out V3");
    Serial.println("CLI versie 4");
    Serial.println("\033[39;49mGemaakt door \033[34;49;1m17\033[36;49;1mnct \033[0m@ 2024");
    rdy();
  }

  if (ingang == 'a' || ingang == 'A') {
    Serial.println(ingang);
    Serial.print("\033[35m");
    Serial.println(WiFi.localIP());
    rdy();
  }

}
void rdy() {
  Serial.print("\033[39;49m>");
}

void blink() {
  digitalWrite(2, HIGH);
  delay(50);
  digitalWrite(2, LOW); // 2 is interne led
}
void netBlink() {
  digitalWrite(2, HIGH);
  delay(50);
  digitalWrite(2, LOW);
  delay(50);
  digitalWrite(2, HIGH);
  delay(50);
  digitalWrite(2, LOW);
}
