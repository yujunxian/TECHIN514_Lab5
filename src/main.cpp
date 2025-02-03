/**
 * ABOUT:
 *
 * This example is for new users which are familiar with other legacy Firebase libraries.
 *
 * The example shows how to set, push and get the values to/from Realtime database.
 *
 * All functions used in this example are blocking (sync) functions.
 *
 * This example will not use any authentication method included database secret.
 *
 * It needs to change the security rules to allow read and write.
 *
 * This example is for ESP32, ESP8266 and Raspberry Pi Pico W.
 *
 * You can adapt the WiFi and SSL client library that are available for your devices.
 *
 * For the ethernet and GSM network which are not covered by this example,
 * you have to try another elaborate examples and read the library documentation thoroughly.
 *
 */

#include <Arduino.h>
#include <HCSR04.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

#define WIFI_SSID "YJX"
#define WIFI_PASSWORD "66668888"

#define DATABASE_SECRET "AIzaSyB6FhRCeGqARmXPSifgXun29CQ4DsZPPaY/"
#define DATABASE_URL "https://techin514-lab5-8f142-default-rtdb.firebaseio.com"

WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));

FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
LegacyToken dbSecret(DATABASE_SECRET);

HCSR04 hc(D2,D3);
int bl_ulttasonic=0;
int n_faraway=0;
 
#define LET_SKIPINIT	1
#define uS_TO_S_FACTOR 1000000 	
#define TIME_TO_SLEEP  20 
#define TIMES_TO_LOOP  30 

RTC_DATA_ATTR int bootCount = 0;    
int send_time_delay=30 ;
int print_wakeup_reason();
int print_wakeup_reason()
{
	int rt=0;
    esp_sleep_wakeup_cause_t wakeup_reason;
 
    wakeup_reason = esp_sleep_get_wakeup_cause();
 
    switch(wakeup_reason)
    {
        case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO");
			rt=1;
			break;
        case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); 
			rt=1;
			break;
        case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer");
			rt=1;
			break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
        case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
        default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
    }
	return rt;
}

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

void setup()
{

	Serial.begin(115200);
    delay(1000);  
	 
    ++bootCount;    
    Serial.println("Boot number: " + String(bootCount));
 
    int n=print_wakeup_reason();   
 
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);      
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +" Seconds");
 
	if(n==1 && LET_SKIPINIT==1){	
		return ;	//
		/*Serial.println("Going to sleep now");
		delay(1000);
		Serial.flush(); 
		esp_deep_sleep_start();     // deep sleep
		Serial.println("This will never be printed");*/
	}
	
	int i=0;
	
    Serial.begin(115200);
	delay(1000);    
	
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();	
	
    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    ssl.setInsecure();
#if defined(ESP8266)
    ssl.setBufferSizes(1024, 1024);
#endif
    
    // Initialize the authentication handler.
    Serial.println("Initializing the app...");
    initializeApp(client, app, getAuth(dbSecret));
	
		//stage 4 ultrasonic + wifi + send data 
	i=0;
	
	Serial.print("\n stage 4 ultrasonic + wifi= send datas\n");
	
    // Binding the authentication handler with your Database class object.
    app.getApp<RealtimeDatabase>(Database);

    // Set your database URL (requires only for Realtime Database)
    Database.url(DATABASE_URL);

    // In sync functions, we have to set the operating result for the client that works with the function.
    client.setAsyncResult(result);
	
	i=10;//do not do send here
	while(i<5){
		// Set, push and get integer value

		Serial.print("Setting the int value... ");
		bool status = Database.set<int>(client, "/test/int", 12345);
		if (status)
			Serial.println("ok");
		else
			printError(client.lastError().code(), client.lastError().message());

		Serial.print("Pushing the int value... ");
		String name = Database.push<int>(client, "/test/push", 12345);
		if (client.lastError().code() == 0)
			Firebase.printf("ok, name: %s\n", name.c_str());
		else
			printError(client.lastError().code(), client.lastError().message());

		Serial.print("Getting the int value... ");
		int v1 = Database.get<int>(client, "/test/int");
		if (client.lastError().code() == 0)
			Serial.println(v1);
		else
			printError(client.lastError().code(), client.lastError().message());

		// Set, push and get Boolean value

		Serial.print("Setting the bool value... ");
		status = Database.set<bool>(client, "/test/bool", true);
		if (status)
			Serial.println("ok");
		else
			printError(client.lastError().code(), client.lastError().message());

		Serial.print("Pushing the bool value... ");
		name = Database.push<bool>(client, "/test/push", true);
		if (client.lastError().code() == 0)
			Firebase.printf("ok, name: %s\n", name.c_str());

		Serial.print("Getting the bool value... ");
		bool v2 = Database.get<bool>(client, "/test/bool");
		if (client.lastError().code() == 0)
			Serial.println(v2);
		else
			printError(client.lastError().code(), client.lastError().message());

		// Set, push, and get String value

		Serial.print("Setting the String value... ");
		status = Database.set<String>(client, "/test/string", "hello");
		if (status)
			Serial.println("ok");
		else
			printError(client.lastError().code(), client.lastError().message());

		Serial.print("Pushing the String value... ");
		name = Database.push<String>(client, "/test/push", "hello");
		if (client.lastError().code() == 0)
			Firebase.printf("ok, name: %s\n", name.c_str());

		Serial.print("Getting the String value... ");
		String v3 = Database.get<String>(client, "/test/string");
		if (client.lastError().code() == 0)
			Serial.println(v3);
		else
			printError(client.lastError().code(), client.lastError().message());

		// Set, push, and get float value

		Serial.print("Setting the float value... ");
		status = Database.set<number_t>(client, "/test/float", number_t(123.456, 2));
		if (status)
			Serial.println("ok");
		else
			printError(client.lastError().code(), client.lastError().message());

		Serial.print("Pushing the float value... ");
		name = Database.push<number_t>(client, "/test/push", number_t(123.456, 2));
		if (client.lastError().code() == 0)
			Firebase.printf("ok, name: %s\n", name.c_str());

		Serial.print("Getting the float value... ");
		float v4 = Database.get<float>(client, "/test/float");
		if (client.lastError().code() == 0)
			Serial.println(v4);
		else
			printError(client.lastError().code(), client.lastError().message());

		// Set, push, and get double value
		
		Serial.print("Setting the double value... ");

		status = Database.set<number_t>(client, "/test/double", number_t(1234.56789, 4));
		if (status)
			Serial.println("ok");
		else
			printError(client.lastError().code(), client.lastError().message());

		Serial.print("Pushing the double value... ");
		name = Database.push<number_t>(client, "/test/push", number_t(1234.56789, 4));
		if (client.lastError().code() == 0)
			Firebase.printf("ok, name: %s\n", name.c_str());

		Serial.print("Getting the double value... ");
		double v5 = Database.get<double>(client, "/test/double");
		if (client.lastError().code() == 0)
			Serial.println(v5);
		else
			printError(client.lastError().code(), client.lastError().message());

		// Set, push, and get JSON value

		Serial.print("Setting the JSON object... ");

		status = Database.set<object_t>(client, "/test/json", object_t("{\"test\":{\"data\":123}}"));
		if (status)
			Serial.println("ok");
		else
			printError(client.lastError().code(), client.lastError().message());

		Serial.print("Pushing the JSON object... ");
		name = Database.push<object_t>(client, "/test/push", object_t("{\"test\":{\"data\":123}}"));
		if (client.lastError().code() == 0)
			Firebase.printf("ok, name: %s\n", name.c_str());

		Serial.print("Getting the JSON object... ");
		String v6 = Database.get<String>(client, "/test/json");
		if (client.lastError().code() == 0)
			Serial.println(v6);
		else
			printError(client.lastError().code(), client.lastError().message());

		// Set, push and get Array value

		Serial.print("Setting the Array object... ");

		status = Database.set<object_t>(client, "/test/array", object_t("[1,2,\"test\",true]"));
		if (status)
			Serial.println("ok");
		else
			printError(client.lastError().code(), client.lastError().message());

		Serial.print("Pushing the Array object... ");
		name = Database.push<object_t>(client, "/test/push", object_t("[1,2,\"test\",true]"));
		if (client.lastError().code() == 0)
			Firebase.printf("ok, name: %s\n", name.c_str());

		Serial.print("Getting the Array object... ");
		String v7 = Database.get<String>(client, "/test/array");
		if (client.lastError().code() == 0)
			Serial.println(v7);
		else
			printError(client.lastError().code(), client.lastError().message());
		delay(10000);
		i++;
	}
	   
	Serial.println("All tests finished!");
}

void loop()
{
	
    // We don't need to poll the async task using Database.loop(); as in the Stream examples because
    // only blocking (sync) functions were used in this example.

    // We don't have to poll authentication handler task using app.loop() as seen in other examples
    // because the database secret is the priviledge access key that never expired.
	double d=hc.dist();
	if(d>=0.3){	//is something is not stand above the ultra_sonic 
		n_faraway++;
	}
	else{
		n_faraway=0;
	}   
	Serial.print("Get distance and Pushing the value... ");
	String name = Database.push<number_t>(client, "/test/dist_dectected", number_t(d, 4));
	delay(TIMES_TO_LOOP*1000);	
		
	if (n_faraway>2)	// if there still not object stand before the ultrasonic ,go to sleep 30 seconds
	{
		n_faraway=0;
		Serial.println("Going to sleep now");
		Serial.flush(); 
		esp_deep_sleep_start();     // deep sleep
		Serial.println("This will never be printed");
	}
}