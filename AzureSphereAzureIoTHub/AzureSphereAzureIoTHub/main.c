﻿#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

// applibs_versions.h defines the API struct versions to use for applibs APIs.
#include "applibs_versions.h"
#include "epoll_timerfd_utilities.h"

#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/wificonfig.h>

#include "mt3620_rdb.h"
#include "rgbled_utility.h"

// This sample C application for a MT3620 Reference Development Board (Azure Sphere) demonstrates how to
// connect an Azure Sphere device to an Azure IoT Hub. To use this sample, you must first
// add the Azure IoT Hub Connected Service reference to the project (right-click
// References -> Add Connected Service -> Azure IoT Hub), which populates this project
// with additional sample code used to communicate with an Azure IoT Hub.
//
// The sample leverages these functionalities of the Azure IoT SDK C:
// - Device to cloud messages;
// - Cloud to device messages;
// - Direct Method invocation;
// - Device Twin management;
//
// A description of the sample follows:
// - LED 1 blinks constantly.
// - Pressing button A toggles the rate at which LED 1 blinks
//   between three values.
// - Pressing button B triggers the sending of a message to the IoT Hub.
// - LED 2 flashes red when button B is pressed (and a
//   message is sent) and flashes yellow when a message is received.
// - LED 3 indicates whether network connection to the Azure IoT Hub has been
//   established.
//
// Direct Method related notes:
// - Invoking the method named "LedColorControlMethod" with a payload containing '{"color":"red"}'
//   will set the color of LED 1 to red;
//
// Device Twin related notes:
// - Setting LedBlinkRateProperty in the Device Twin to a value from 0 to 2 causes the sample to
//   update the blink rate of LED 1 accordingly, e.g '{"LedBlinkRateProperty": 2}';
// - Upon receipt of the LedBlinkRateProperty desired value from the IoT hub, the sample updates
//   the device twin on the IoT hub with the new value for LedBlinkRateProperty.
// - Pressing button A causes the sample to report the blink rate to the device
//   twin on the IoT Hub.

// This sample uses the API for the following Azure Sphere application libraries:
// - gpio (digital input for button);
// - log (messages shown in Visual Studio's Device Output window during debugging);
// - wificonfig (configure WiFi settings);
// - azureiot (interaction with Azure IoT services)

#ifndef AZURE_IOT_HUB_CONFIGURED
#error \
    "WARNING: Please add a project reference to the Connected Service first \
(right-click References -> Add Connected Service)."
#endif

#include "azure_iot_utilities.h"

// An array defining the RGB GPIOs for each LED on the device
static const GPIO_Id ledsPins[3][3] = {
    {MT3620_RDB_LED1_RED, MT3620_RDB_LED1_GREEN, MT3620_RDB_LED1_BLUE}, {MT3620_RDB_LED2_RED, MT3620_RDB_LED2_GREEN, MT3620_RDB_LED2_BLUE}, {MT3620_RDB_LED3_RED, MT3620_RDB_LED3_GREEN, MT3620_RDB_LED3_BLUE}};

static size_t blinkIntervalIndex = 0;
static RgbLedUtility_Colors ledBlinkColor = RgbLedUtility_Colors_Blue;

static const struct timespec blinkIntervals[] = {{0, 125000000}, {0, 250000000}, {0, 500000000}};
static const size_t blinkIntervalsCount = sizeof(blinkIntervals) / sizeof(*blinkIntervals);

// File descriptors - initialized to invalid value
static int epollFd = -1;
static int gpioLedBlinkRateButtonFd = -1;
static int gpioSendMessageButtonFd = -1;
static int gpioButtonsManagementTimerFd = -1;
static int gpioLed1TimerFd = -1;
static int gpioLed2TimerFd = -1;
static int azureIotDoWorkTimerFd = -1;

// LED state
static RgbLed led1 = RGBLED_INIT_VALUE;
static RgbLed led2 = RGBLED_INIT_VALUE;
static RgbLed led3 = RGBLED_INIT_VALUE;
static RgbLed *rgbLeds[] = {&led1, &led2, &led3};
static const size_t rgbLedsCount = sizeof(rgbLeds) / sizeof(*rgbLeds);

// Default blinking rate of LED1
static struct timespec blinkingLedPeriod = {0, 125000000};
static bool blinkingLedState;

// A null period to not start the timer when it is created with CreateTimerFdAndAddToEpoll.
static const struct timespec nullPeriod = {0, 0};
static const struct timespec defaultBlinkTimeLed2 = {0, 150 * 1000 * 1000};

// Connectivity state
static bool connectedToIoTHub = false;

// Termination state
static volatile sig_atomic_t terminationRequired = false;

//Uart jiongshi
static int uartFd = -1;

/// <summary>
///     Signal handler for termination requests. This handler must be async-signal-safe.
/// </summary>
static void TerminationHandler(int signalNumber)
{
    // Don't use Log_Debug here, as it is not guaranteed to be async-signal-safe.
    terminationRequired = true;
}

//Uart shijiong
/// <summary>
///     Helper function to send a fixed message via the given UART.
/// </summary>
/// <param name="uartFd">The open file descriptor of the UART to write to</param>
/// <param name="dataToSend">The data to send over the UART</param>
static void SendUartMessage(int uartFd, const char *dataToSend)
{
	size_t totalBytesSent = 0;
	size_t totalBytesToSend = strlen(dataToSend);
	int sendIterations = 0;
	while (totalBytesSent < totalBytesToSend) {
		sendIterations++;

		// Send as much of the remaining data as possible
		size_t bytesLeftToSend = totalBytesToSend - totalBytesSent;
		const char *remainingMessageToSend = dataToSend + totalBytesSent;
		ssize_t bytesSent = write(uartFd, remainingMessageToSend, bytesLeftToSend);
		if (bytesSent < 0) {
			Log_Debug("ERROR: Could not write to UART: %s (%d).\n", strerror(errno), errno);
			terminationRequired = true;
			return;
		}

		totalBytesSent += (size_t)bytesSent;
	}

	Log_Debug("Sent %zu bytes over UART in %d calls\n", totalBytesSent, sendIterations);
}

//Uart Shijiong
/// <summary>
///     Handle UART event: if there is incoming data, print it, and blink the LED.
/// </summary>
static void UARTEventHandler()
{
	const size_t receiveBufferSize = 64;
	uint8_t receiveBuffer[receiveBufferSize + 1]; // allow extra byte for string termination
	ssize_t bytesRead;

	// Read UART message
	bytesRead = read(uartFd, receiveBuffer, receiveBufferSize);
	if (bytesRead < 0) {
		Log_Debug("ERROR: Could not read UART: %s (%d).\n", strerror(errno), errno);
		terminationRequired = true;
		return;
	}

	if (bytesRead > 0) {
		// Null terminate the buffer to make it a valid string, and print it
		receiveBuffer[bytesRead] = 0;
		Log_Debug("UART received %d bytes: '%s'\n", bytesRead, (char *)receiveBuffer);


		char deviceID[1];
		strncpy(deviceID, &receiveBuffer[1], 1);
		int id = deviceID[0] - 48;

		char tempValue[2];
		strncpy(tempValue, &receiveBuffer[2], 2);
		int temp = (tempValue[0] - 48) * 10 + (tempValue[1] - 48);


		char humiValue[2];
		strncpy(humiValue, &receiveBuffer[4], 2);
		int humi = (humiValue[0] - 48) * 10 + (humiValue[1] - 48);


		char lightValue[2];
		strncpy(lightValue, &receiveBuffer[6], 2);
		int light = (lightValue[0] - 48) * 10 + (lightValue[1] - 48);

		char gasValue[2];
		strncpy(gasValue, &receiveBuffer[8], 2);
		int gas = (gasValue[0] - 48) * 10 + (gasValue[1] - 48);

		char pirValue[1];
		strncpy(pirValue, &receiveBuffer[10], 1);
		int pir = pirValue[0] - 48;

		//AzureIoT_SendMessage(receiveBuffer);
		JSON_Value *root_value = json_value_init_object();
		JSON_Object *root_object = json_value_get_object(root_value);

		json_object_set_number(root_object, "Device ID", id);
		json_object_set_number(root_object, "Temperature", temp);
		json_object_set_number(root_object, "Humidity", humi);
		json_object_set_number(root_object, "Light", light);
		json_object_set_number(root_object, "Gas", gas);
		json_object_set_number(root_object, "PIR", pir);

		char *serialized_string = json_serialize_to_string_pretty(root_value);
		AzureIoT_SendMessage(serialized_string);

		json_free_serialized_string(serialized_string);
		json_value_free(root_value);
	}
}

/// <summary>
///     Show details of the currently connected WiFi network.
/// </summary>
static void DebugPrintCurrentlyConnectedWiFiNetwork(void)
{
    WifiConfig_ConnectedNetwork network;
    int result = WifiConfig_GetCurrentNetwork(&network);
    if (result < 0) {
        Log_Debug("INFO: Not currently connected to a WiFi network.\n");
    } else {
        Log_Debug("INFO: Currently connected WiFi network: \n");
        Log_Debug("INFO: SSID \"%.*s\", BSSID %02x:%02x:%02x:%02x:%02x:%02x, Frequency %dMHz.\n",
                  network.ssidLength, network.ssid, network.bssid[0], network.bssid[1],
                  network.bssid[2], network.bssid[3], network.bssid[4], network.bssid[5],
                  network.frequencyMHz);
    }
}

/// <summary>
///     Helper function to blink LED2 once.
/// </summary>
static void BlinkLed2Once(void)
{
    RgbLedUtility_SetLed(&led2, RgbLedUtility_Colors_Red);
    SetTimerFdToSingleExpiry(gpioLed2TimerFd, &defaultBlinkTimeLed2);
}

/// <summary>
///     Helper function to open a file descriptor for the given GPIO as input mode.
/// </summary>
/// <param name="gpioId">The GPIO to open.</param>
/// <param name="outGpioFd">File descriptor of the opened GPIO.</param>
/// <returns>True if successful, false if an error occurred.</return>
static bool OpenGpioFdAsInput(GPIO_Id gpioId, int *outGpioFd)
{
    *outGpioFd = GPIO_OpenAsInput(gpioId);
    if (*outGpioFd < 0) {
        Log_Debug("ERROR: Could not open GPIO '%d': %d (%s).\n", gpioId, errno, strerror(errno));
        return false;
    }

    return true;
}

/// <summary>
///     Toggles the blink speed of the blink LED between 3 values, and updates the device twin.
/// </summary>
/// <param name="rate">The blink rate</param>
static void SetLedRate(const struct timespec *rate)
{
    if (SetTimerFdToPeriod(gpioLed1TimerFd, rate) != 0) {
        Log_Debug("ERROR: could not set the period of the LED.\n");
        terminationRequired = true;
        return;
    }

    if (connectedToIoTHub) {
        // Report the current state to the Device Twin on the IoT Hub.
        AzureIoT_TwinReportState("LedBlinkRateProperty", blinkIntervalIndex);
    } else {
        Log_Debug("WARNING: Cannot send reported property; not connected to the IoT Hub.\n");
    }
}

/// <summary>
///     Sends a message to the IoT Hub.
/// </summary>
static void SendMessageToIotHub(void)
{
    if (connectedToIoTHub) {
        // Send a message
        AzureIoT_SendMessage("Hello from Azure IoT sample!");

        // Set the send/receive LED2 to blink once immediately to indicate the message has been
        // queued.
        BlinkLed2Once();
    } else {
        Log_Debug("WARNING: Cannot send message: not connected to the IoT Hub.\n");
    }
}

/// <summary>
///     MessageReceived callback function, called when a message is received from the Azure IoT Hub.
/// </summary>
/// <param name="payload">The payload of the received message.</param>
static void MessageReceived(const char *payload)
{
    // Set the send/receive LED2 to blink once immediately to indicate a message has been received.
    BlinkLed2Once();
}

/// <summary>
///     Device Twin update callback function, called when an update is received from the Azure IoT
///     Hub.
/// </summary>
/// <param name="desiredProperties">The JSON root object containing the desired Device Twin
/// properties received from the Azure IoT Hub.</param>
static void DeviceTwinUpdate(JSON_Object *desiredProperties)
{
    JSON_Value *blinkRateJson = json_object_get_value(desiredProperties, "LedBlinkRateProperty");

    // If the attribute is missing or its type is not a number.
    if (blinkRateJson == NULL) {
        Log_Debug(
            "INFO: A device twin update was received that did not contain the property "
            "\"LedBlinkRateProperty\".\n");
    } else if (json_value_get_type(blinkRateJson) != JSONNumber) {
        Log_Debug(
            "INFO: Device twin desired property \"LedBlinkRateProperty\" was received with "
            "incorrect type; it must be an integer.\n");
    } else {
        // Get the value of the LedBlinkRateProperty and print it.
        size_t desiredBlinkRate = (size_t)json_value_get_number(blinkRateJson);

        blinkIntervalIndex =
            desiredBlinkRate % blinkIntervalsCount; // Clamp value to [0..blinkIntervalsCount) .

        Log_Debug("INFO: Received desired value %zu for LedBlinkRateProperty, setting it to %zu.\n",
                  desiredBlinkRate, blinkIntervalIndex);

        blinkingLedPeriod = blinkIntervals[blinkIntervalIndex];
        SetLedRate(&blinkIntervals[blinkIntervalIndex]);
    }
}

/// <summary>
///     Allocates and formats a string message on the heap.
/// </summary>
/// <param name="messageFormat">The format of the message</param>
/// <param name="maxLength">The maximum length of the formatted message string</param>
/// <returns>The pointer to the heap allocated memory.</returns>
static void *SetupHeapMessage(const char *messageFormat, size_t maxLength, ...)
{
    va_list args;
    va_start(args, maxLength);
    char *message =
        malloc(maxLength + 1); // Ensure there is space for the null terminator put by vsnprintf.
    if (message != NULL) {
        vsnprintf(message, maxLength, messageFormat, args);
    }
    va_end(args);
    return message;
}

/// <summary>
///     Direct Method callback function, called when a Direct Method call is received from the Azure
///     IoT Hub.
/// </summary>
/// <param name="methodName">The name of the method being called.</param>
/// <param name="payload">The payload of the method.</param>
/// <param name="responsePayload">The response payload content. This must be a heap-allocated
/// string, 'free' will be called on this buffer by the Azure IoT Hub SDK.</param>
/// <param name="responsePayloadSize">The size of the response payload content.</param>
/// <returns>200 HTTP status code if the method name is "LedColorControlMethod" and the color is
/// correctly parsed;
/// 400 HTTP status code is the color has not been recognised in the payload;
/// 404 HTTP status code if the method name is unknown.</returns>
static int DirectMethodCall(const char *methodName, const char *payload, size_t payloadSize,
                            char **responsePayload, size_t *responsePayloadSize)
{
    // Prepare the payload for the response. This is a heap allocated null terminated string.
    // The Azure IoT Hub SDK is responsible of freeing it.
    *responsePayload = NULL;  // Reponse payload content.
    *responsePayloadSize = 0; // Response payload content size.

    int result = 404; // HTTP status code.

    if (strcmp(methodName, "LedColorControlMethod") != 0) {
        result = 404;
        Log_Debug("INFO: Method not found called: '%s'.\n", methodName);

        static const char noMethodFound[] = "\"method not found '%s'\"";
        size_t responseMaxLength = sizeof(noMethodFound) + strlen(methodName);
        *responsePayload = SetupHeapMessage(noMethodFound, responseMaxLength, methodName);
        if (*responsePayload == NULL) {
            Log_Debug("ERROR: Could not allocate buffer for direct method response payload.\n");
            abort();
        }
        *responsePayloadSize = strlen(*responsePayload);
        return result;
    }

    RgbLedUtility_Colors ledColor = RgbLedUtility_Colors_Unknown;
    // The payload should contains JSON such as: { "color": "red"}
    char *directMethodCallContent = malloc(payloadSize + 1); // +1 to store null char at the end.
    if (directMethodCallContent == NULL) {
        Log_Debug("ERROR: Could not allocate buffer for direct method request payload.\n");
        abort();
    }

    memcpy(directMethodCallContent, payload, payloadSize);
    directMethodCallContent[payloadSize] = 0; // Null terminated string.
    JSON_Value *payloadJson = json_parse_string(directMethodCallContent);
    if (payloadJson == NULL) {
        goto colorNotFound;
    }
    JSON_Object *colorJson = json_value_get_object(payloadJson);
    if (colorJson == NULL) {
        goto colorNotFound;
    }
    const char *colorName = json_object_get_string(colorJson, "color");
    if (colorName == NULL) {
        goto colorNotFound;
    }

    ledColor = RgbLedUtility_GetColorFromString(colorName, strlen(colorName));

    // If color's name has not been identified.
    if (ledColor == RgbLedUtility_Colors_Unknown) {
        goto colorNotFound;
    }

    // Color's name has been identified.
    result = 200;
    const char *colorString = RgbLedUtility_GetStringFromColor(ledColor);
    Log_Debug("INFO: LED color set to: '%s'.\n", colorString);
    // Set the blinking LED color.
    ledBlinkColor = ledColor;

    static const char colorOkResponse[] =
        "{ \"success\" : true, \"message\" : \"led color set to %s\" }";
    size_t responseMaxLength = sizeof(colorOkResponse) + strlen(payload);
    *responsePayload = SetupHeapMessage(colorOkResponse, responseMaxLength, colorString);
    if (*responsePayload == NULL) {
        Log_Debug("ERROR: Could not allocate buffer for direct method response payload.\n");
        abort();
    }
    *responsePayloadSize = strlen(*responsePayload);

    return result;

colorNotFound:
    result = 400; // Bad request.
    Log_Debug("INFO: Unrecognised direct method payload format.\n");

    static const char noColorResponse[] =
        "{ \"success\" : false, \"message\" : \"request does not contain an identifiable "
        "color\" }";
    responseMaxLength = sizeof(noColorResponse);
    *responsePayload = SetupHeapMessage(noColorResponse, responseMaxLength);
    if (*responsePayload == NULL) {
        Log_Debug("ERROR: Could not allocate buffer for direct method response payload.\n");
        abort();
    }
    *responsePayloadSize = strlen(*responsePayload);

    return result;
}

/// <summary>
///     IoT Hub connection status callback function.
/// </summary>
/// <param name="connected">'true' when the connection to the IoT Hub is established.</param>
static void IoTHubConnectionStatusChanged(bool connected)
{
    connectedToIoTHub = connected;
}

/// <summary>
///     Handle the blinking for LED1.
/// </summary>
static void Led1UpdateHandler(event_data_t *eventData)
{
    if (ConsumeTimerFdEvent(gpioLed1TimerFd) != 0) {
        terminationRequired = true;
        return;
    }

    // Set network status with LED3 color.
    RgbLedUtility_Colors color =
        (connectedToIoTHub ? RgbLedUtility_Colors_Green : RgbLedUtility_Colors_Off);
    RgbLedUtility_SetLed(&led3, color);

    // Trigger LED to blink as appropriate.
    blinkingLedState = !blinkingLedState;
    color = (blinkingLedState ? ledBlinkColor : RgbLedUtility_Colors_Off);
    RgbLedUtility_SetLed(&led1, color);
}

/// <summary>
///     Handle the blinking for LED2.
/// </summary>
static void Led2UpdateHandler(event_data_t *eventData)
{
    if (ConsumeTimerFdEvent(gpioLed2TimerFd) != 0) {
        terminationRequired = true;
        return;
    }

    // Clear the send/receive LED2.
    RgbLedUtility_SetLed(&led2, RgbLedUtility_Colors_Off);
}

/// <summary>
///     Check whether a given button has just been pressed.
/// </summary>
/// <param name="fd">The button file descriptor</param>
/// <param name="oldState">Old state of the button (pressed or released)</param>
/// <returns>true if pressed, false otherwise</returns>
static bool IsButtonPressed(int fd, GPIO_Value_Type *oldState)
{
    bool isButtonPressed = false;
    GPIO_Value_Type newState;
    int result = GPIO_GetValue(fd, &newState);
    if (result != 0) {
        Log_Debug("ERROR: Could not read button GPIO: %s (%d).\n", strerror(errno), errno);
        terminationRequired = true;
    } else {
        // Button is pressed if it is low and different than last known state.
        isButtonPressed = (newState != *oldState) && (newState == GPIO_Value_Low);
        *oldState = newState;
    }

    return isButtonPressed;
}

/// <summary>
///     Handle button timer event: if the button is pressed, change the LED blink rate.
/// </summary>
static void ButtonsHandler(event_data_t *eventData)
{
    if (ConsumeTimerFdEvent(gpioButtonsManagementTimerFd) != 0) {
        terminationRequired = true;
        return;
    }

    // If the button is pressed, change the LED blink interval, and update the Twin Device.
    static GPIO_Value_Type blinkButtonState;
    if (IsButtonPressed(gpioLedBlinkRateButtonFd, &blinkButtonState)) {
        blinkIntervalIndex = (blinkIntervalIndex + 1) % blinkIntervalsCount;
        SetLedRate(&blinkIntervals[blinkIntervalIndex]);
    }

    // If the button is pressed, send a message to the IoT Hub.
    static GPIO_Value_Type messageButtonState;
    if (IsButtonPressed(gpioSendMessageButtonFd, &messageButtonState)) {
        SendMessageToIotHub();
    }
}

/// <summary>
///     Hand over control periodically to the Azure IoT SDK's DoWork.
/// </summary>
static void AzureIotDoWorkHandler(event_data_t *eventData)
{
    if (ConsumeTimerFdEvent(azureIotDoWorkTimerFd) != 0) {
        terminationRequired = true;
        return;
    }

    // Set up the connection to the IoT Hub client.
    // Notes it is safe to call this function even if the client has already been set up, as in
    //   this case it would have no effect
    if (AzureIoT_SetupClient()) {
        // AzureIoT_DoPeriodicTasks() needs to be called frequently in order to keep active
        // the flow of data with the Azure IoT Hub
        AzureIoT_DoPeriodicTasks();
    }
}

// event handler data structures. Only the event handler field needs to be populated.
static event_data_t buttonsEventData = {.eventHandler = &ButtonsHandler};
static event_data_t led1EventData = {.eventHandler = &Led1UpdateHandler};
static event_data_t led2EventData = {.eventHandler = &Led2UpdateHandler};
static event_data_t azureIotEventData = {.eventHandler = &AzureIotDoWorkHandler};

/// <summary>
///     Initialize peripherals, termination handler, and Azure IoT
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static int InitPeripheralsAndHandlers(void)
{
    // Register a SIGTERM handler for termination requests
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = TerminationHandler;
    sigaction(SIGTERM, &action, NULL);

	//Uart shijiong
	// Create a UART_Config object, open the UART and set up UART event handler
	UART_Config uartConfig;
	UART_InitConfig(&uartConfig);
	uartConfig.baudRate = 115200;
	uartConfig.flowControl = UART_FlowControl_None;
	uartFd = UART_Open(MT3620_RDB_HEADER2_ISU0_UART, &uartConfig);
	if (uartFd < 0) {
		Log_Debug("ERROR: Could not open UART: %s (%d).\n", strerror(errno), errno);
		return -1;
	}

    // Open button A
    Log_Debug("INFO: Opening MT3620_RDB_BUTTON_A.\n");
    if (!OpenGpioFdAsInput(MT3620_RDB_BUTTON_A, &gpioLedBlinkRateButtonFd)) {
        return -1;
    }

    // Open button B
    Log_Debug("INFO: Opening MT3620_RDB_BUTTON_B.\n");
    if (!OpenGpioFdAsInput(MT3620_RDB_BUTTON_B, &gpioSendMessageButtonFd)) {
        return -1;
    }

    // Open file descriptors for the RGB LEDs and store them in the rgbLeds array (and in turn in
    // the ledBlink, ledMessageEventSentReceived, ledNetworkStatus variables)
    RgbLedUtility_OpenLeds(rgbLeds, rgbLedsCount, ledsPins);

    // Initialize the Azure IoT SDK
    if (!AzureIoT_Initialize()) {
        Log_Debug("ERROR: Cannot initialize Azure IoT Hub SDK.\n");
        return -1;
    }

    // Set the Azure IoT hub related callbacks
    AzureIoT_SetMessageReceivedCallback(&MessageReceived);
    AzureIoT_SetDeviceTwinUpdateCallback(&DeviceTwinUpdate);
    AzureIoT_SetDirectMethodCallback(&DirectMethodCall);
    AzureIoT_SetConnectionStatusCallback(&IoTHubConnectionStatusChanged);

    // Display the currently connected WiFi connection.
    DebugPrintCurrentlyConnectedWiFiNetwork();

    epollFd = CreateEpollFd();
    if (epollFd < 0) {
        return -1;
    }

    // Set up a timer for LED1 blinking
    gpioLed1TimerFd =
        CreateTimerFdAndAddToEpoll(epollFd, &blinkingLedPeriod, &led1EventData, EPOLLIN);
    if (gpioLed1TimerFd < 0) {
        return -1;
    }

    // Set up a timer for blinking LED2 once.
    gpioLed2TimerFd = CreateTimerFdAndAddToEpoll(epollFd, &nullPeriod, &led2EventData, EPOLLIN);
    if (gpioLed2TimerFd < 0) {
        return -1;
    }

    // Set up a timer for buttons status check
    static struct timespec buttonsPressCheckPeriod = {0, 1000000};
    gpioButtonsManagementTimerFd =
        CreateTimerFdAndAddToEpoll(epollFd, &buttonsPressCheckPeriod, &buttonsEventData, EPOLLIN);
    if (gpioButtonsManagementTimerFd < 0) {
        return -1;
    }

    // Set up a timer for Azure IoT SDK DoWork execution.
    static struct timespec azureIotDoWorkPeriod = {1, 0};
    azureIotDoWorkTimerFd =
        CreateTimerFdAndAddToEpoll(epollFd, &azureIotDoWorkPeriod, &azureIotEventData, EPOLLIN);
    if (azureIotDoWorkTimerFd < 0) {
        return -1;
    }

    return 0;
}

/// <summary>
///     Close peripherals and Azure IoT
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
    Log_Debug("INFO: Closing GPIOs and Azure IoT client.\n");

    // Close all file descriptors
    CloseFdAndPrintError(gpioLedBlinkRateButtonFd, "LedBlinkRateButton");
    CloseFdAndPrintError(gpioSendMessageButtonFd, "SendMessageButton");
    CloseFdAndPrintError(gpioButtonsManagementTimerFd, "ButtonsManagementTimer");
    CloseFdAndPrintError(azureIotDoWorkTimerFd, "IotDoWorkTimer");
    CloseFdAndPrintError(gpioLed1TimerFd, "Led1Timer");
    CloseFdAndPrintError(gpioLed2TimerFd, "Led2Timer");
    CloseFdAndPrintError(epollFd, "Epoll");

	//Uart shijiong
	CloseFdAndPrintError(uartFd, "Uart");

    // Close the LEDs and leave then off
    RgbLedUtility_CloseLeds(rgbLeds, rgbLedsCount);

    // Destroy the IoT Hub client
    AzureIoT_DestroyClient();
    AzureIoT_Deinitialize();
}

/// <summary>
///     Main entry point for this application.
/// </summary>
int main(int argc, char *argv[])
{
    Log_Debug("INFO: Azure IoT application starting.\n");

    int initResult = InitPeripheralsAndHandlers();
    if (initResult != 0) {
        terminationRequired = true;
    }

    while (!terminationRequired) {
        if (WaitForEventAndCallHandler(epollFd) != 0) {
            terminationRequired = true;
        }
		//uart shijiong
		UARTEventHandler();
    }

    ClosePeripheralsAndHandlers();
    Log_Debug("INFO: Application exiting.\n");
    return 0;
}
