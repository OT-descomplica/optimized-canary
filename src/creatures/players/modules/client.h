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

#ifndef SRC_CREATURES_PLAYERS_MODULES_CLIENTMODULES_H_
#define SRC_CREATURES_PLAYERS_MODULES_CLIENTMODULES_H_

#include "creatures/players/player.h"
#include "server/network/protocol/protocolgame/network_definitions.hpp"
#include "creatures/creatures_definitions.hpp"

class PlayerClientModules
{
	public:
		explicit PlayerClientModules(Player* initPlayer) : player(initPlayer) {};

		// Helpers so we don't need to bind every time
		template <typename Callable, typename... Args>
		void addGameTask(Callable function, Args &&... args);

		void ParseFromNetworkData(const ClientPacketDepotSearchRetrieve* data);
		void ParseFromNetworkData(const ClientPacketGoEast* data);
		void ParseFromNetworkData(const ClientPacketGoNorth* data);
		void ParseFromNetworkData(const ClientPacketGoPath* data);
		void ParseFromNetworkData(const ClientPacketGoSouth* data);
		void ParseFromNetworkData(const ClientPacketGoWest* data);
		void ParseFromNetworkData(const ClientPacketPartyHuntAnalyzer* data);
		void ParseFromNetworkData(const ClientPacketPing* data);
		void ParseFromNetworkData(const ClientPacketPingBack* data);
		void ParseFromNetworkData(const ClientPacketQuitGame* data);
		void ParseFromNetworkData(const ClientPacketExtendedOpCode* data);
		void ParseFromNetworkData(const ClientPacketStashAction* data);
		void ParseFromNetworkData(const ClientPacketTeamFinderAssembleTeam* data);
		void ParseFromNetworkData(const ClientPacketTeamFinderJoinTeam* data);
		void ParseFromNetworkData(const ClientPacketTrackBestiaryRace* data);
		void ParseFromNetworkData(const ClientPacketStop* data);
		void ParseFromNetworkData(const ClientPacketGoNorthEast* data);
		void ParseFromNetworkData(const ClientPacketGoSouthEast* data);
		void ParseFromNetworkData(const ClientPacketGoSouthWest* data);
		void ParseFromNetworkData(const ClientPacketGoNorthWest* data);
		void ParseFromNetworkData(const ClientPacketRotateNorth* data);
		void ParseFromNetworkData(const ClientPacketRotateEast* data);
		void ParseFromNetworkData(const ClientPacketRotateSouth* data);
		void ParseFromNetworkData(const ClientPacketRotateWest* data);
		void ParseFromNetworkData(const ClientPacketTeleport* data);
		void ParseFromNetworkData(const ClientPacketEquipObject* data);
		void ParseFromNetworkData(const ClientPacketMoveObject* data);
		void ParseFromNetworkData(const ClientPacketLookNpcTrade* data);
		void ParseFromNetworkData(const ClientPacketBuyObject* data);
		void ParseFromNetworkData(const ClientPacketSellObject* data);
		void ParseFromNetworkData(const ClientPacketCloseNpcTrade* data);
		void ParseFromNetworkData(const ClientPacketTradeObject* data);
		void ParseFromNetworkData(const ClientPacketLookTrade* data);
		void ParseFromNetworkData(const ClientPacketAcceptTrade* data);
		void ParseFromNetworkData(const ClientPacketRejectTrade* data);
		void ParseFromNetworkData(const ClientPacketUseObject* data);
		void ParseFromNetworkData(const ClientPacketUseTwoObject* data);
		void ParseFromNetworkData(const ClientPacketUseOnCreature* data);
		void ParseFromNetworkData(const ClientPacketTurnObject* data);
		void ParseFromNetworkData(const ClientPacketShowOffSocket* data);
		void ParseFromNetworkData(const ClientPacketCloseContainer* data);
		void ParseFromNetworkData(const ClientPacketUpContainer* data);
		void ParseFromNetworkData(const ClientPacketEditText* data);
		void ParseFromNetworkData(const ClientPacketEditList* data);
		void ParseFromNetworkData(const ClientPacketToggleWrapState* data);
		void ParseFromNetworkData(const ClientPacketLook* data);
		void ParseFromNetworkData(const ClientPacketLookAtCreature* data);
		void ParseFromNetworkData(const ClientPacketQuickLoot* data);
		void ParseFromNetworkData(const ClientPacketLootContainer* data);
		void ParseFromNetworkData(const ClientPacketQuickLootBlackWhiteList* data);
		void ParseFromNetworkData(const ClientPacketOpenDepotSearch* data);
		void ParseFromNetworkData(const ClientPacketCloseDepotSearch* data);
		void ParseFromNetworkData(const ClientPacketDepotSearchType* data);
		void ParseFromNetworkData(const ClientPacketOpenParentContainer* data);
		void ParseFromNetworkData(const ClientPacketTalk* data);
		void ParseFromNetworkData(const ClientPacketGetChannels* data);
		void ParseFromNetworkData(const ClientPacketJoinChannel* data);
		void ParseFromNetworkData(const ClientPacketLeaveChannel* data);
		void ParseFromNetworkData(const ClientPacketPrivateChannel* data);
		void ParseFromNetworkData(const ClientPacketCloseNpcChannel* data);
		void ParseFromNetworkData(const ClientPacketSetTatics* data);
		void ParseFromNetworkData(const ClientPacketAttack* data);
		void ParseFromNetworkData(const ClientPacketFollow* data);
		void ParseFromNetworkData(const ClientPacketInviteToParty* data);
		void ParseFromNetworkData(const ClientPacketJoinParty* data);
		void ParseFromNetworkData(const ClientPacketRevokeInvitation* data);
		void ParseFromNetworkData(const ClientPacketPassLeadership* data);
		void ParseFromNetworkData(const ClientPacketLeaveParty* data);
		void ParseFromNetworkData(const ClientPacketShareExperience* data);
		void ParseFromNetworkData(const ClientPacketOpenChannel* data);
		void ParseFromNetworkData(const ClientPacketInviteToChannel* data);
		void ParseFromNetworkData(const ClientPacketExcludeFromChannel* data);
		void ParseFromNetworkData(const ClientPacketHighscores* data);
		void ParseFromNetworkData(const ClientPacketPreyHuntingTaskAction* data);
		void ParseFromNetworkData(const ClientPacketCancel* data);
		void ParseFromNetworkData(const ClientPacketBrowseField* data);
		void ParseFromNetworkData(const ClientPacketSeekInContainer* data);
		void ParseFromNetworkData(const ClientPacketInspectObject* data);
		void ParseFromNetworkData(const ClientPacketGetOutfit* data);
		void ParseFromNetworkData(const ClientPacketSetOutfit* data);
		void ParseFromNetworkData(const ClientPacketMount* data);
		void ParseFromNetworkData(const ClientPacketApplyImbuement* data);
		void ParseFromNetworkData(const ClientPacketApplyClearingCharm* data);
		void ParseFromNetworkData(const ClientPacketCloseImbuingDialog* data);
		void ParseFromNetworkData(const ClientPacketAddBuddy* data);
		void ParseFromNetworkData(const ClientPacketRemoveBuddy* data);
		void ParseFromNetworkData(const ClientPacketEditBuddy* data);
		void ParseFromNetworkData(const ClientPacketOpenMonsterCyclopedia* data);
		void ParseFromNetworkData(const ClientPacketOpenMonsterCyclopediaMonsters* data);
		void ParseFromNetworkData(const ClientPacketOpenMonsterCyclopediaRace* data);
		void ParseFromNetworkData(const ClientPacketMonsterBonusEffectAction* data);
		void ParseFromNetworkData(const ClientPacketOpenCyclopediaCharacterInfo* data);
		void ParseFromNetworkData(const ClientPacketBugReport* data);
		void ParseFromNetworkData(const ClientPacketPreyAction* data);
		void ParseFromNetworkData(const ClientPacketGeet* data);
		void ParseFromNetworkData(const ClientPacketTransferCurrency* data);
		void ParseFromNetworkData(const ClientPacketGetQuestLog* data);
		void ParseFromNetworkData(const ClientPacketGetQuestLine* data);
		void ParseFromNetworkData(const ClientPacketMarketLeave* data);
		void ParseFromNetworkData(const ClientPacketMarketBrowse* data);
		void ParseFromNetworkData(const ClientPacketMarketCreate* data);
		void ParseFromNetworkData(const ClientPacketMarketCancel* data);
		void ParseFromNetworkData(const ClientPacketMarketAccept* data);
		void ParseFromNetworkData(const ClientPacketAnswerModalDialog* data);
		void ParseFromNetworkData(const ClientPacketOpenInGameShop* data);
		void ParseFromNetworkData(const ClientPacketRequestShopOffers* data);
		void ParseFromNetworkData(const ClientPacketBuyInGameShopOffer* data);

	private:
		Player* player;
};

#endif  // SRC_CREATURES_PLAYERS_MODULES_CLIENTMODULES_H_
