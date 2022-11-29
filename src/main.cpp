#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <alarms.hpp>
#include <iostream>
#include <secrets.hpp>
#include <string>

WiFiClient espClient;
PubSubClient client(espClient);
boolean sirenActive = false;

OffAlarm off_alarm(pin);                              // 0
FoodReadyAlarm food_ready_alarm(pin, baseFrequency);  // 1
MailboxAlarm mailbox_alarm(pin);                      // 2
DoorbellAlarm doorbell_alarm(pin);                    // 3
LowFrequencyAlarm low_frequency_alarm(pin);           // 4
ShortBeepAlarm short_beep_alarm(pin);                 // 5
LongBeepAlarm long_beep_alarm(pin);                   // 6

int current_alarm = 0;

std::string mqtt_alarms_topic_prefix(mqtt_alarms_topic);

void playAlertSound() {
    if (beepIfNoConnection) {
        tone(pin, 30);
        delay(1);
        noTone(pin);
    }
}

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    WiFi.setAutoReconnect(true);

    while (WiFi.status() != WL_CONNECTED) {
        delay(5000);
        playAlertSound();
        Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        setup_wifi();
        playAlertSound();
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str()), mqtt_availability_topic, 2, true, "offline") {
            Serial.println("connected");

            client.publish(mqtt_availability_topic, "online", true);

            client.subscribe(mqtt_alarm_topic);
            client.subscribe(mqtt_alarms_topic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
    tone(pin, 1800);
    delay(3);
    noTone(pin);
}

// https://www.codegrepper.com/code-examples/c/check+if+string+starts+with+c
// checks whether a starts with b
bool startsWith(const char* a, const char* b) {
    if (strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("received message");
    Serial.println(topic);

    int previous_alarm = current_alarm;

    if (startsWith(topic, mqtt_alarms_topic_prefix.c_str())) {  // checks if the topic starts with the prefix
        // check if we have an off command
        char* message = (char*)payload;
        if (strncmp(message, "OFF", 3) == 0) {
            current_alarm = 0;
        } else {
            // find the alarm number from the topic
            size_t read_i = strlen(topic) - 1;  // ignore terminator
            size_t write_i = 0;
            char alarm_number_buffer[8];

            while (read_i > 0) {
                if (topic[read_i] == '/') {
                    alarm_number_buffer[write_i + 1] = '\0';
                    break;
                } else {
                    alarm_number_buffer[write_i] = topic[read_i];
                    Serial.print("wrote ");
                    Serial.println(topic[read_i]);
                    write_i++;
                }
                read_i--;
            }
            current_alarm = atoi(alarm_number_buffer);
        }

    } else {
        char* message = (char*)payload;
        message[length] = '\0';
        Serial.println(message);
        current_alarm = atoi(message);
    }

    if (previous_alarm != current_alarm) {
        char previous_alarm_char[5];
        char current_alarm_char[5];

        itoa(previous_alarm, previous_alarm_char, 10);
        itoa(current_alarm, current_alarm_char, 10);

        client.publish(mqtt_alarm_state_topic, current_alarm_char);

        char last_alarm_state_topic[strlen(mqtt_alarms_state_topic_prefix) + 20];
        strcpy(last_alarm_state_topic, mqtt_alarms_state_topic_prefix);
        strcat(last_alarm_state_topic, "/");
        strcat(last_alarm_state_topic, previous_alarm_char);

        char current_alarm_state_topic[strlen(mqtt_alarms_state_topic_prefix) + 20];
        strcpy(current_alarm_state_topic, mqtt_alarms_state_topic_prefix);
        strcat(current_alarm_state_topic, "/");
        strcat(current_alarm_state_topic, current_alarm_char);

        client.publish(last_alarm_state_topic, "OFF");
        client.publish(current_alarm_state_topic, "ON");
    }
}

void setup() {
    Serial.begin(115200);
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    mqtt_alarms_topic_prefix.pop_back();  // remove the #
    mqtt_alarms_topic_prefix.pop_back();  // remove the /
    pinMode(pin, OUTPUT);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    if (current_alarm == 1) {
        food_ready_alarm.tick();
    } else if (current_alarm == 2) {
        mailbox_alarm.tick();
    } else if (current_alarm == 3) {
        doorbell_alarm.tick();
    } else if (current_alarm == 4) {
        low_frequency_alarm.tick();
    } else if (current_alarm == 5) {
        short_beep_alarm.tick();
    } else if (current_alarm == 6) {
        long_beep_alarm.tick();
    } else {
        off_alarm.tick();
    }
}
