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

#include "creatures/players/modules/client.h"
#include "game/game.h"
#include "config/configmanager.h"
#include "game/scheduling/tasks.h"
#include "lua/creature/creatureevent.h"
#include "creatures/players/grouping/party.h"
#include "creatures/creatures_definitions.h"
#include "game/game_definitions.h"

// Binders region
template <typename Callable, typename... Args>
void PlayerClientModules::addGameTask(Callable function, Args &&... args)
{
	g_dispatcher().addTask(createTask(std::bind(function, &g_game(), std::forward<Args>(args)...)));
}
// End region

void PlayerClientModules::ParseFromNetworkData(const ClientPacketDepotSearchRetrieve* data)
{
	addGameTask(&Game::playerRequestDepotSearchRetrieve, player->getID(), data->ItemId, data->ItemTier, data->Type);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGoEast* data)
{
	addGameTask(&Game::playerMove, player->getID(), DIRECTION_EAST);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGoNorth* data)
{
	addGameTask(&Game::playerMove, player->getID(), DIRECTION_NORTH);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGoPath* data)
{
	if (!(data->Path.empty())) {
		addGameTask(&Game::playerAutoWalk, player->getID(), data->Path);
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGoSouth* data)
{
	addGameTask(&Game::playerMove, player->getID(), DIRECTION_SOUTH);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGoWest* data)
{
	addGameTask(&Game::playerMove, player->getID(), DIRECTION_WEST);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketPartyHuntAnalyzer* data)
{
	Party* party = player->getParty();
	if (!party || !party->getLeader() || party->getLeader()->getID() != player->getID()) {
		return;
	}

	// Reset
	if (data->Action == 0) {
		party->resetAnalyzer();

	// Price type change
	} else if (data->Action == 1) {
		party->switchAnalyzerPriceType();

	// Price value
	} else if (data->Action == 2) {
		for (const auto& [ItemId, Price] : data->ItemPrices) {
			player->setItemCustomPrice(ItemId, Price);
		}

		party->reloadPrices();
		party->updateTrackerAnalyzer();
	} else {
		SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkMessage::ClientPacketPartyHuntAnalyzer] - "
					"Invalid ENUM({}) 'action' value received from player {}", data->Action, player->getName());
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketPing* data)
{
	player->sendPingBack();
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketPingBack* data)
{
	player->receivePing();
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketQuitGame* data)
{
	bool removePlayer = !player->isRemoved() && !data->Forced;
	if (removePlayer && !player->isAccessPlayer()) {
		if (player->getTile()->hasFlag(TILESTATE_NOLOGOUT)) {
			player->sendCancelMessage(RETURNVALUE_YOUCANNOTLOGOUTHERE);
			return;
		}

		if (!player->getTile()->hasFlag(TILESTATE_PROTECTIONZONE) && player->hasCondition(CONDITION_INFIGHT)) {
			player->sendCancelMessage(RETURNVALUE_YOUMAYNOTLOGOUTDURINGAFIGHT);
			return;
		}
	}

	if (removePlayer && !g_creatureEvents().playerLogout(player)) {
		return;
	}

	bool displayEffect = data->DisplayEffect && !player->isRemoved() && player->getHealth() > 0 && !player->isInGhostMode();
	if (displayEffect) {
		g_game().addMagicEffect(player->getPosition(), CONST_ME_POFF);
	}

	player->client->sendSessionEndInformation(Forced ? SESSION_END_FORCECLOSE : SESSION_END_LOGOUT);

	g_game().removeCreature(player, true);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketExtendedOpCode* data)
{
	// process additional opcodes via lua script event
	addGameTask(&Game::parsePlayerExtendedOpcode, player->getID(), data->OpCode, data->Buffer);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketStashAction* data)
{
	if (!player->isSupplyStashMenuAvailable()) {
		player->sendCancelMessage("You can't use supply stash right now.");
		return;
	}

	if (player->isStashExhausted()) {
		player->sendCancelMessage("You need to wait to do this again.");
		return;
	}

	// Stow item
	if (data->Action == 0) {
		addGameTask(&Game::playerStowItem, player->getID(), data->Pos, data->ItemId, data->Stackpos, data->Count, false);

	// Stow container
	} else if (data->Action == 1) {
		addGameTask(&Game::playerStowItem, player->getID(), data->Pos, data->ItemId, data->Stackpos, 0, false);

	// Stow stack
	} else if (data->Action == 2) {
		addGameTask(&Game::playerStowItem, player->getID(), data->Pos, data->ItemId, data->Stackpos, 0, true);

	// Withdraw
	} else if (data->Action == 3) {
		addGameTask(&Game::playerStashWithdraw, player->getID(), data->ItemId, data->Count, data->Stackpos);
	} else {
		SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkMessage::ClientPacketStashAction] - "
					"Invalid ENUM({}) 'action' value received from player {}", data->Action, player->getName());
	}

	player->updateStashExhausted();
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketTeamFinderAssembleTeam* data)
{
	if (data->Action == 0) {
		player->sendLeaderTeamFinder(false);
	} else if (data->Action == 1) {
		player->sendLeaderTeamFinder(true);
	} else if (data->Action == 2) {
		const Player* member = g_game().getPlayerByGUID(data->MemberId);
		if (!member) {
			return;
		}

		std::map<uint32_t, TeamFinder*> teamFinder = g_game().getTeamFinderList();
		TeamFinder* teamAssemble = nullptr;
		if (auto it = teamFinder.find(player->getGUID()); it != teamFinder.end()) {
			teamAssemble = it->second;
		}

		if (!teamAssemble) {
			return;
		}

		for (auto& memberPair : teamAssemble->membersMap) {
			if (memberPair.first == data->MemberId) {
				memberPair.second = data->MemberStatus;
			}
		}

		if (data->MemberStatus == 2) {
			member->sendTextMessage(MESSAGE_STATUS, "You are invited to a new team.");
		} else if (data->MemberStatus == 3) {
			member->sendTextMessage(MESSAGE_STATUS, "Your team finder request was accepted.");
		} else if (data->MemberStatus == 4) {
			member->sendTextMessage(MESSAGE_STATUS, "Your team finder request was denied.");
		} else {
			SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkMessage::ClientPacketTeamFinderAssembleTeam] - "
						"Invalid ENUM({}) 'memberStatus' value received from player {}", data->MemberStatus, player->getName());
		}

		player->sendLeaderTeamFinder(false);
	} else if (data->Action == 3) {
		std::map<uint32_t, uint8_t> members;
		std::map<uint32_t, TeamFinder*> teamFinder = g_game().getTeamFinderList();
		TeamFinder* teamAssemble = nullptr;
		if (auto it = teamFinder.find(player->getGUID()); it != teamFinder.end()) {
			members = it->second->membersMap;
			teamAssemble = it->second;
		}

		if (!teamAssemble) {
			teamAssemble = new TeamFinder();
		}

		teamAssemble->minLevel = data->MinLevel;
		teamAssemble->maxLevel = data->MaxLevel;
		teamAssemble->vocationIDs = data->VocationIds;
		teamAssemble->teamSlots = data->TeamSlots;
		teamAssemble->freeSlots = data->FreeSlots;
		teamAssemble->partyBool = data->PartyBool;
		teamAssemble->timestamp = data->Timestamp;
		teamAssemble->teamType = data->TeamType;

		if (teamAssemble->teamType == 1) {
			teamAssemble->bossID = data->BossId;
		} else if (teamAssemble->teamType == 2) {
			teamAssemble->hunt_type = data->HuntType;
			teamAssemble->hunt_area = data->HuntArea;
		} else if (teamAssemble->teamType == 3) {
			teamAssemble->questID = data->QuestId;
		} else {
			SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkMessage::ClientPacketTeamFinderAssembleTeam] - "
						"Invalid ENUM({}) 'teamAssemble->teamType' value received from player {}", teamAssemble->teamType, player->getName());
		}

		teamAssemble->leaderGuid = player->getGUID();
		if (teamAssemble->partyBool && player->getParty()) {
			for (Player* member : player->getParty()->getMembers()) {
				if (member && member->getGUID() != player->getGUID()) {
					members.insert({member->getGUID(), 3});
				}
			}
			if (player->getParty()->getLeader()->getGUID() != player->getGUID()) {
				members.insert({player->getParty()->getLeader()->getGUID(), 3});
			}
		}

		teamAssemble->membersMap = members;
		g_game().registerTeamFinderAssemble(player->getGUID(), teamAssemble);


		player->sendLeaderTeamFinder(false);
	} else {
		SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkMessage::ClientPacketTeamFinderAssembleTeam] - "
					"Invalid ENUM({}) 'action' value received from player {}", data->Action, player->getName());
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketTeamFinderJoinTeam* data)
{
	// Open window
	if (data->Action == 0) {
		player->sendTeamFinderList();

	// Join request
	} else if (data->Action == 1) {
		const Player* leader = g_game().getPlayerByGUID(data->LeaderId);
		if (!leader) {
			return;
		}

		std::map<uint32_t, TeamFinder*> teamFinder = g_game().getTeamFinderList();
		TeamFinder* teamAssemble = nullptr;
		if (auto it = teamFinder.find(data->LeaderId); it != teamFinder.end()) {
			teamAssemble = it->second;
		}

		if (!teamAssemble) {
			return;
		}

		leader->sendTextMessage(MESSAGE_STATUS, "There is a new request to join your team.");
		teamAssemble->membersMap.insert({player->getGUID(), 1});
		player->sendTeamFinderList();

	// Leave team
	} else if (data->Action == 2) {
		const Player* leader = g_game().getPlayerByGUID(data->LeaderId);
		if (!leader) {
			return;
		}

		std::map<uint32_t, TeamFinder*> teamFinder = g_game().getTeamFinderList();
		TeamFinder* teamAssemble = nullptr;
		if (auto it = teamFinder.find(data->LeaderId); it != teamFinder.end()) {
			teamAssemble = it->second;
		}

		if (!teamAssemble) {
			return;
		}
	
		for (auto teamIt = teamAssemble->membersMap.begin(), end = teamAssemble->membersMap.end(); teamIt != end; ++teamIt) {
			if (teamIt->first == player->getGUID()) {
				teamAssemble->membersMap.erase(teamIt);
				break;
			}
		}

		player->sendTeamFinderList();
	} else {
		SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkMessage::ClientPacketTeamFinderJoinTeam] - "
					"Invalid ENUM({}) 'action' value received from player {}", data->Action, player->getName());
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketTrackBestiaryRace* data)
{
	const std::map<uint16_t, std::string> mtype_list = g_game().getBestiaryList();
	if (auto it = mtype_list.find(data->RaceId); it != mtype_list.end()) {
		if (MonsterType *mtype = g_monsters().getMonsterType(it->second);) {
			player->addBestiaryTrackerList(mtype);
		}
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketStop* data)
{
	addGameTask(&Game::playerStopAutoWalk, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGoNorthEast* data)
{
	addGameTask(&Game::playerMove, player->getID(), DIRECTION_NORTHEAST);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGoSouthEast* data)
{
	addGameTask(&Game::playerMove, player->getID(), DIRECTION_SOUTHEAST);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGoSouthWest* data)
{
	addGameTask(&Game::playerMove, player->getID(), DIRECTION_SOUTHWEST);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGoNorthWest* data)
{
	addGameTask(&Game::playerMove, player->getID(), DIRECTION_NORTHWEST);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketRotateNorth* data)
{
	addGameTask(&Game::playerTurn, player->getID(), DIRECTION_NORTH); 
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketRotateEast* data)
{
	addGameTask(&Game::playerTurn, player->getID(), DIRECTION_EAST); 
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketRotateSouth* data)
{
	addGameTask(&Game::playerTurn, player->getID(), DIRECTION_SOUTH); 
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketRotateWest* data)
{
	addGameTask(&Game::playerTurn, player->getID(), DIRECTION_WEST); 
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketTeleport* data)
{
	addGameTask(&Game::playerTeleport, player->getID(), data->Pos);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketEquipObject* data)
{
	addGameTask(&Game::onPressHotkeyEquip, player->getID(), date->ItemId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketMoveObject* data)
{
	if (data->ToPosition != data->FromPosition) {
		addGameTask(&Game::playerMoveThing, player->getID(), data->FromPosition, data->ItemId, data->FromStackPos, data->ToPosition, data->Count);
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketLookNpcTrade* data)
{
	addGameTask(&Game::playerLookInShop, player->getID(), data->ItemId, data->Count);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketBuyObject* data)
{
	addGameTask(&Game::playerBuyItem, player->getID(), data->ItemId, data->Count, data->Amount, data->IgnoreCap, data->InBackpacks);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketSellObject* data)
{
	addGameTask(&Game::playerSellItem, player->getID(), data->ItemId, data->Count, data->Amount, data->IgnoreEquipped);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketCloseNpcTrade* data)
{
	addGameTask(&Game::playerCloseShop, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketTradeObject* data)
{
	addGameTask(&Game::playerRequestTrade, player->getID(), data->Pos, data->Stackpos, data->PlayerId, data->ItemId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketLookTrade* data)
{
	addGameTask(&Game::playerLookInTrade, player->getID(), data->CounterOffer, data->Index);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketAcceptTrade* data)
{
	addGameTask(&Game::playerAcceptTrade, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketRejectTrade* data)
{
	addGameTask(&Game::playerCloseTrade, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketUseObject* data)
{
	addGameTask(&Game::playerUseItem, player->getID(), data->Pos, data->Stackpos, data->Index, data->ItemId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketUseTwoObject* data)
{
	addGameTask(&Game::playerUseItemEx, player->getID(), data->FromPosition, data->FromStackPos, data->FromItemId, data->ToPosition, data->ToStackPos, data->ToItemId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketUseOnCreature* data)
{
	addGameTask(&Game::playerUseWithCreature, player->getID(), data->FromPosition, data->FromStackPos, data->CreatureId, data->ItemId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketTurnObject* data)
{
	addGameTask(&Game::playerRotateItem, player->getID(), data->Pos, data->Stackpos, data->ItemId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketShowOffSocket* data)
{
	addGameTask(&Game::playerConfigureShowOffSocket, player->getID(), data->Pos, data->Stackpos, data->ItemId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketCloseContainer* data)
{
	addGameTask(&Game::playerCloseContainer, player->getID(), data->ContainerId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketUpContainer* data)
{
	addGameTask(&Game::playerMoveUpContainer, player->getID(), data->ContainerId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketEditText* data)
{
	addGameTask(&Game::playerWriteItem, player->getID(), data->WindowTextId, data->Text);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketEditList* data)
{
	addGameTask(&Game::playerUpdateHouseWindow, player->getID(), data->DoorId, data->Id, data->Text);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketToggleWrapState* data)
{
	addGameTask(&Game::playerWrapableItem, player->getID(), data->Pos, data->Stackpos, data->ItemId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketLook* data)
{
	addGameTask(&Game::playerLookAt, player->getID(), data->ItemId, data->Pos, data->Stackpos);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketLookAtCreature* data)
{
	addGameTask(&Game::playerLookInBattleList, player->getID(), data-CreatureId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketQuickLoot* data)
{
	addGameTask(&Game::playerQuickLoot, player->getID(), data->Pos, data->ItemId, data->Stackpos, nullptr, data->LootAllCorpses, data->AutoLoot);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketLootContainer* data)
{
	if (data->Action == 0) {
		addGameTask(&Game::playerSetLootContainer, player->getID(), static_cast<ObjectCategory_t>(data->Category), data->Pos, data->ItemId, data->Stackpos);
	} else if (data->Action == 1) {
		addGameTask(&Game::playerClearLootContainer, player->getID(), static_cast<ObjectCategory_t>(data->Category));
	} else if (data->Action == 2) {
		addGameTask(&Game::playerOpenLootContainer, player->getID(), static_cast<ObjectCategory_t>(data->Category));
	} else if (data->Action == 3) {
		addGameTask(&Game::playerSetQuickLootFallback, player->getID(), data->UseMainAsFallback);
	} else {
		SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkMessage::ClientPacketLootContainer] - "
					"Invalid ENUM({}) 'action' value received from player {}", data->Action, player->getName());
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketQuickLootBlackWhiteList* data)
{
	addGameTask(&Game::playerQuickLootBlackWhitelist, player->getID(), data->Filter, data->ListedItems);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketOpenDepotSearch* data)
{
	addGameTask(&Game::playerRequestDepotItems, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketCloseDepotSearch* data)
{
	addGameTask(&Game::playerRequestCloseDepotSearch, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketDepotSearchType* data)
{
	addGameTask(&Game::playerRequestDepotSearchItem, player->getID(), data->ItemId, data->ItemTier);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketOpenParentContainer* data)
{
	addGameTask(&Game::playerRequestOpenContainerFromDepotSearch, player->getID(), data->Pos);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketTalk* data)
{
	addGameTask(&Game::playerSay, player->getID(), data->ChannelId, static_cast<SpeakClasses>(data->Type), data->Receiver, data->Text);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGetChannels* data)
{
	addGameTask(&Game::playerRequestChannels, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketJoinChannel* data)
{
	addGameTask(&Game::playerOpenChannel, player->getID(), data->ChannelId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketLeaveChannel* data)
{
	addGameTask(&Game::playerCloseChannel, player->getID(), data->ChannelId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketPrivateChannel* data)
{
	addGameTask(&Game::playerOpenPrivateChannel, player->getID(), data->Receiver);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketCloseNpcChannel* data)
{
	addGameTask(&Game::playerCloseNpcChannel, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketSetTatics* data)
{
	FightMode_t fightMode;
	if (data->FightMode == 1) {
		fightMode = FIGHTMODE_ATTACK;
	} else if (data->FightMode == 2) {
		fightMode = FIGHTMODE_BALANCED;
	} else {
		fightMode = FIGHTMODE_DEFENSE;
	}

	addGameTask(&Game::playerSetFightModes, player->getID(), fightMode, data->ChaseMode != 0, data->SecureMode != 0);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketAttack* data)
{
	addGameTask(&Game::playerSetAttackedCreature, player->getID(), data->CreatureId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketFollow* data)
{
	addGameTask(&Game::playerFollowCreature, player->getID(), data->CreatureId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketInviteToParty* data)
{
	addGameTask(&Game::playerInviteToParty, player->getID(), data->TargetId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketJoinParty* data)
{
	addGameTask(&Game::playerJoinParty, player->getID(), data->TargetId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketRevokeInvitation* data)
{
	addGameTask(&Game::playerRevokePartyInvitation, player->getID(), data->TargetId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketPassLeadership* data)
{
	addGameTask(&Game::playerPassPartyLeadership, player->getID(), data->TargetId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketLeaveParty* data)
{
	addGameTask(&Game::playerLeaveParty, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketShareExperience* data)
{
	addGameTask(&Game::playerEnableSharedPartyExperience, player->getID(), data->SharedExpActive);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketOpenChannel* data)
{
	addGameTask(&Game::playerCreatePrivateChannel, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketInviteToChannel* data)
{
	addGameTask(&Game::playerChannelInvite, player->getID(), data->Name);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketExcludeFromChannel* data)
{
	addGameTask(&Game::playerChannelExclude, player->getID(), data->Name);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketHighscores* data)
{
	addGameTask(&Game::playerHighscores, player->getID(), static_cast<HighscoreType_t>(type), data->Category, data->Vocation, data->WorldName, std::max<uint16_t>(1, data->Page), std::min<uint8_t>(30, std::max<uint8_t>(5, data->EntriesPerPage)));
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketPreyHuntingTaskAction* data)
{
	if (!g_configManager().getBoolean(TASK_HUNTING_ENABLED)) {
		return;
	}

	addGameTask(&Game::playerTaskHuntingAction, player->getID(), data->Slot, data->Action, data->Upgrade, data->RaceId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketCancel* data)
{
	addGameTask(&Game::playerCancelAttackAndFollow, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketBrowseField* data)
{
	addGameTask(&Game::playerBrowseField, player->getID(), data->Pos);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketSeekInContainer* data)
{
	addGameTask(&Game::playerSeekInContainer, player->getID(), data->ContainerId, data->Index);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketInspectObject* data)
{
	// Normal object
	if (data->Type == 0) {
		addGameTask(&Game::playerInspectItem, player->getID(), data->Pos);
	
	// Npc trade
	} else if (data->Type == 1) {
		addGameTask(&Game::playerInspectItem, player->getID(), data->ItemId, static_cast<int8_t>(data->Count), false);
	
	// Cyclopedia
	} else if (data->Type == 3) {
		addGameTask(&Game::playerInspectItem, player->getID(), data->ItemId, static_cast<int8_t>(data->Count), true);
	} else {
		SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkMessage::ClientPacketInspectObject] - "
					"Invalid ENUM({}) 'type' value received from player {}", data->Type, player->getName());
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGetOutfit* data)
{
	addGameTask(&Game::playerRequestOutfit, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketSetOutfit* data)
{
	if (!player || player->isRemoved()) {
		return;
	}

	if (data->Type == 0) {
		addGameTask(&Game::playerChangeOutfit, player->getID(), data->Outfit);
	} else if (data->Type == 1) {
		//This value probably has something to do with try outfit variable inside outfit window dialog
		//if try outfit is set to 2 it expects uint32_t value after mounted and disable mounts from outfit window dialog
	} else if (data->Type == 2) {
		addGameTask(&Game::playerSetShowOffSocket, player->getID(), data->Outfit, data->Pos, data->Stackpos, data->ItemId, data->PodiumVisible, data->Direction);
	} else {
		SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkMessage::ClientPacketSetOutfit] - "
					"Invalid ENUM({}) 'type' value received from player {}", data->Type, player->getName());
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketMount* data)
{
	addGameTask(&Game::playerToggleMount, player->getID(), data->Mount);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketApplyImbuement* data)
{
	addGameTask(&Game::playerApplyImbuement, player->getID(), data->ImbuementId, data->Slot, data->ProtectionCharm);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketApplyClearingCharm* data)
{
	addGameTask(&Game::playerClearImbuement, player->getID(), data->Slot);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketCloseImbuingDialog* data)
{
	addGameTask(&Game::playerCloseImbuementWindow, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketAddBuddy* data)
{
	addGameTask(&Game::playerRequestAddVip, player->getID(), data->Name);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketRemoveBuddy* data)
{
	addGameTask(&Game::playerRequestRemoveVip, player->getID(), data->CreatureId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketEditBuddy* data)
{
	addGameTask(&Game::playerRequestEditVip, player->getID(), data->CreatureId, data->Description, std::min<uint32_t>(10, data->Icon), data->Notify);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketOpenMonsterCyclopedia* data)
{
	addGameTask(&Game::playerRequestMonsterCyclopedia, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketOpenMonsterCyclopediaMonsters* data)
{
	std::map<uint16_t, std::string> race;

	if (data->Type == 1) {
		std::map<uint16_t, std::string> bestiaryList = g_game().getBestiaryList();
		for (uint16_t raceId : data->RaceIds) {
			if (player->getBestiaryKillCount(raceId) > 0) {
				if (auto it = bestiaryList.find(raceId); it != bestiaryList.end()) {
					race.insert({raceId, it->second});
				}
			}
		}
	} else {
		race = g_iobestiary().findRaceByName(data->Name);
		if (race.size() == 0) {
			SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkData::ClientPacketOpenMonsterCyclopediaMonsters] - "
                        "Race was not found '{}' with 'type' '{}'", data->Name, data->Type);
			return;
		}
	}

	addGameTask(&Game::playerRequestMonsterCyclopediaMonsters, player->getID(), data->Name, race);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketOpenMonsterCyclopediaRace* data)
{
	addGameTask(&Game::playerRequestMonsterCyclopediaRace, player->getID(), raceId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketMonsterBonusEffectAction* data)
{
	addGameTask(&Game::playerRequestBuyCharmRune, player->getID(), data->RuneId, data->Action, data->RaceId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketOpenCyclopediaCharacterInfo* data)
{
	uint32_t characterID = data->CharacterID;
	if (characterID == 0) {
		characterID = player->getGUID();
	}
	addGameTask(&Game::playerCyclopediaCharacterInfo, player->getID(), characterID, static_cast<CyclopediaCharacterInfoType_t>(data->Type), std::min<uint16_t>(30, std::max<uint16_t>(5, data->EntriesPerPage)), std::max<uint16_t>(1, data->Page));
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketBugReport* data)
{
	addGameTask(&Game::playerReportBug, player->getID(), data->Message, data->Pos, data->Category);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketPreyAction* data)
{
	if (!g_configManager().getBoolean(PREY_ENABLED)) {
		return;
	}

	addGameTask(&Game::playerPreyAction, player->getID(), data->Slot, data->Action, data->Option, data->Index, data->RaceId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGeet* data)
{
	addGameTask(&Game::playerNpcGreet, player->getID(), data->NpcId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketTransferCurrency* data)
{
	if (g_configManager().getBoolean(STOREMODULES)) {
		return;
	}

	if (amount > 0) {
		addGameTask(&Game::playerCoinTransfer, player->getID(), data->Receiver, data->Amount);
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGetQuestLog* data)
{
	addGameTask(&Game::playerShowQuestLog, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketGetQuestLine* data)
{
	addGameTask(&Game::playerShowQuestLine, player->getID(), data->QuestId);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketMarketLeave* data)
{
	addGameTask(&Game::playerLeaveMarket, player->getID());
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketMarketBrowse* data)
{
	if (data->Type == static_cast<uint8_t>(MARKETREQUEST_OWN_OFFERS)) {
		addGameTask(&Game::playerBrowseMarketOwnOffers, player->getID());
	} else if (data->Type == static_cast<uint8_t>(MARKETREQUEST_OWN_HISTORY)) {
		addGameTask(&Game::playerBrowseMarketOwnHistory, player->getID());
	} else if (data->Type == static_cast<uint8_t>(MARKETREQUEST_ITEM_BROWSE)) {
		player->sendMarketEnter(player->getLastDepotId());
		addGameTask(&Game::playerBrowseMarket, player->getID(), data->ItemId);
	} else {
		SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkMessage::ClientPacketMarketBrowse] - "
					"Invalid ENUM({}) 'type' value received from player {}", data->Type, player->getName());
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketMarketCreate* data)
{
	if (data->Amount > 0 && data->Price > 0) {
		addGameTask(&Game::playerCreateMarketOffer, player->getID(), data->Type, data->ItemId, data->Amount, data->Price, data->Anonymous);
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketMarketCancel* data)
{
	if (data->Counter > 0) {
		addGameTask(&Game::playerCancelMarketOffer, player->getID(), data->Timestamp, data->Counter);
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketMarketAccept* data)
{
	if (data->Amount > 0 && data->Counter > 0) {
		addGameTask(&Game::playerAcceptMarketOffer, player->getID(), data->Timestamp, data->Counter, data->Amount);
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketAnswerModalDialog* data)
{
	addGameTask(&Game::playerAnswerModalWindow, player->getID(), data->Id, data->Button, data->Choice);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketOpenInGameShop* data)
{
	if (g_configManager().getBoolean(STOREMODULES)) {
		return;
	}

	addGameTask(&Game::playerStoreOpen, player->getID(), data->Type);
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketRequestShopOffers* data)
{
	if (g_configManager().getBoolean(STOREMODULES)) {
		return;
	}

	if (data->Index > 0) {
		addGameTask(&Game::playerShowStoreCategoryOffers, player->getID(), data->Type, data->Index);
	} else {
		SPDLOG_WARN("[PlayerClientModules::ParseFromNetworkData::ClientPacketRequestShopOffers] - "
                    "Requested category: {} doesn't exists", data->Category);
	}
}

void PlayerClientModules::ParseFromNetworkData(const ClientPacketBuyInGameShopOffer* data)
{
	if (g_configManager().getBoolean(STOREMODULES)) {
		return;
	}

	addGameTask(&Game::playerBuyStoreOffer, player->getID(), data->OfferId, data->Type, data->Info);
}
