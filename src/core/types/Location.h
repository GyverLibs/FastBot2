#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "../api.h"
#include "ID.h"
#include "Message.h"

// https://core.telegram.org/bots/api#sendlocation
namespace fb {

class Location : private Message {
    friend class ::FastBot2Client;

   public:
    Location();
    Location(float latitude, float longitude, ID chatID) : latitude(latitude), longitude(longitude) {
        this->chatID = chatID;
    }

    using Message::chatID;
    using Message::json;
    using Message::notification;
    using Message::protect;
    using Message::removeMenu;
    using Message::reply;
    using Message::setInlineMenu;
    using Message::setMenu;
    using Message::threadID;

    // широта
    float latitude;

    // долгота
    float longitude;

    // точность в метрах, 0-1500
    float horizontalAccuracy = NAN;

    // период обновления локации в секундах 60.. 86400
    uint32_t livePeriod = 0;

    // направление в градусах, 1-360
    uint16_t heading = 0;

    // Максимальное расстояние в метрах для оповещений о приближении к другому участнику чата
    uint32_t proximityAlertRadius = 0;

   protected:
    void makePacket(Packet& p) const {
        Message::makePacket(p);
        p[tg_api::latitude].add(latitude, 6);
        p[tg_api::longitude].add(longitude, 6);
        if (!isnan(horizontalAccuracy)) p[tg_api::horizontal_accuracy].add(horizontalAccuracy, 1);
        if (livePeriod) p[tg_api::live_period] = livePeriod;
        if (heading) p[tg_api::heading] = heading;
        if (proximityAlertRadius) p[tg_api::proximity_alert_radius] = proximityAlertRadius;
    }
};

}  // namespace fb