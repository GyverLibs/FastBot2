#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message.h"

namespace fb {

// https://core.telegram.org/bots/api#editmessagetext
class TextEdit : private Message {
    friend class ::FastBot2Client;

   public:
    TextEdit() {}
    TextEdit(Text text, uint32_t messageID, Value chatID) : messageID(messageID) {
        text.toString(this->text);
        this->chatID = chatID;
    }

    // id сообщения
    uint32_t messageID;

    using Message::chatID;
    using Message::mode;
    using Message::preview;
    using Message::setInlineMenu;
    using Message::text;
    using Message::json;

   protected:
    void makePacket(Packet& p) const {
        Message::makePacket(p);
        p[tg_api::message_id] = messageID;
    }
};

// https://core.telegram.org/bots/api#editmessagereplymarkup
class MenuEdit : private Message {
    friend class ::FastBot2Client;

   public:
    MenuEdit() {}
    MenuEdit(uint32_t messageID, Value chatID) : messageID(messageID) {
        this->chatID = chatID;
    }
    MenuEdit(uint32_t messageID, Value chatID, InlineMenu& menu) : messageID(messageID) {
        this->chatID = chatID;
        setInlineMenu(menu);
    }

    // id сообщения
    uint32_t messageID;

    using Message::chatID;
    using Message::setInlineMenu;
    using Message::json;

   protected:
    void makePacket(Packet& p) const {
        Message::makePacket(p);
        p[tg_api::message_id] = messageID;
    }
};

// https://core.telegram.org/bots/api#editmessagecaption
class CaptionEdit : private Message {
    friend class ::FastBot2Client;

   public:
    CaptionEdit() {}
    CaptionEdit(const String& caption, uint32_t messageID, Value chatID) : caption(caption), messageID(messageID) {
        this->chatID = chatID;
    }

    // заголовок
    String caption;

    // id сообщения
    uint32_t messageID;

    using Message::chatID;
    using Message::mode;
    using Message::setInlineMenu;
    using Message::json;

   protected:
    void makePacket(Packet& p) const {
        Message::makePacket(p);
        p[tg_api::message_id] = messageID;
        p.addStringEsc(tg_api::caption, caption);
    }
};

// https://core.telegram.org/bots/api#editmessagelivelocation
class LocationEdit : private Message {
    friend class ::FastBot2Client;

   public:
    LocationEdit() {}
    LocationEdit(float latitude, float longitude, uint32_t messageID, Value chatID) : latitude(latitude), longitude(longitude), messageID(messageID) {
        this->chatID = chatID;
    }

    // широта
    float latitude;

    // долгота
    float longitude;

    // id сообщения
    uint32_t messageID;

    // точность в метрах, 0-1500
    float horizontalAccuracy = NAN;

    // направление в градусах, 1-360
    uint16_t heading = 0;

    // Максимальное расстояние в метрах для оповещений о приближении к другому участнику чата
    uint32_t proximityAlertRadius = 0;

    using Message::chatID;
    using Message::setInlineMenu;
    using Message::json;

   protected:
    void makePacket(Packet& p) const {
        Message::makePacket(p);
        p[tg_api::message_id] = messageID;
        p.addFloat(tg_api::latitude, latitude, 6);
        p.addFloat(tg_api::longitude, longitude, 6);
        if (!isnan(horizontalAccuracy)) p.addFloat(tg_api::horizontal_accuracy, horizontalAccuracy, 1);
        if (heading) p[tg_api::heading] = heading;
        if (proximityAlertRadius) p[tg_api::proximity_alert_radius] = proximityAlertRadius;
    }
};

// https://core.telegram.org/bots/api#stopmessagelivelocation
class LocationStop : private Message {
    friend class ::FastBot2Client;

   public:
    LocationStop() {}
    LocationStop(uint32_t messageID, Value chatID) : messageID(messageID) {
        this->chatID = chatID;
    }

    // id сообщения
    uint32_t messageID;

    using Message::chatID;
    using Message::setInlineMenu;
    using Message::json;

   protected:
    void makePacket(Packet& p) const {
        Message::makePacket(p);
        p[tg_api::message_id] = messageID;
    }
};

}  // namespace fb