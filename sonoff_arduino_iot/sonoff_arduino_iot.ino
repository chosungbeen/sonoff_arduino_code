#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include <ArduinoJson.h>

//json을 위한 설정
StaticJsonDocument<200> doc;
DeserializationError error;
JsonObject root;

const char *ssid = "U+NetA5DF";  // 와이파이 이름
const char *pass = "0105014585";      // 와이파이 비밀번호
const char *thingId = "";          // 사물 이름 (thing ID) 
const char *host = "adh167sfy9bbp-ats.iot.us-east-2.amazonaws.com"; // AWS IoT Core 주소
const char* outTopic = "sungbeen/out"; 
const char* inTopic = "sungbeen/in"; 
String sIP;

int gpio13Led = 13;
int gpio12Relay = 12;


// 사물 인증서 (파일 이름: xxxxxxxxxx-certificate.pem.crt)
const char cert_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUBSjdqv1D7v6eVCNPsN+ANoFHzT0wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MTAwOTE0MTAw
NFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMRNusNzN3nGY6NZxzLS
rEGLYPZdaleCA2TQX1j/jH6yjWLQOzdRFT8ld6NaN+zed+G5ol8ChRWsy/DPgJlV
5BwFPO/jCo0T3jZ3xF8mF/NT98hOQoahaIuwn95W85BNJr5shdXm2eN+y1jp9KcA
32UE2ovmCo0dlcA+dXRDe2lnVW54v3BPlJVqwezO1M37DZJCRh6TNytvO9ygbMUb
Puv2OZmwOsdHorH22rn8DsPNMk3KJ6WjKBCA28AcN0pTaCB9QQBezL3u1C+PP+9E
FZGrqZxGBv9GCKmtpSDFzyQzy9QgTYeHhGL/ncGUKVU+Voalnvk3Q2fYNZw6A00X
V9UCAwEAAaNgMF4wHwYDVR0jBBgwFoAUdy8qerntemlMNBnroZHUrVi7BFcwHQYD
VR0OBBYEFNzwZ6XOJWsXDdSvjZWN2ptqotKPMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAXEg0glSIxICaYCl6GxumatV33
kNA9MV/xDeeBn4oiAzWx7qaTr4aF6k1SuBd2/58dGEIdwKmCq4XCtOZj66WYIHGV
FECbt5QKeEMSATW5EElDVgmpJVzsLzlcVZKjJeRB8t+KdjxIx9P2A+rMWeRoUOVV
Yi/vJkHLLKKnlEl1ZU7KvhTsuMPPIdBPf9UzhjndGuKFLITCxnco2Y926P024geS
/jjlPUnLFaQWk1BQsdv2XOTIvBxUx1b636lLsCQLBDLQmIkrTY4K/Y3mppkQQFLM
OK+tHxUumrWdzK8hU7kSNbiq9CecmToO1tpaR0RFIbDR8XQXKkr4exOj0C+c
-----END CERTIFICATE-----
)EOF";
// 사물 인증서 프라이빗 키 (파일 이름: xxxxxxxxxx-private.pem.key)
const char key_str[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAxE26w3M3ecZjo1nHMtKsQYtg9l1qV4IDZNBfWP+MfrKNYtA7
N1EVPyV3o1o37N534bmiXwKFFazL8M+AmVXkHAU87+MKjRPeNnfEXyYX81P3yE5C
hqFoi7Cf3lbzkE0mvmyF1ebZ437LWOn0pwDfZQTai+YKjR2VwD51dEN7aWdVbni/
cE+UlWrB7M7UzfsNkkJGHpM3K2873KBsxRs+6/Y5mbA6x0eisfbaufwOw80yTcon
paMoEIDbwBw3SlNoIH1BAF7Mve7UL48/70QVkaupnEYG/0YIqa2lIMXPJDPL1CBN
h4eEYv+dwZQpVT5WhqWe+TdDZ9g1nDoDTRdX1QIDAQABAoIBAQCwIJGuZYTjxMWH
PbkFd5JIKLbxtPUB2MwUuwShhH/Y+XjzrKNEyYJHquVB8mN1KlHjfDwcBNXFkclV
ocffS/ojzetTzKCd3bKlqiXNvQ8GKlOKK/VyeAWmN+3ofpl/4kkd157pQXxDqnLm
rtrssFK/f3BazyauYFoJJ8EYdKeY9CQ7qeD6NBW2izsQVd6P2b9NnWXB0l+4xLGo
HbjDZ6Cex15Sz/TwxXOcpgev28s8WeIyov7QX/DcFZ/dEL8HFodQc7yFnZgq7sRP
X8Cv3CXZak9/fVQYzTY5iQZ6FRpWx676Y1iAP67lxkAYBzyjYjvj1sZ0afOKxVzJ
MKrl+ustAoGBAOqCsX1tB7cFN0B3H9ugpMbTBoZ3tvm1Xvcyn4RenLnYObUOkjWm
a9zucPbOH8uDCpsnEI0MSTsta4K7vbc19BftTZ2Hf6gu56Fvpgnvq/lsLDzhaV4G
+0Aa6NYhpwKhLI+f+3ohY2ltUec3CkuiS6FV1+Wg8jBSIxnaMV3WdDkbAoGBANZK
wMxv880Ge1w/Frr7WMvkxaLpi3xjcya2MBSQPEh0cchBVrtjGiitvAW5Q6us0DKW
hr8rELMyTPUUeZ1S6iigDKBhKB6zYskIhWuVxTKik/LDR0HM62c19iqCm3SiMRRO
bgzzrqxW9wijFmHQDYrnNNFMQH6jlc63pxSMfTHPAoGAblrCGzvLfZy+u19TZo7Z
+5YiY/TB7hAjeEAiQLAlIG9X+JO6yfRencC29+1XmPU5cHmZmEZHz8k35nVieWyy
SPAqrg6tS/+GkLlxcQuP6fe1eZC5fL3cTfZbyyha5ynhRlUKihZcKah/+kY0WhMt
c+Zpim2KiqKcUfYTdQ0N9DECgYEAvuPgUzz3JRA9G7NngDEerKKiG7mHc8rzbOym
b+r8iF6SbR0nMc3aI5p//2lFisxqumW0qt6mBbstKrSOkUKnLIFGPifOIWsS+JoK
oAfOnoiM2phyBX+Rw2MiYkPuTzucFMa46fi5BPSDIsKE0aUQK0hCmJ8gZK88Z+TZ
hUiQzakCgYEA4yryYpzyIrokrQazd9+Tb9aMxGBAIpmBBTtzfFK8bfoMbCq2o6UD
DtxZCRZSbDopewnA4SqtZkZrkrsmuZJSXdIZPKl+J5iMV7vXGSzEZBIe4bT5iGXW
GjgcnHv5kvwqytJzxJLv3kWy4dn6N9YO2QU7ylelDIlljcqX+qJBdbI=
-----END RSA PRIVATE KEY-----
)EOF";
// Amazon Trust Services(ATS) 엔드포인트 CA 인증서 (서버인증 > "RSA 2048비트 키: Amazon Root CA 1" 다운로드)
const char ca_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  deserializeJson(doc,payload);
  root = doc.as<JsonObject>();
  int value = root["on"];
  if(value==1) {
    digitalWrite(gpio13Led, HIGH);
    digitalWrite(gpio12Relay, HIGH);
  }
  else {
    digitalWrite(gpio13Led, LOW);
    digitalWrite(gpio12Relay, LOW);
  }
  Serial.println(value);
}

X509List ca(ca_str);
X509List cert(cert_str);
PrivateKey key(key_str);
WiFiClientSecure wifiClient;
PubSubClient client(host, 8883, callback, wifiClient); //set  MQTT port number to 8883 as per //standard

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(thingId)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "hello world");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      char buf[256];
      wifiClient.getLastSSLError(buf,256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void setup() {
  pinMode(gpio13Led, OUTPUT);
  pinMode(gpio12Relay, OUTPUT);
  digitalWrite(gpio13Led, HIGH);
  digitalWrite(gpio12Relay, HIGH);
  
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  sIP=toStringIp(WiFi.localIP());
  Serial.println(sIP);

  wifiClient.setTrustAnchors(&ca);
  wifiClient.setClientRSACert(&cert, &key);
  Serial.println("Certifications and key are set");

  setClock();
  //client.setServer(host, 8883);
  client.setCallback(callback);
}

long lastMsg = 0;
char msg[50];
int value = 0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld",value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    Serial.println(sIP);
    client.publish(outTopic, msg);
    Serial.print("Heap: "); Serial.println(ESP.getFreeHeap()); //Low heap can cause problems
  }
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
