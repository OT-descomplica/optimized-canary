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

#ifndef SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_APPLYIMBUEMENT_HPP_
#define SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_APPLYIMBUEMENT_HPP_

#include "server/network/protocol/protocolgame/networkpacketbuffer.hpp"

class ClientPacketApplyImbuement final : public NetworkPacketBuffer
{
	public:
		constexpr ClientPacketApplyImbuement(Player* sender) : NetworkPacketBuffer(sender) {};
		ClientPacketApplyImbuement* getData() const override final {
			return this;
		}

		bool Mount;

		uint8_t Slot;

		uint32_t ImbuementId;

		bool ParseFromNetworkMessage(NetworkMessage &msg) override final {
			if (!IsInitialized()) {
				return false;
			}

			Slot = msg.getByte();
			ImbuementId = msg.get<uint32_t>();
			ProtectionCharm = msg.getByte() != 0x00;
			return true;
		}
};

#endif  // SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_APPLYIMBUEMENT_HPP_