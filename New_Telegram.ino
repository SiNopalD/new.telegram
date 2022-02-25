#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <CTBot.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

#define BOTtoken "your telegram bot token"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Cek untuk pesan baru setiap 1 detik.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = D8;
bool ledState = LOW;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);

    // Print pesan diterima
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Selamat Datang , " + from_name + ".\n";
      welcome += "Gunakan perintah berikut untuk mengontrol output Anda.\n\n";
      welcome += "/led_on untuk MENGAKTIFKAN GPIO \n";
      welcome += "/led_off untuk MENONAKTIFKAN GPIO \n";
      welcome += "/state untuk meminta status GPIO saat ini \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/led_on") {
      bot.sendMessage(chat_id, "LED status set ke ON", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }

    if (text == "/led_off") {
      bot.sendMessage(chat_id, "LED status set ke OFF", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }

    if (text == "/state") {
      if (digitalRead(ledPin)) {
        bot.sendMessage(chat_id, "LED is ON", "");
      }
      else {
        bot.sendMessage(chat_id, "LED is OFF", "");
      }
    }
  }
}

void setup() {
  WiFiManager wifiManager;
  wifiManager.autoConnect("LampOpal");
  Serial.begin(115200);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  

  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  // Connect to Wi-Fi
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);

  // Print ESP32 Local IP Address
  //Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("Respon Diterima");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
