#include <Bridge.h>
#include <HttpClient.h>
#include <BridgeClient.h>

#define DEVICEID "Dq0kN6se" // Input your deviceId
#define DEVICEKEY "uYQnB0bhMgbwDb6H" // Input your deviceKey
#define SITE_URL "api.mediatek.com"

static int LED_PIN = 13;
static int PIEZO_PIN = A0; // Piezo output

static unsigned long beat = 0;
static String commandServer;
static int commandPort = 0;

// This will be used to connect to command server
BridgeClient bc;

void setup()
{
  Bridge.begin();
  Serial.begin(115200);

  getCommandServer();
  beat = millis();
}

void getCommandServer()
{
  Serial.print("Query command server:");
  
  // Prepare header for MCS API authentication
  String header = "deviceKey: ";
  header += DEVICEKEY;
  header += "\r\n";
  header += "Connection: close";

  HttpClient c;
  c.setHeader(header);
  c.get(SITE_URL "/mcs/v2/devices/" DEVICEID "/connections.csv");
  c.setTimeout(1000);
  
  const String resp = c.readString();
  const int sep = resp.indexOf(',');
  if (-1 == sep)
  {
    return;
  }
  commandServer = resp.substring(0, sep);
  commandPort = resp.substring(sep+1).toInt();  
  Serial.print(commandServer);
  Serial.print(":");
  Serial.println(commandPort);
  int ret = bc.connect(commandServer.c_str(), commandPort);
  Serial.print("connect result = ");
  Serial.println(ret);
  Serial.flush();
}

void heartBeat(Client &c)
{
  // Send a heart beat data;
  // format reference: https://mcs.mediatek.com/resources/latest/api_references/#get-connection
  static const char* heartbeat = DEVICEID "," DEVICEKEY ",0";
  c.println(heartbeat);
  c.println();
  Serial.println("HeartBeat sent.");
  Serial.flush();
  delay(100);
}

void loop()
{
  if(!bc.connected())
  {
    Serial.println("command server offline, try re-connect");
    int ret = bc.connect(commandServer.c_str(), commandPort);
    Serial.print("connect = ");
    Serial.println(ret);
    Serial.flush();
  }
  
  // send heart beat for every 30 seconds;
  // or our connection will be closed by command server.
  if(30000 < (millis() - beat))
  {
    heartBeat(bc);
    beat = millis();
  }

  // process commands if any
  // note that the command server will also echo our hearbeat.
  String tcpcmd;
  static const char* tcpcmd_led_on = "switch,1";
  static const char* tcpcmd_led_off = "switch,0";
  bool sendUpdate = false;
  while (bc.available())
  {
    int v = bc.read();
    if (v != -1)
    {
      Serial.print((char)v);
      tcpcmd += (char)v;
      if (tcpcmd.substring(40).equals(tcpcmd_led_on)){
        digitalWrite(LED_PIN, HIGH);
        Serial.println("Switch LED ON ");
        tcpcmd="";
        sendUpdate = true;
      }else if(tcpcmd.substring(40).equals(tcpcmd_led_off)){  
        digitalWrite(LED_PIN, LOW);
        Serial.println("Switch LED OFF");
        tcpcmd="";
        sendUpdate = true;
      }
    }else{
      Serial.println("NULL");
    }
  }

  // POST LED value back to MCS.
  uploadstatus(sendUpdate);
  
  delay(300);
}

void uploadstatus(bool sendUpdate)
{  
  // We update the LED_Display data value
  // to reflect the status of the board.
  int piezoADC = analogRead(PIEZO_PIN);
  float piezoV = piezoADC / 1023.0 * 5.0;
  String upload_piezoADC = "voltage,," + String(piezoV);
  String upload_led = (digitalRead(LED_PIN)==0) ? "led,,0\n" : "led,,1\n";
  
  String upload_str = upload_led;
  if(digitalRead(LED_PIN)==1) upload_str += upload_piezoADC;

  String header = "Content-Type: text-csv\r\n";
  header += "deviceKey: ";
  header += DEVICEKEY;
  header += "\r\n";
  header += "Connection: close";

  // refer to https://mcs.mediatek.com/resources/latest/api_references/#upload-data-points
  HttpClient http;
  http.setHeader(header);
  http.post(SITE_URL "/mcs/v2/devices/" DEVICEID "/datapoints.csv", upload_str.c_str());
  Serial.println(upload_piezoADC.c_str());
  http.setTimeout(1000);
  
  delay(300);

  String resp = http.readString();
  Serial.print("POST result:");
  Serial.println(resp);
}

