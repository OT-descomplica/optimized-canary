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

#ifndef SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_SETOUTFIT_HPP_
#define SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_SETOUTFIT_HPP_

#include "server/network/protocol/protocolgame/networkpacketbuffer.hpp"
#include "creatures/creatures_definitions.hpp"
#include "game/movement/position.h"

class ClientPacketSetOutfit final : public NetworkPacketBuffer
{
	public:
		constexpr ClientPacketSetOutfit(Player* sender) : NetworkPacketBuffer(sender) {};
		ClientPacketSetOutfit* getData() const override final {
			return this;
		}

		bool PodiumVisible;
		bool RandomMount;

		uint8_t Type;
		uint8_t StackPos;
		uint8_t Direction;

		uint16_t ItemId;

		uint32_t TryOutfitKey;

		Outfit_t Outfit;;
		Position Pos;

		bool ParseFromNetworkMessage(NetworkMessage &msg) override final {
			if (!IsInitialized()) {
				return false;
			}

			Type = msg.getByte();
			Outfit.lookType = msg.get<uint16_t>();
			Outfit.lookHead = std::min<uint8_t>(132, msg.getByte());
			Outfit.lookBody = std::min<uint8_t>(132, msg.getByte());
			Outfit.lookLegs = std::min<uint8_t>(132, msg.getByte());
			Outfit.lookFeet = std::min<uint8_t>(132, msg.getByte());
			Outfit.lookAddons = msg.getByte();
			if (Type == 0) {
				Outfit.lookMount = msg.get<uint16_t>();
				Outfit.lookMountHead = std::min<uint8_t>(132, msg.getByte());
				Outfit.lookMountBody = std::min<uint8_t>(132, msg.getByte());
				Outfit.lookMountLegs = std::min<uint8_t>(132, msg.getByte());
				Outfit.lookMountFeet = std::min<uint8_t>(132, msg.getByte());
				Outfit.lookFamiliarsType = msg.get<uint16_t>();
				RandomMount = msg.getByte() != 0x00;
			} else if (Type == 1) {
				TryOutfitKey = msg.get<uint32_t>();
			} else if (outfitType == 2) {
				Pos = msg.getPosition();
				ItemId = msg.get<uint16_t>();
				StackPos = msg.getByte();
				Outfit.lookMount = msg.get<uint16_t>();
				Outfit.lookMountHead = std::min<uint8_t>(132, msg.getByte());
				Outfit.lookMountBody = std::min<uint8_t>(132, msg.getByte());
				Outfit.lookMountLegs = std::min<uint8_t>(132, msg.getByte());
				Outfit.lookMountFeet = std::min<uint8_t>(132, msg.getByte());
				Direction = std::min<uint8_t>(3, msg.getByte());
				PodiumVisible = msg.getByte() != 0x00;
			}
			return true;
		}
};

#endif  // SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_SETOUTFIT_HPP_