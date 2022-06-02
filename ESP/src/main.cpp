#include <Arduino.h>
#include "pinout.h"
#include "WifiHandler.h"
#include "MDNSManager.h"
#include "cameraHandler.h"
#include "LEDManager.h"
#include "webServer/webserverHandler.h"
#include "OTA.h"
#include "storage.h"
#include "StateManager.h"

auto ota = OpenIris::OTA();
auto ledManager = OpenIris::LEDManager(33);
auto trackerConfig = OpenIris::Configuration();
auto cameraHandler = OpenIris::CameraHandler(&trackerConfig);
auto stateManager = OpenIris::StateManager();
auto httpdHandler = OpenIris::HTTPDHandler(&trackerConfig);
auto mdnsHandler = OpenIris::MDNSHandler(&stateManager, &trackerConfig);

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  ledManager.setupLED();

  trackerConfig.setup("/config.json");
  trackerConfig.loadConfig();
  trackerConfig.attach(&cameraHandler);
  trackerConfig.attach(&mdnsHandler);

  cameraHandler.setupCamera();
  OpenIris::WiFiHandler::setupWifi(&stateManager, &trackerConfig);
  mdnsHandler.startMDNS();

  if (stateManager.getCurrentState() == OpenIris::ConnectingToWifiSuccess)
    // THIS WILL CRASH THE ESP IF NO NETWORK IS CONNECTED< WHAT THE HELL
    httpdHandler.startStreamServer();

  ledManager.on();
  ota.SetupOTA(trackerConfig);
}

void loop()
{
  ota.HandleOTAUpdate();
  ledManager.displayStatus();
}