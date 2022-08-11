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

#include "otpch.h"

#include "creatures/players/modules/server.h"
#include "game/game.h"
#include "config/configmanager.h"
#include "game/scheduling/tasks.h"
#include "lua/creature/creatureevent.h"
#include "creatures/players/grouping/party.h"
#include "creatures/creatures_definitions.h"
#include "game/game_definitions.h"

void PlayerServerModules::sendPendingStateEntered()
{
	auto packet = new ServerPacketPendingStateEntered(player);
	packet->sendToPlayer();
	delete packet;
}

void PlayerServerModules::sendWorldEntered()
{
	auto packet = new ServerPacketWorldEntered(player);
	packet->sendToPlayer();
	delete packet;
}

void PlayerServerModules::sendLoginError(const std::string &error)
{
	auto packet = new ServerPacketLoginError(player);
	packet->Error = error;
	packet->sendToPlayer();
	delete packet;
}

void PlayerServerModules::sendLoginAdvice(const std::string &advice)
{
	auto packet = new ServerPacketLoginAdvice(player);
	packet->Advice = advice;
	packet->sendToPlayer();
	delete packet;
}

void PlayerServerModules::sendLoginWait(const std::string &error, const uint8_t delay)
{
	auto packet = new ServerPacketLoginAdvice(player);
	packet->Error = advice;
	packet->Delay = delay;
	packet->sendToPlayer();
	delete packet;
}
