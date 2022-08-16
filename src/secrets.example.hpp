// Copyright (c) 2022 Alwin Lohrie
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <Arduino.h>

const char* ssid = "yourap";
const char* password = "yourpw";
const char* mqtt_server = "192.168.178.33";
const char* mqtt_server = "192.168.178.33";
const char* mqtt_alarm_topic = "sirens/alwin2/alarm";
const char* mqtt_alarms_topic = "sirens/alwin2/alarms/#"; 
const char* mqtt_alarms_state_topic_prefix = "sirens/alwin2/state/alarms"; 
const char* mqtt_alarm_state_topic = "sirens/alwin2/state/alarm"; 
const char* mqtt_availability_topic = "sirens/alwin2/availability";
const uint8_t pin = 5;
const int baseFrequency = 1000;
