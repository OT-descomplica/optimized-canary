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

#ifndef SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_PARTYHUNTANALYZER_HPP_
#define SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_PARTYHUNTANALYZER_HPP_

#include "server/network/protocol/protocolgame/networkpacketbuffer.hpp"

class ClientPacketPartyHuntAnalyzer final : public NetworkPacketBuffer
{
	public:
		constexpr ClientPacketPartyHuntAnalyzer(Player* sender) : NetworkPacketBuffer(sender) {};
		ClientPacketPartyHuntAnalyzer* getData() const override final {
			return this;
		}

		uint8_t Action;

		std::map<uint16_t, uint64_t> ItemPrices;

		void ParseFromNetworkMessage(NetworkMessage &msg) override final {
			if (!IsInitialized()) {
				return;
			}

			Action = msg.getByte();
			if (Action == 2) {
				uint16_t size = msg.get<uint16_t>();
				for (uint16_t i = 1; i <= size; i++) {
					uint16_t itemId = msg.get<uint16_t>();
					uint64_t price = msg.get<uint64_t>();
					ItemPrices[itemId] = price;
				}
			}
		}
};

#endif  // SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_PARTYHUNTANALYZER_HPP_