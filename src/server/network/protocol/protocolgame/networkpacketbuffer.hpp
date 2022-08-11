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

#ifndef SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_NETWORKPACKETBUFFER_HPP_
#define SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_NETWORKPACKETBUFFER_HPP_

#include "otpch.h"
#include "game/game.h"
#include "declarations.hpp"
#include "creatures/players/player.h"
#include "server/network/protocol/protocol.h"
#include "server/network/protocol/protocolgame/network_definitions.hpp"

class Game;
class Player;
class NetworkMessage;

class NetworkPacketBuffer final : public Protocol
{
	public:
		constexpr NetworkPacketBuffer(Player* sender) :  player(sender) {};

		virtual NetworkPacketBuffer* getData() {
			return nullptr;
		}

		virtual bool ParseFromNetworkMessage(NetworkMessage &msg) {}
		virtual bool AppendToNetworkMessage(NetworkMessage &msg) {}
		virtual bool IsEnabled() const {}
		bool IsInitialized() const {
			return player != nullptr && player->client != nullptr;
		}

		void setServerPacket(NetworkServerPacket_t packet) {
			serverPacket = packet;
		}
		NetworkServerPacket_t getServerPacket() const {
			return serverPacket;
		}

		void sendToPlayer() {
			if (packet->AppendToNetworkMessage() && msg.getLenght() > 0) {
				player->sendNetworkMessage(msg);
				msg.reset();
			}
		}

	private:
		Player* player;
		NetworkMessage msg;
		NetworkServerPacket_t serverPacket = NETWORK_SERVERPACKET_INVALID;
};

#endif  // SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_NETWORKPACKETBUFFER_HPP_