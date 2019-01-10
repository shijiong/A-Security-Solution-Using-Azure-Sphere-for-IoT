# A Security Solution Using Azure Sphere for IoT
The Azure Sphere based security solution for Internet of Things (IoT) proposed in this project, which utilizes the fully integrated, high performance MT3620 microcontroller unit (MCU), as well as Microsoft Azure cloud services, can be used to meet the requirements of modern, robust end-to-end security user scenarios.
![image](https://github.com/shijiong/A-Security-Solution-Using-Azure-Sphere-for-IoT/blob/master/Cover.png)

# Hardware requirement
1. Azure Sphere MT3620 development kit (Seeed Studio).
2. CC2530 ZigBee Enddevice and Coordinator.
3. Sensors: Grove1 -DHT11; Grove-Light Sensor; Grove-Sound Sensor; Grove-Gas Sensor (MQ2); Grove-PIR Motion Sensor; Grove-Relay (All sensors are manufactured by Seeed Studio). 

# Software requirement
1. Windows 10 PC with the latest updates.
2. Visual Studio 2017 Community IDE (Enterprise and Professional version are also supported).
3. Azure Sphere software development kit (SDK) Version 18.11.
4. IAR Embedded Workbench for 8051 V8.10.
5. Microsoft Azure Subscription.

# Building the application
## ZigBee Wireless Sensor Network
1. Navigate to "ZStack-CC2530-2.5.1a/Projects/zstack/Samples/GenericApp/CC2530DB/", open GenericApp.eww with IAR Embedded Workbench for 8051.
2. Build the "EndDevice" and "Coordinator" respectively, and use CC Debuger to download the application to CC2530 boards.
3. Connect all the sensors to the EndDevice.
4. Connect Coordinator's Tx0, Rx0, GND (UART0) with MT3620's Rx0, Tx0 and GND.
5. Power on the EndDevice and Coordinator to enable the ZigBee Wireless Sensor Network.

## Azure Sphere MT3620 device
1. Navigate to "AzureSphereAzureIoTHub/", open AzureSphereAzureIoTHub.sln with Visual Studio 2017.
2. Modify the azureIoTCertificatesX in "azure_iot_utilities.c" with your own Certificates.
3. Enter your IoTHub in "app_manifest.json" under "AllowedConnections" section.
4. Build the application and download the application to the MT3620.
5. Power on the device.

## Windows-form based remote monitor application
1. Navigate to "AzureSphereRemoteMonitor/", open AzureSphereRemoteMonitor.sln with Visual Studio 2017.
2. Modify the connectionString in "Form1.cs" with your own connectionString.
3. Build the application and run it on local PC.
