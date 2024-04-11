#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "VirtualFastBot2_class.h"
#include "core/api.h"
#include "core/packet.h"

// https://core.telegram.org/bots/api#setmycommands
namespace fb {

class MyCommands {
    friend class ::VirtualFastBot2;

   public:
    MyCommands() {}
    MyCommands(const String& commands, const String& description) : commands(commands), description(description) {}

    // список команд, длина команды 1-32, разделитель ;
    String commands = "";

    // список описаний команд, длина описания 1-256, разделитель ;
    String description = "";

    // зарезервировать строки
    void reserve(uint16_t len) {
        commands.reserve(len);
        description.reserve(len);
    }

    // добавить команду
    void addCommand(const String& command, const String& description) {
        if (_first) _first = false;
        else {
            commands += ';';
            this->description += ';';
        }

        commands += command;
        this->description += description;
    }

   private:
    void makePacket(Packet& p) const {
        p.beginArr(fb::api::commands);
        su::TextParser cmd(commands, ';');
        su::TextParser desc(description, ';');
        while (cmd.parse() && desc.parse()) {
            p.beginObj().addString(fb::api::command, cmd).addStringEsc(fb::api::description, desc).endObj();
        }
        p.endArr();
    }
    bool _first = true;
};

}  // namespace fb