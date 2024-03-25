#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message.h"

namespace fb {

// https://core.telegram.org/bots/api#editmessagetext
class EditText : private Message {
    friend class ::FastBot2;

   public:
    // id сообщения
    int32_t messageID;

    using Message::chatID;
    using Message::mode;
    using Message::preview;
    using Message::removeMenu;
    using Message::setMenu;
    using Message::text;

   protected:
    void makePacket(fb::Packet& p) {
        Message::makePacket(p);
        p.addInt(fbapi::message_id(), messageID);
    }
};

// https://core.telegram.org/bots/api#editmessagereplymarkup
class EditKeyboard : private Message {
    friend class ::FastBot2;

   public:
    // id сообщения
    int32_t messageID;

    using Message::chatID;
    using Message::removeMenu;
    using Message::setMenu;

   protected:
    void makePacket(fb::Packet& p) {
        Message::makePacket(p);
        p.addInt(fbapi::message_id(), messageID);
    }
};

// https://core.telegram.org/bots/api#editmessagecaption
class EditCaption : private Message {
    friend class ::FastBot2;

   public:
    // id сообщения
    int32_t messageID;

    // заголовок
    String caption;

    using Message::chatID;
    using Message::mode;
    using Message::removeMenu;
    using Message::setMenu;

   protected:
    void makePacket(fb::Packet& p) {
        Message::makePacket(p);
        p.addInt(fbapi::message_id(), messageID);
        p.addString(fbapi::caption(), caption);
    }
};

// https://core.telegram.org/bots/api#editmessagemedia
class EditMedia : private Message {
    friend class ::FastBot2;

   public:
    // id сообщения
    int32_t messageID;

    // media https://core.telegram.org/bots/api#inputmedia

    using Message::chatID;
    using Message::removeMenu;
    using Message::setMenu;

   protected:
    void makePacket(fb::Packet& p) {
        Message::makePacket(p);
        p.addInt(fbapi::message_id(), messageID);
    }
};

// https://core.telegram.org/bots/api#editmessagelivelocation
class EditLocation : private Message {
    friend class ::FastBot2;

   public:
    // id сообщения
    int32_t messageID;

    // широта
    float latitude;

    // долгота
    float longitude;

    // точность в метрах, 0-1500
    float horizontalAccuracy = NAN;

    // направление в градусах, 1-360
    uint16_t heading = 0;

    // Максимальное расстояние в метрах для оповещений о приближении к другому участнику чата
    uint32_t proximityAlertRadius = 0;

    using Message::chatID;
    using Message::removeMenu;
    using Message::setMenu;

   protected:
    void makePacket(fb::Packet& p) {
        Message::makePacket(p);
        p.addInt(fbapi::message_id(), messageID);
        p.addFloat(fbapi::latitude(), latitude, 6);
        p.addFloat(fbapi::longitude(), longitude, 6);
        if (!isnan(horizontalAccuracy)) p.addFloat(fbapi::horizontal_accuracy(), horizontalAccuracy, 1);
        if (heading) p.addInt(fbapi::heading(), heading);
        if (proximityAlertRadius) p.addInt(fbapi::proximity_alert_radius(), proximityAlertRadius);
    }
};

// https://core.telegram.org/bots/api#stopmessagelivelocation
class StopLocation : private Message {
    friend class ::FastBot2;

   public:
    // id сообщения
    int32_t messageID;

    using Message::chatID;
    using Message::removeMenu;
    using Message::setMenu;

   protected:
    void makePacket(fb::Packet& p) {
        Message::makePacket(p);
        p.addInt(fbapi::message_id(), messageID);
    }
};

}  // namespace fb