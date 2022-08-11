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

#ifndef SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_LOOTCONTAINER_HPP_
#define SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_LOOTCONTAINER_HPP_

#include "server/network/protocol/protocolgame/networkpacketbuffer.hpp"
#include "game/movement/position.h"

class ClientPacketLootContainer final : public NetworkPacketBuffer
{
	public:
		constexpr ClientPacketLootContainer(Player* sender) : NetworkPacketBuffer(sender) {};
		ClientPacketLootContainer* getData() const override final {
			return this;
		}

		bool UseMainAsFallback;

		uint8_t Action;
		uint8_t StackPos;
		uint8_t Category;

		uint16_t ItemId;

		Position Pos;

		bool ParseFromNetworkMessage(NetworkMessage &msg) override final {
			if (!IsInitialized()) {
				return false;
			}

			Action = msg.getByte();
			if (Action == 0) {
				Category = msg.getByte();
				Pos = msg.getPosition();
				ItemId = msg.get<uint16_t>();
				StackPos = msg.getByte();
			} else if (Action == 1) {
				Category = msg.getByte();
			} else if (Action == 2) {
				Category = msg.getByte();
			} else if (Action == 3) {
				UseMainAsFallback = msg.getByte() != 0x00;
			}
			return true;
		}
};

#endif  // SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_LOOTCONTAINER_HPP_