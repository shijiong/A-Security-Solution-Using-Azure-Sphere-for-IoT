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

# Create Azure IoTHub
IoT Hub service is used to gather all the data that sent from the Azure Sphere devices. The IoTHub service is created as follows.
1. In the [Azure Portal](https://portal.azure.com), click the on the "New" icon along the left to see all the services, then click the "Internet of Things" item and choose “IoT Hub”.
2. Complete the fields of “Name”, “Pricing Tier”, “IoT Hub Units”, “Device-to-Cloud Partitions”, “Resource group” and “location”, then click the "Create" button. It should be noted that “F1 Free” Pricing is enough for our project. And we’d better choose the local location of the service.
3. Wait for the new IoT hub to show as "Online". When it is ready, open the blade of the new IoT hub, take note of the URI and select the key icon at the top to access to the shared access policy settings.
4. Select the Shared access policy called iothubowner, and take note of the Primary key and connection string in the right blade. We may copy these into a text file for future use.
5. For more information, we can refer to this doc online: Create an IoT hub using the [Azure portal](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-create-through-portal).

# Create Azure Stream Analytics Job
Azure Stream Analytics is used in this project to filter the sensor data that gathered by IoTHub, and stream the data to PowerBI. The new Stream Analytics is created as follows.
1. In the [Azure Portal](https://portal.azure.com), click the on the "New" icon along the left to see all the services, then click the "Internet of Things" item and choose “Stream Analytics job”.
2. Complete the fields of “Job Name”, “Resource group” and “location”, then click the "Create" button. Wait for the new Stream Analytics to show as "Online".
3. Now, it’s time to config the Stream Analytics Job. First, in the Inputs window, we choose IoT Hub, and in the IoT Hub Settings screen, we complete the following information:
* Input Alias: MT3620
* Subscription: Use IoT Hub from Current Subscription
* Choose an IoT Hub: input the name used during the IoT Hub creation
* IoT Hub Shared Access Policy Name: iothubowner
* IoT Hub Consumer Group: powerbi
Click Next, and then Complete (leave the Serialization settings as they are).
To set up the output, go to the Stream Analytics Job's OUTPUTS tab, and click the ADD AN OUTPUT link. In the Add an output to your job popup, select the POWER BI option and the click the Next button. In the following screen you will setup the credentials of your Power BI account to allow the job to connect and send data to it. Click the Authorize Now link. 
To set up the Query configuration, go to the Stream Analytics Job QUERY tab and replace the query with the following statement:
``` C
SELECT
   *
INTO
   [PowerBI]
FROM
   [MT3620]
``` 
It should be noticed that we just select all the data from IoTHub to PowerBI. Click on the SAVE button and YES in the confirmation dialog. Now that the job is configured, the START button is enabled. Click the button to start the job and then select the JOB START TIME option in the START OUTPUT popup. After clicking OK the job will be started. Once the job starts it creates the Power BI datasource associated with the given subscription.

# Config PowerBI Dashboard
Now that the datasource is created, go back to Power BI session, and find My Workspace by clicking the Power BI link. After some minutes of the job running you will see that the dataset that you configured as an output for the Job, is now displayed in the Power BI workspace Datasets section.
Please note that the Power BI dataset will only be created if the job is running and if it is receiving data from the IoT Hub input, so check that the MT3620 is working and sending data to Azure to ensure that the dataset be created. To check if the Stream Analytics job is receiving and processing data you can check the Azure Management Stream Analytics monitor.
Once the datasource becomes available you can start creating reports. To create a new Report, click on the Power BI datasource.
The Report designer will be opened showing the list of fields available for the selected datasource and the different visualizations supported by the tool. Here we choose Humidity, sound, light and temperature value as Y axis, and measurement time as X axis. Now the report is almost ready. Click the SAVE button and set the name for the report. The following figure shows the reports in editor view including average temperature, average humidity, maximum light, minimum sound, real-time sound and light, real-time temperature and humidity.  
![image](https://github.com/shijiong/A-Security-Solution-Using-Azure-Sphere-for-IoT/blob/master/powerbiview.png)

# Browse the data anywhere anytime you want
With [Power BI Desktop](https://powerbi.microsoft.com/en-us/desktop/), we can get the data from Power BI Web and display the data report that we create above on desktop PC. See the report as follows.  
![image](https://github.com/shijiong/A-Security-Solution-Using-Azure-Sphere-for-IoT/blob/master/PowerBIDesktop.png)
With Power BI application on iOS or Android, we can get the data from Power BI Web and display the data report on mobile phone. See the reports on Andorid device as follows.  
![image](https://github.com/shijiong/A-Security-Solution-Using-Azure-Sphere-for-IoT/blob/master/PowerBIMobile.png)
