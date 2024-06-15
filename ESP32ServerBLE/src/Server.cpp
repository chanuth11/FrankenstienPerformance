/*
  Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
  Ported to Arduino ESP32 by Evandro Copercini
  updated by chegewara and MoThunderz
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

// Initialize all pointers
BLEServer* pServer = NULL;                        // Pointer to the server
BLECharacteristic* pCharacteristic_1 = NULL;      // Pointer to Characteristic 1
BLECharacteristic* pCharacteristic_2 = NULL;      // Pointer to Characteristic 2
BLECharacteristic* pCharacteristic_3 = NULL;      // Pointer to Characteristic 2
BLECharacteristic* pCharacteristic_4 = NULL;      // Pointer to Characteristic 2

BLEDescriptor *pDescr_1;                          // Pointer to Descriptor of Characteristic 1
BLE2902 *pBLE2902_1;                              // Pointer to BLE2902 of Characteristic 1
BLE2902 *pBLE2902_2;                              // Pointer to BLE2902 of Characteristic 2
BLE2902 *pBLE2902_3;                              // Pointer to BLE2902 of Characteristic 2
BLE2902 *pBLE2902_4;                              // Pointer to BLE2902 of Characteristic 2

// Some variables to keep track on device connected
bool deviceConnected = false;
bool oldDeviceConnected = false;
// Variable that will continuously be increased and written to the client
uint32_t value = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
// UUIDs used in this example:
#define SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_1 "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define FSR1_IN GPIO_NUM_27
#define FSR2_IN GPIO_NUM_39
#define FSR3_IN GPIO_NUM_34
#define FSR4_IN GPIO_NUM_35
#define FSR5_IN GPIO_NUM_32


int Fsr1Out = 0;
int Fsr2Out = 0;
int Fsr3Out = 0;
int Fsr4Out = 0;
int Fsr5Out = 0;

int Mtr1DutyCycle = 0;
int Mtr2DutyCycle = 0;
int Mtr3DutyCycle = 0;
int Mtr4DutyCycle = 0;
int Mtr5DutyCycle = 0;

// Callback function that is called whenever a client is connected or disconnected
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic_1 = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_1,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE   
                    );                   

  // pCharacteristic_2 = pService->createCharacteristic(
  //                     CHARACTERISTIC_UUID_2,
  //                     BLECharacteristic::PROPERTY_READ   |
  //                     BLECharacteristic::PROPERTY_WRITE                        
  //                   );  
  // pCharacteristic_3 = pService->createCharacteristic(
  //                     CHARACTERISTIC_UUID_3,
  //                     BLECharacteristic::PROPERTY_READ   |
  //                     BLECharacteristic::PROPERTY_WRITE                        
  //                   );  

  // pCharacteristic_4 = pService->createCharacteristic(
  //                   CHARACTERISTIC_UUID_3,
  //                   BLECharacteristic::PROPERTY_READ   |
  //                   BLECharacteristic::PROPERTY_WRITE                        
  //                 ); 

  // Create a BLE Descriptor  
  pDescr_1 = new BLEDescriptor((uint16_t)0x2901);
  pDescr_1->setValue("A very interesting variable");
  pCharacteristic_1->addDescriptor(pDescr_1);

  // Add the BLE2902 Descriptor because we are using "PROPERTY_NOTIFY"
  pBLE2902_1 = new BLE2902();
  pBLE2902_1->setNotifications(true);                 
  //pCharacteristic_1->addDescriptor(pBLE2902_1);

  // pBLE2902_2 = new BLE2902();
  // pBLE2902_2->setNotifications(true);
  // //pCharacteristic_2->addDescriptor(pBLE2902_2);

  // pBLE2902_3 = new BLE2902();
  // pBLE2902_3->setNotifications(true);
  // //pCharacteristic_3->addDescriptor(pBLE2902_3);

  // pBLE2902_4 = new BLE2902();
  // pBLE2902_4->setNotifications(true);
  //pCharacteristic_4->addDescriptor(pBLE2902_4);
  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
  static boolean connected = false;
  static boolean doScan = false;
  
  // Define pointer for the BLE connection
  static BLEAdvertisedDevice* myDevice;
  BLERemoteCharacteristic* pRemoteChar_1;
  BLERemoteCharacteristic* pRemoteChar_2;
  BLERemoteCharacteristic* pRemoteChar_3;
  BLERemoteCharacteristic* pRemoteChar_4;
  
}

void loop() {
    Fsr1Out = analogRead(FSR1_IN);
    Mtr1DutyCycle = (Fsr1Out*255) / 4095; // Map ADC to PWM duty cycle
    String Mtr1DutyCycleStr = String(Mtr1DutyCycle) + "%";

    Fsr2Out = analogRead(FSR2_IN);
    Mtr2DutyCycle = (Fsr2Out*255) / 4095; // Map ADC to PWM duty cycle
    String Mtr2DutyCycleStr = String(Mtr2DutyCycle) + "%";

    Fsr3Out = analogRead(FSR3_IN);
    Mtr3DutyCycle = (Fsr3Out*255) / 4095; // Map ADC to PWM duty cycle
    String Mtr3DutyCycleStr = String(Mtr3DutyCycle) + "%";

    Fsr4Out = analogRead(FSR4_IN);
    Mtr4DutyCycle = (Fsr4Out*255) / 4095; // Map ADC to PWM duty cycle
    String Mtr4DutyCycleStr = String(Mtr4DutyCycle) + "%";

    Fsr5Out = analogRead(FSR5_IN);
    Mtr5DutyCycle = (Fsr5Out*255) / 4095; // Map ADC to PWM duty cycle
    String Mtr5DutyCycleStr = String(Mtr5DutyCycle) + "%";

    String Long_String = Mtr1DutyCycleStr + Mtr2DutyCycleStr + Mtr3DutyCycleStr + Mtr4DutyCycleStr + Mtr5DutyCycleStr;
    // notify changed value
    if (deviceConnected) {
      // pCharacteristic_1 is an integer that is increased with every second
      // in the code below we send the value over to the client and increase the integer counter
      
      // Read the FSR data, map to Duty cycle for analogwrite on server, convert to string to send
      
      /*Characteristic 1*/

      pCharacteristic_1->setValue(Long_String.c_str());
      Serial.println("Characteristic 1 (setValue): " + Long_String);
      //pCharacteristic_1->notify();

      // pCharacteristic_2 is a std::string (NOT a String). In the code below we read the current value
      // write this to the Serial interface and send a different value back to the Client
      // Here the current value is read using getValue() 
      // std::string rxValue = pCharacteristic_2->getValue();
      // Serial.print("Characteristic 2 (getValue): ");
      // Serial.println(rxValue.c_str());

      /*Characteristic 2*/
      // Read the FSR data, map to Duty cycle for analogwrite on server, convert to string to send

      // Here the value is written to the Client using setValue();
      // pCharacteristic_2->setValue(Mtr2DutyCycleStr.c_str());
      // Serial.println("Characteristic 2 (setValue): " + Mtr2DutyCycleStr);
      //pCharacteristic_2->notify();

      /*Characteristic 3*/
      // Read the FSR data, map to Duty cycle for analogwrite on server, convert to string to send

      // Here the value is written to the Client using setValue();
      // pCharacteristic_3->setValue(Mtr3DutyCycleStr.c_str());
      // Serial.println("Characteristic 3 (setValue): " + Mtr3DutyCycleStr);
      //pCharacteristic_2->notify();

      /*Characteristic 4*/
      // Read the FSR data, map to Duty cycle for analogwrite on server, convert to string to send

      // Here the value is written to the Client using setValue();
      // pCharacteristic_4->setValue(Mtr4DutyCycleStr.c_str());
      // Serial.println("Characteristic 4 (setValue): " + Mtr4DutyCycleStr);
      // pCharacteristic_2->notify();
      // In this example "delay" is used to delay with one second. This is of course a very basic 
      // implementation to keep things simple. I recommend to use millis() for any production code
      delay(5);
    }
    // The code below keeps the connection status uptodate:
    // Disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(2); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // Connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
