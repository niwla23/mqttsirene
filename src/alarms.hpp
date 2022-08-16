// Copyright (c) 2022 Alwin Lohrie
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <Arduino.h>

// serves as a base class for all alarms
class BaseAlarm {
   protected:
    int _pin;
    int _delay_left = 0;

   public:
    BaseAlarm(uint8_t pin) {
        this->_pin = pin;
    };
    void setDelay(int millis) {
        this->_delay_left = millis;
    }
    bool do_delays() {
        if (_delay_left > 0) {
            this->_delay_left--;
            delay(1);
            return false;
        } else {
            return true;
        }
    }
    void tick() {
        Serial.println("base. tf you doing?");
    };
};

// keeps the speaker off
class OffAlarm : public BaseAlarm {
   public:
    OffAlarm(uint8_t pin) : BaseAlarm(pin){};
    void tick() {
        noTone(this->_pin);
        delay(10);
    }
};

class FoodReadyAlarm : public BaseAlarm {
   private:
    int _baseFrequency;

   public:
    FoodReadyAlarm(uint8_t pin, int baseFrequency) : BaseAlarm(pin) {
        this->_baseFrequency = baseFrequency;
    };
    void tick() {
        Serial.println("food ready!!");
        for (size_t i = 0; i < 10; i++) {
            tone(this->_pin, i * 10 + this->_baseFrequency);
            delay(10);
        }
        // delay(100);
        for (size_t i = 10; i > 0; i--) {
            tone(this->_pin, i * 10 + this->_baseFrequency);
            delay(10);
        }
    }
};

class MailboxAlarm : public BaseAlarm {
   public:
    MailboxAlarm(uint8_t pin) : BaseAlarm(pin){};
    void tick() {
        if (this->do_delays()) {
            tone(this->_pin, 300);
            delay(500);
            tone(this->_pin, 600);
            delay(500);
            tone(this->_pin, 300);
            delay(500);
            noTone(this->_pin);
            this->setDelay(300);
        }
    }
};

class DoorbellAlarm : public BaseAlarm {
   public:
    DoorbellAlarm(uint8_t pin) : BaseAlarm(pin){};
    void tick() {
        tone(this->_pin, 500);
        delay(50);
        noTone(this->_pin);
        delay(10);
    }
};

class LowFrequencyAlarm : public BaseAlarm {
   public:
    LowFrequencyAlarm(uint8_t pin) : BaseAlarm(pin){};
    void tick() {
        tone(this->_pin, 80);
        delay(50);
        tone(this->_pin, 100);
        delay(40);
    }
};

class ShortBeepAlarm : public BaseAlarm {
   private:
    size_t tick_count = 0;

   public:
    ShortBeepAlarm(uint8_t pin) : BaseAlarm(pin){};
    void tick() {
        if (tick_count > 10000) {
            tone(this->_pin, 600);
        }
        if (tick_count > 10050) {
            noTone(this->_pin);
            tick_count = 0;
        }
        tick_count++;
    }
};

class LongBeepAlarm : public BaseAlarm {
   private:
    size_t tick_count = 0;

   public:
    LongBeepAlarm(uint8_t pin) : BaseAlarm(pin){};
    void tick() {
        if (tick_count > 10000) {
            noTone(this->_pin);
        }
        if (tick_count > 10050) {
            tone(this->_pin, 600);

            tick_count = 0;
        }
        tick_count++;
    }
};