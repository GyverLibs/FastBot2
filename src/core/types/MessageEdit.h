#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message.h"

namespace fb {

// https://core.telegram.org/bots/api#editmessagetext
class TextEdit : private Message {
    friend class ::VirtualFastBot2;

   public:
    TextEdit() {}
    TextEdit(const String& text, uint32_t messageID, const su::Value& chatID) : messageID(messageID) {
        this->text = text;
        this->chatID = chatID;
    }

    // id сообщения
    uint32_t messageID;

    using Message::chatID;
    using Message::mode;
    using Message::preview;
    using Message::setInlineMenu;
    using Message::text;

   protected:
    void makePacket(fb::Packet& p) const {
        Message::makePacket(p);
        p[fb::api::message_id] = messageID;
    }
};

// https://core.telegram.org/bots/api#editmessagereplymarkup
class MenuEdit : private Message {
    friend class ::VirtualFastBot2;

   public:
    MenuEdit() {}
    MenuEdit(uint32_t messageID, const su::Value& chatID) : messageID(messageID) {
        this->chatID = chatID;
    }

    // id сообщения
    uint32_t messageID;

    using Message::chatID;
    using Message::setInlineMenu;

   protected:
    void makePacket(fb::Packet& p) const {
        Message::makePacket(p);
        p[fb::api::message_id] = messageID;
    }
};

// https://core.telegram.org/bots/api#editmessagecaption
class CaptionEdit : private Message {
    friend class ::VirtualFastBot2;

   public:
    CaptionEdit() {}
    CaptionEdit(const String& caption, uint32_t messageID, const su::Value& chatID) : caption(caption), messageID(messageID) {
        this->chatID = chatID;
    }

    // заголовок
    String caption;

    // id сообщения
    uint32_t messageID;

    using Message::chatID;
    using Message::mode;
    using Message::setInlineMenu;

   protected:
    void makePacket(fb::Packet& p) const {
        Message::makePacket(p);
        p[fb::api::message_id] = messageID;
        p.addStringEsc(fb::api::caption, caption);
    }
};

// https://core.telegram.org/bots/api#editmessagelivelocation
class LocationEdit : private Message {
    friend class ::VirtualFastBot2;

   public:
    LocationEdit() {}
    LocationEdit(float latitude, float longitude, uint32_t messageID, const su::Value& chatID) : latitude(latitude), longitude(longitude), messageID(messageID) {
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

   protected:
    void makePacket(fb::Packet& p) const {
        Message::makePacket(p);
        p[fb::api::message_id] = messageID;
        p.addFloat(fb::api::latitude, latitude, 6);
        p.addFloat(fb::api::longitude, longitude, 6);
        if (!isnan(horizontalAccuracy)) p.addFloat(fb::api::horizontal_accuracy, horizontalAccuracy, 1);
        if (heading) p[fb::api::heading] = heading;
        if (proximityAlertRadius) p[fb::api::proximity_alert_radius] = proximityAlertRadius;
    }
};

// https://core.telegram.org/bots/api#stopmessagelivelocation
class LocationStop : private Message {
    friend class ::VirtualFastBot2;

   public:
    LocationStop() {}
    LocationStop(uint32_t messageID, const su::Value& chatID) : messageID(messageID) {
        this->chatID = chatID;
    }

    // id сообщения
    uint32_t messageID;

    using Message::chatID;
    using Message::setInlineMenu;

   protected:
    void makePacket(fb::Packet& p) const {
        Message::makePacket(p);
        p[fb::api::message_id] = messageID;
    }
};

}  // namespace fb