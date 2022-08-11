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

#ifndef SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_USEONCREATURE_HPP_
#define SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_USEONCREATURE_HPP_

#include "server/network/protocol/protocolgame/networkpacketbuffer.hpp"
#include "game/movement/position.h"

class ClientPacketUseOnCreature final : public NetworkPacketBuffer
{
	public:
		constexpr ClientPacketUseOnCreature(Player* sender) : NetworkPacketBuffer(sender) {};
		ClientPacketUseOnCreature* getData() const override final {
			return this;
		}

		uint8_t FromStackPos;

		uint16_t ItemId;

		uint32_t CreatureId;

		Position FromPosition;

		bool ParseFromNetworkMessage(NetworkMessage &msg) override final {
			if (!IsInitialized()) {
				return false;
			}

			FromPosition = msg.getPosition();
			ItemId = msg.get<uint16_t>();
			FromStackPos = msg.getByte();
			CreatureId = msg.get<uint32_t>();
			return true;
		}
};

#endif  // SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_USEONCREATURE_HPP_