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

#ifndef SRC_CREATURES_PLAYERS_MODULES_SERVERMODULES_H_
#define SRC_CREATURES_PLAYERS_MODULES_SERVERMODULES_H_

#include "creatures/players/player.h"
#include "creatures/creatures_definitions.hpp"
#include "server/network/protocol/protocolgame/network_definitions.hpp"
#include "server/network/protocol/protocolgame/serverpackets/init.hpp"

class PlayerServerModules
{
	public:
		explicit PlayerServerModules(Player* initPlayer) : player(initPlayer) {};

		void sendPendingStateEntered();
		void sendWorldEntered();
		void sendLoginError(const std::string &error);
		void sendLoginAdvice(const std::string &advice);
		void sendLoginWait(const std::string &error, const uint8_t delay);

	private:
		Player* player;
};

#endif  // SRC_CREATURES_PLAYERS_MODULES_SERVERMODULES_H_
