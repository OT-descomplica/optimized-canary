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

#ifndef SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_GOPATH_HPP_
#define SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_GOPATH_HPP_

#include "server/network/protocol/protocolgame/networkpacketbuffer.hpp"
#include "game/movement/position.h"

class ClientPacketGoPath final : public NetworkPacketBuffer
{
	public:
		constexpr ClientPacketGoPath(Player* sender) : NetworkPacketBuffer(sender) {};
		ClientPacketGoPath* getData() const override final {
			return this;
		}

		uint8_t NumberOfDirections;
		std::forward_list<Direction> Path;

		bool ParseFromNetworkMessage(NetworkMessage &msg) override final {
			if (!IsInitialized()) {
				return false;
			}

			NumberOfDirections = msg.getByte();
			if (NumberOfDirections == 0 || (msg.getBufferPosition() + NumberOfDirections) != (msg.getLength() + 8)) {
				return false;
			}

			msg.skipBytes(NumberOfDirections);
			for (uint8_t i = 0; i < NumberOfDirections; ++i) {
				uint8_t rawdir = msg.getPreviousByte();
				if (rawdir == 1) {
					Path.push_front(DIRECTION_EAST);
				} else if (rawdir == 2) {
					Path.push_front(DIRECTION_NORTHEAST);
				} else if (rawdir == 3) {
					Path.push_front(DIRECTION_NORTH);
				} else if (rawdir == 4) {
					Path.push_front(DIRECTION_NORTHWEST);
				} else if (rawdir == 5) {
					Path.push_front(DIRECTION_WEST);
				} else if (rawdir == 6) {
					Path.push_front(DIRECTION_SOUTHWEST);
				} else if (rawdir == 7) {
					Path.push_front(DIRECTION_SOUTH);
				} else if (rawdir == 8) {
					Path.push_front(DIRECTION_SOUTHEAST);
				} else {
					SPDLOG_WARN("[ClientPacketGoPath::ParseFromNetworkMessage] - "
								"Invalid ENUM({}) 'rawdir' value received from player {}", rawdir, player->getName());
					return false;
				}
			}

			return true;
		}
};

#endif  // SRC_SERVER_NETWORK_PROTOCOL_PROTOCOLGAME_CLIENTPACKETS_GOPATH_HPP_