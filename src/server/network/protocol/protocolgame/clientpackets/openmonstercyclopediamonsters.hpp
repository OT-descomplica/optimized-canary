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

#ifndef SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_OPENMONSTERCYCLOPEDIAMONSTERS_HPP_
#define SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_OPENMONSTERCYCLOPEDIAMONSTERS_HPP_

#include "server/network/protocol/protocolgame/networkpacketbuffer.hpp"

class ClientPacketOpenMonsterCyclopediaMonsters final : public NetworkPacketBuffer
{
	public:
		constexpr ClientPacketOpenMonsterCyclopediaMonsters(Player* sender) : NetworkPacketBuffer(sender) {};
		ClientPacketOpenMonsterCyclopediaMonsters* getData() const override final {
			return this;
		}

		uint8_t Type;

		std::string Name;
		std::vector<uint16_t> RaceIds;

		bool ParseFromNetworkMessage(NetworkMessage &msg) override final {
			if (!IsInitialized()) {
				return false;
			}

			Type = msg.getByte();
			if (Type == 1) {
				uint16_t size = msg.get<uint16_t>();
				RaceIds.resize(size);
				for (int i = 0; i < size; i++) {
					RaceIds.push_back(msg.get<uint16_t>());
				}
			} else {
				Name = msg.getString();
			}
			return true;
		}
};

#endif  // SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_OPENMONSTERCYCLOPEDIAMONSTERS_HPP_