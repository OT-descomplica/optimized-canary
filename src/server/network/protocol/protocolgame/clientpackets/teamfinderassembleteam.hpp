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

#ifndef SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_TEAMFINDERASSEMBLETEAM_HPP_
#define SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_TEAMFINDERASSEMBLETEAM_HPP_

#include "server/network/protocol/protocolgame/networkpacketbuffer.hpp"

class ClientPacketTeamFinderAssembleTeam final : public NetworkPacketBuffer
{
	public:
		constexpr ClientPacketTeamFinderAssembleTeam(Player* sender) : NetworkPacketBuffer(sender) {};
		ClientPacketTeamFinderAssembleTeam* getData() const override final {
			return this;
		}

		bool PartyBool;

		uint8_t Action;
		uint8_t TeamType;
		uint8_t VocationIds;
		uint8_t MemberStatus;

		uint16_t BossId;
		uint16_t QuestId;
		uint16_t MinLevel;
		uint16_t MaxLevel;
		uint16_t HuntType;
		uint16_t HuntArea;
		uint16_t TeamSlots;
		uint16_t FreeSlots;

		uint32_t MemberId;
		uint32_t Timestamp;

		bool ParseFromNetworkMessage(NetworkMessage &msg) override final {
			if (!IsInitialized()) {
				return false;
			}

			Action = msg.getByte();
			if (Action == 2) {
				MemberId = msg.get<uint32_t>();
				MemberStatus = msg.getByte();
			} else if (Action == 3) {
				MinLevel = msg.get<uint16_t>();
				MaxLevel = msg.get<uint16_t>();
				VocationIds = msg.getByte();
				TeamSlots = msg.get<uint16_t>();
				FreeSlots = msg.get<uint16_t>();
				PartyBool = (msg.getByte() != 0);
				Timestamp = msg.get<uint32_t>();
				TeamType = msg.getByte();

				if (TeamType == 1) {
					BossId = msg.get<uint16_t>();
				} else if (TeamType == 2) {
					HuntType = msg.get<uint16_t>();
					HuntArea = msg.get<uint16_t>();
				} else if (TeamType == 3) {
					QuestId = msg.get<uint16_t>();
				}
			}

			return true;
		}
};

#endif  // SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_TEAMFINDERASSEMBLETEAM_HPP_