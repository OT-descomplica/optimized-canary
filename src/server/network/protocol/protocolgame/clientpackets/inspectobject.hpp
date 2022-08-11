/**
 * The Forgotten Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2019  Mark Samman <mark.samman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_INSPECTOBJECT_HPP_
#define SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_INSPECTOBJECT_HPP_

#include "server/network/protocol/protocolgame/networkpacketbuffer.hpp"
#include "game/movement/position.h"

class ClientPacketInspectObject final : public NetworkPacketBuffer
{
	public:
		constexpr ClientPacketInspectObject(Player* sender) : NetworkPacketBuffer(sender) {};
		ClientPacketInspectObject* getData() const override final {
			return this;
		}

		uint8_t Type;

		uint16_t Count;
		uint16_t ItemId;

		Position Pos;

		bool ParseFromNetworkMessage(NetworkMessage &msg) override final {
			if (!IsInitialized()) {
				return false;
			}

			Type = msg.getByte();
			if (Type == 0) {
				Pos = msg.getPosition();
			} else if (Type == 1) {
				ItemId = msg.get<uint16_t>();
				Count = msg.getByte();
			} else if (Type == 3) {
				ItemId = msg.get<uint16_t>();
				Count = msg.getByte();
			}
			return true;
		}
};

#endif  // SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_INSPECTOBJECT_HPP_