#include "definitions.h"
#include "Plugin.h"
#include "hook_tools.h"

//Hook header files
#include "hooks/main/game_hooks.h"
#include "graphics/draw_hook.h"

#include "hooks/apply_upgrade_flags.h"
#include "hooks/attack_priority.h"
#include "hooks/bunker_hooks.h"
#include "hooks/cloak_nearby_units.h"
#include "hooks/cloak_tech.h"
#include "hooks/detector.h"
#include "hooks/harvest.h"
#include "hooks/rally_point.h"
#include "hooks/recharge_shields.h"
#include "hooks/stim_packs.h"
#include "hooks/tech_target_check.h"
#include "hooks/transfer_tech_upgrades.h"
#include "hooks/unit_speed.h"
#include "hooks/update_status_effects.h"
#include "hooks/update_unit_state.h"
#include "hooks/weapons/weapon_cooldown.h"
#include "hooks/weapons/weapon_damage.h"
#include "hooks/weapons/weapon_fire.h"

#include "hooks/unit_destructor_special.h"
#include "hooks/psi_field.h"

#include "hooks/unit_stats/armor_bonus.h"
#include "hooks/unit_stats/sight_range.h"
#include "hooks/unit_stats/max_energy.h"
#include "hooks/unit_stats/weapon_range.h"
#include "hooks/interface/status_display/weapon_armor_tooltip.h"

//in alphabetical order
#include "hooks/orders/base_orders/attack_orders.h"
#include "hooks/interface/btns_cond.h"
#include "hooks/orders/building_making/building_morph.h"
#include "hooks/interface/buttonsets.h"
#include "hooks/orders/building_making/building_protoss.h"
#include "hooks/orders/building_making/building_terran.h"
#include "hooks/orders/burrow_orders.h"
#include "hooks/recv_commands/burrow_tech.h"
#include "hooks/orders/spells/cast_order.h"
#include "hooks/orders/cloak_nearby_units_order.h"
#include "hooks/recv_commands/CMDRECV_Build.h"
#include "hooks/recv_commands/CMDRECV_Cancel.h"
#include "hooks/recv_commands/CMDRECV_LiftOff.h"
#include "hooks/recv_commands/CMDRECV_MergeArchon.h"
#include "hooks/recv_commands/CMDRECV_Morph.h"
#include "hooks/recv_commands/CMDRECV_QueuedOrder.h"
#include "hooks/recv_commands/CMDRECV_ResearchUpgrade.h"
#include "hooks/recv_commands/CMDRECV_RightClick.h"
#include "hooks/recv_commands/CMDRECV_Selection.h"
#include "hooks/recv_commands/CMDRECV_SiegeTank.h"
#include "hooks/recv_commands/CMDRECV_Stimpack.h"
#include "hooks/recv_commands/CMDRECV_Stop.h"
#include "hooks/create_init_units.h"
#include "hooks/orders/critter_order.h"
#include "hooks/orders/spells/defense_matrix.h"
#include "hooks/orders/base_orders/die_orders.h"
#include "hooks/orders/doodad_orders.h"
#include "hooks/orders/enter_nydus.h"
#include "hooks/utils/ExtendSightLimit.h"
#include "hooks/orders/spells/feedback_spell.h"
#include "hooks/give_unit.h"
#include "hooks/orders/spells/hallucination_spell.h"
#include "hooks/orders/harvest_orders.h"
#include "hooks/orders/infestation.h"
#include "hooks/orders/interceptor_return_order.h"
#include "hooks/orders/junkyarddog_order.h"
#include "hooks/orders/larva_creep_spawn.h"
#include "hooks/orders/larva_order.h"
#include "hooks/orders/liftland.h"
#include "hooks/orders/load_unload_orders.h"
#include "hooks/load_unload_proc.h"
#include "hooks/orders/building_making/make_nydus_exit.h"
#include "hooks/orders/medic_orders.h"
#include "hooks/orders/merge_units.h"
#include "hooks/orders/spells/mindcontrol_spell.h"
#include "hooks/orders/base_orders/move_orders.h"
#include "hooks/orders/spells/nuke_orders.h"
#include "hooks/orders/0_orders/orders_root.h"
#include "hooks/orders/base_orders/patrol_order.h"
#include "hooks/orders/powerup.h"
#include "hooks/orders/spells/recall_spell.h"
#include "hooks/recv_commands/receive_command.h"
#include "hooks/orders/repair_order.h"
#include "hooks/utils/replace_unit.h"
#include "hooks/orders/rescuable_order.h"
#include "hooks/orders/research_upgrade_orders.h"
#include "hooks/orders/reset_collision.h"
#include "hooks/right_click_CMDACT.h"
#include "hooks/orders/base_orders/rightclick_order.h"
#include "hooks/orders/spells/scanner_orders.h"
#include "hooks/interface/select_larva.h"
#include "hooks/interface/selection.h"
#include "hooks/orders/shield_recharge_orders.h"
#include "hooks/orders/shrouded_order.h"
#include "hooks/orders/siege_transform.h"
#include "hooks/orders/spidermine.h"
#include "hooks/interface/status_display/stats_display_main.h"
#include "hooks/interface/status_display/stats_panel_display.h"
#include "hooks/interface/status_display/advanced/status_base_text.h"
#include "hooks/interface/status_display/advanced/status_buildmorphtrain.h"
#include "hooks/interface/status_display/advanced/status_nukesilo_resources.h"
#include "hooks/interface/status_display/advanced/status_research_upgrade.h"
#include "hooks/interface/status_display/advanced/status_supply_provider.h"
#include "hooks/interface/status_display/advanced/status_transport.h"
#include "hooks/orders/base_orders/stopholdpos_orders.h"
#include "hooks/recv_commands/train_cmd_receive.h"
#include "hooks/orders/unit_making/unit_morph.h"
#include "hooks/interface/status_display/unit_stat_act.h"
#include "hooks/interface/status_display/unit_stat_cond.h"
#include "hooks/interface/status_display/unit_stat_selection.h"
#include "hooks/orders/unit_making/unit_train.h"
#include "hooks/interface/updateSelectedUnitsData.h"
#include "hooks/utils/utils.h"
#include "hooks/orders/warpin.h"
#include "hooks/interface/status_display/wireframe.h"
#include "hooks/weapons/wpnspellhit.h"
#include "hooks/DatExtend/dat_load.h"

namespace manifold {
	const u32 statDataBinOffset[] = { 0x00456A50, 0x00457F30, 0x00457F30, 0x00457F30, 0x00457F30, 0x00457F30, 0x004581E0, 0x00456EC0,
								0x00457F30, 0x00457F30, 0x00457F30, 0x00457F30, 0x004581E0, 0x004581E0, 0x00457F30, 0x004581E0,
								0x00457F30, 0x00457E90, 0x00457E90, 0x00457E90, 0x00457E90, 0x00457E90, 0x00457E90, 0x00457E90,
								0x00457E90, 0x00457E90, 0x00457E90, 0x00457E90, 0x00457E90, 0x00457E90, 0x00457E90, 0x00457E90,
								0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0,
								0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0,
								0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0, 0x004583E0 };

	extern CUnit** localSelectionGroup = new CUnit * [SELECTION_ARRAY_LENGTH];
	CUnit** localSelectionGroup2 = new CUnit * [SELECTION_ARRAY_LENGTH];
	CUnit** activeUnitSelection = new CUnit * [SELECTION_ARRAY_LENGTH];
	CUnit** CurrentUnitSelection = new CUnit * [SELECTION_ARRAY_LENGTH];
	CUnit** AllPlayerSelectionGroups = new CUnit * [8 * SELECTION_ARRAY_LENGTH]; //PLAYER_COUNT´Â 12
	CUnit** selections = new CUnit * [144 * SELECTION_ARRAY_LENGTH];//8*18 °ªÀÓ
	u16* selGroupType = new u16[SELECTION_ARRAY_LENGTH];
	u32* selGroupHp = new u32[SELECTION_ARRAY_LENGTH];
	u32* dword_63FE40 = new u32[SELECTION_ARRAY_LENGTH * 8 / 3];

	const u32 Hook_GetSelectedUnitsInBox00 = 0x0046FA41;
	const u32 Hook_GetSelectedUnitsInBox00_Back = 0x0046FA46;
	void __declspec(naked) getSelectedUnitsInBox00() {
		static s32 num = SELECTION_ARRAY_LENGTH * 8;

		__asm {
			MOV EBP, ESP
			SUB ESP, num
			JMP Hook_GetSelectedUnitsInBox00_Back
		}

	}

	const u32 Hook_GetSelectedUnitsInBox01 = 0x0046FAAC;
	const u32 Hook_GetSelectedUnitsInBox01_Back = 0x0046FAB9;
	void __declspec(naked) getSelectedUnitsInBox01() {
		static s32 num0 = -(SELECTION_ARRAY_LENGTH * 4);
		static s32 num1 = -(SELECTION_ARRAY_LENGTH * 8);

		__asm {
			MOV EDI, num1
			LEA EDI, [EBP + EDI]
			PUSH EBX
			MOV EAX, num0
			LEA EAX, [EBP + EAX]
			REP MOVSD
			PUSH EAX
			MOV EDI, num1
			LEA EDI, [EBP + EDI]
			JMP Hook_GetSelectedUnitsInBox01_Back
		}

	}

	const u32 Hook_GetSelectedUnitsAtPoint00 = 0x0046FB41;
	const u32 Hook_GetSelectedUnitsAtPoint00_Back = 0x0046FB46;
	void __declspec(naked) getSelectedUnitsAtPoint00() {
		static s32 num = SELECTION_ARRAY_LENGTH * 8 + 12;

		__asm {
			MOV EBP, ESP
			SUB ESP, num
			JMP Hook_GetSelectedUnitsAtPoint00_Back
		}

	}

	const u32 Hook_GetSelectedUnitsAtPoint01 = 0x0046FC9A;
	const u32 Hook_GetSelectedUnitsAtPoint01_Back = 0x0046FCA0;
	void __declspec(naked) getSelectedUnitsAtPoint01() {
		static s32 num = -(SELECTION_ARRAY_LENGTH * 8 + 12);

		__asm {
			MOV EDI, num
			LEA EDI, [EBP + EDI]
			LEA EAX, [EBP - 0Ch]
			JMP Hook_GetSelectedUnitsAtPoint01_Back
		}

	}

	const u32 Hook_GetSelectedUnitsAtPoint02 = 0x0046FCD6;
	const u32 Hook_GetSelectedUnitsAtPoint02_Back = 0x0046FCDD;
	void __declspec(naked) getSelectedUnitsAtPoint02() {
		static s32 num0 = -(SELECTION_ARRAY_LENGTH * 4 + 12);
		static s32 num1 = -(SELECTION_ARRAY_LENGTH * 8 + 12);

		__asm {
			MOV EAX, num0
			LEA EAX, [EBP + EAX]
			PUSH EAX
			MOV EDI, num1
			LEA EDI, [EBP + EDI]
			JMP Hook_GetSelectedUnitsAtPoint02_Back
		}

	}

	const u32 Hook_CMDACT_Select00 = 0x004C0861;
	const u32 Hook_CMDACT_Select00_Back = 0x004C0866;
	void __declspec(naked) CMDACT_Select00() {
		static s32 num = (SELECTION_ARRAY_LENGTH * 2 + 4) * 3 + 8;

		__asm {
			MOV EBP, ESP
			SUB ESP, num
			JMP Hook_CMDACT_Select00_Back
		}

	}

	const u32 Hook_CMDACT_Select01 = 0x004C086F;
	const u32 Hook_CMDACT_Select01_Back = 0x004C08C0;
	void __declspec(naked) CMDACT_Select01() {
		static s32 num0 = -((SELECTION_ARRAY_LENGTH * 2 + 4) * 3 + 8);
		static s32 num1 = ((SELECTION_ARRAY_LENGTH * 2 + 4) * 3) / 4;

		__asm {
			XOR EAX, EAX
			MOV ECX, num1
			MOV EDI, num0
			LEA EDI, [EBP + EDI]
			REP STOSD
			XOR ECX, ECX
			XOR EDX, EDX
			MOV EDI, [EBP + 0Ch]
			MOV EBX, [EBP + 8]
			TEST EBX, EBX
			JMP Hook_CMDACT_Select01_Back
		}

	}

	const u32 Hook_CMDACT_Select02 = 0x004C0959;
	const u32 Hook_CMDACT_Select02_Back = 0x004C095E;
	void __declspec(naked) CMDACT_Select02() {
		static s32 num = -((SELECTION_ARRAY_LENGTH * 2 + 4) * 3 + 6);

		__asm {
			MOV ESI, num
			ADD ESI, EBP
			MOV WORD PTR[ECX * 2 + ESI], AX
			JMP Hook_CMDACT_Select02_Back
		}

	}

	const u32 Hook_CMDACT_Select03 = 0x004C09E6;
	const u32 Hook_CMDACT_Select03_Back = 0x004C09EB;
	void __declspec(naked) CMDACT_Select03() {
		static s32 num = -((SELECTION_ARRAY_LENGTH * 2 + 4) * 2 + 6);

		__asm {
			MOV ESI, num
			ADD ESI, EBP
			MOV WORD PTR[ECX * 2 + ESI], AX
			JMP Hook_CMDACT_Select03_Back
		}

	}

	const u32 Hook_CMDACT_Select04 = 0x004C0A65;
	const u32 Hook_CMDACT_Select04_Back = 0x004C0A6C;
	void __declspec(naked) CMDACT_Select04() {
		static s32 num = -((SELECTION_ARRAY_LENGTH * 2 + 4) + 6);

		__asm {
			MOV ESI, num
			ADD ESI, EBP
			MOV WORD PTR[EDI * 2 + ESI], AX
			TEST EDI, EDI
			JMP Hook_CMDACT_Select04_Back
		}

	}

	const u32 Hook_CMDACT_Select05 = 0x004C0A71;
	const u32 Hook_CMDACT_Select05_Back = 0x004C0A7F;
	void __declspec(naked) CMDACT_Select05() {
		static s32 num = -((SELECTION_ARRAY_LENGTH * 2 + 4) + 8);

		__asm {
			MOV ECX, num
			MOV BYTE PTR[EBP + ECX], 9
			MOV BYTE PTR[EBP + ECX + 1], AL
			LEA ECX, [EBP + ECX]
			LEA EDX, [EAX + EAX + 2]
			JMP Hook_CMDACT_Select05_Back
		}

	}

	const u32 Hook_CMDACT_Select06 = 0x004C0A91;
	const u32 Hook_CMDACT_Select06_Back = 0x004C0A9F;
	void __declspec(naked) CMDACT_Select06() {
		static s32 num = -((SELECTION_ARRAY_LENGTH * 2 + 4) * 2 + 8);

		__asm {
			MOV ECX, num
			MOV BYTE PTR[EBP + ECX], 0Bh
			MOV BYTE PTR[EBP + ECX + 1], DL
			LEA ECX, [EBP + ECX]
			LEA EDX, [EDX + EDX + 2]
			JMP Hook_CMDACT_Select06_Back
		}

	}

	const u32 Hook_CMDACT_Select07 = 0x004C0AAB;
	const u32 Hook_CMDACT_Select07_Back = 0x004C0AB9;
	void __declspec(naked) CMDACT_Select07() {
		static s32 num = -((SELECTION_ARRAY_LENGTH * 2 + 4) * 3 + 8);

		__asm {
			MOV ECX, num
			MOV BYTE PTR[EBP + ECX], 0Ah
			MOV BYTE PTR[EBP + ECX + 1], AL
			LEA ECX, [EBP + ECX]
			LEA EDX, [EAX + EAX + 2]
			JMP Hook_CMDACT_Select07_Back
		}

	}

	void injectDll() {
		int msgBoxID;
		msgBoxID = MessageBox(NULL, "Do you want to play in W-Mode?", PLUGIN_NAME, MB_YESNOCANCEL);
		switch (msgBoxID) {
		case IDYES:
			if (!LoadLibrary("./WMODE.dll")) {
				MessageBox(NULL, "Failed to inject WMODE.dll", NULL, MB_ICONERROR);
				break;
			}
			if (!LoadLibrary("./WMODE_FIX.dll")) {
				MessageBox(NULL, "Failed to inject WMODE_FIX.dll", NULL, MB_ICONERROR);
			}
			break;
		case IDCANCEL:
			exit(0);
		}
	}

	void injectSelectionExt() {
		memoryPatch_Byte(0x00457D7E, 0x20 + SELECTION_ARRAY_LENGTH);
		memoryPatch(0x00425A0C, 0x20 + SELECTION_ARRAY_LENGTH);
		memoryPatch(0x004584C3, sizeof(statDataBinOffset));	//Data window offset memory size
		memoryPatch(0x004584C8, &statDataBinOffset);		//Data window offset memory size

		memoryPatch(0x004234D5, localSelectionGroup);
		memoryPatch(0x004234F0, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00423556, localSelectionGroup);
		memoryPatch(0x004235CD, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00423683, localSelectionGroup);
		memoryPatch(0x004236A9, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00424385, localSelectionGroup);
		memoryPatch(0x004243DD, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00424552, localSelectionGroup);
		memoryPatch(0x0042457C, ((u32)localSelectionGroup) + 4);
		memoryPatch(0x004245A5, ((u32)localSelectionGroup) + 8);
		memoryPatch(0x004245CF, ((u32)localSelectionGroup) + 12);
		memoryPatch(0x004245F9, ((u32)localSelectionGroup) + 16);
		memoryPatch(0x00424623, ((u32)localSelectionGroup) + 20);
		memoryPatch(0x0042466D, localSelectionGroup);
		memoryPatch(0x00424691, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004259C3, localSelectionGroup);
		memoryPatch(0x004259C9, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428311, localSelectionGroup);
		memoryPatch(0x00428327, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428373, localSelectionGroup);
		memoryPatch(0x004283A5, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004283C1, localSelectionGroup);
		memoryPatch(0x004283DA, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428481, localSelectionGroup);
		memoryPatch(0x00428499, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004284B1, localSelectionGroup);
		memoryPatch(0x004284C9, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428571, localSelectionGroup);
		memoryPatch(0x00428587, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004285B1, localSelectionGroup);
		memoryPatch(0x004285C7, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004285E1, localSelectionGroup);
		memoryPatch(0x004285F7, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428611, localSelectionGroup);
		memoryPatch(0x00428627, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428641, localSelectionGroup);
		memoryPatch(0x00428657, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428671, localSelectionGroup);
		memoryPatch(0x00428687, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004286A1, localSelectionGroup);
		memoryPatch(0x004286CA, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004286E1, localSelectionGroup);
		memoryPatch(0x0042870E, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428734, localSelectionGroup);
		memoryPatch(0x00428753, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428784, localSelectionGroup);
		memoryPatch(0x004287A3, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428861, localSelectionGroup);
		memoryPatch(0x00428880, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004289A1, localSelectionGroup);
		memoryPatch(0x004289B7, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428A21, localSelectionGroup);
		memoryPatch(0x00428A37, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428D41, localSelectionGroup);
		memoryPatch(0x00428D81, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428DA1, localSelectionGroup);
		memoryPatch(0x00428DE7, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428E22, localSelectionGroup);
		memoryPatch(0x00428EA2, localSelectionGroup);
		memoryPatch(0x00428EBA, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428ED2, localSelectionGroup);
		memoryPatch(0x00428F13, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428F32, localSelectionGroup);
		memoryPatch(0x00428F7E, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428FA4, localSelectionGroup);
		memoryPatch(0x00428FC3, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00428FF3, localSelectionGroup);
		memoryPatch(0x00429053, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x0042907D, localSelectionGroup);
		memoryPatch(0x004290C3, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00429100, localSelectionGroup);
		memoryPatch(0x00429149, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00429188, localSelectionGroup);
		memoryPatch(0x004291AB, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004291D8, localSelectionGroup);
		memoryPatch(0x004291FB, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x0042921D, localSelectionGroup);
		memoryPatch(0x00429298, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004292CD, localSelectionGroup);
		memoryPatch(0x0042934A, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00429380, localSelectionGroup);
		memoryPatch(0x004293BE, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004293F0, localSelectionGroup);
		memoryPatch(0x00429430, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00429486, localSelectionGroup);
		memoryPatch(0x004294B3, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x00455A07, ((u32)localSelectionGroup) + (SELECTION_ARRAY_LENGTH * 4) - 4);
		memoryPatch(0x00455BBA, localSelectionGroup);
		memoryPatch(0x004563D3, localSelectionGroup);
		memoryPatch(0x00456403, localSelectionGroup);
		memoryPatch(0x00456543, localSelectionGroup);
		memoryPatch(0x004565C3, localSelectionGroup);
		memoryPatch(0x00458BD7, localSelectionGroup);
		memoryPatch(0x00458C8F, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x0046F657, localSelectionGroup);
		memoryPatch(0x0046F7E8, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004C389E, localSelectionGroup);
		memoryPatch(0x004C38BE, localSelectionGroup);
		memoryPatch(0x004C38D4, localSelectionGroup);
		memoryPatch(0x004C38F4, &localSelectionGroup[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004C3911, ((u32)localSelectionGroup) + 4);
		memoryPatch(0x004C3917, localSelectionGroup);
		memoryPatch(0x004C3C3D, localSelectionGroup);
		memoryPatch(0x004E5692, localSelectionGroup);
		memoryPatch(0x004564FD, localSelectionGroup2);
		memoryPatch(0x004BF8C5, localSelectionGroup2);
		memoryPatch(0x004BF8D0, ((u32)localSelectionGroup2) + 4);
		memoryPatch(0x004BF8DB, ((u32)localSelectionGroup2) + 8);
		memoryPatch(0x004BF8E6, ((u32)localSelectionGroup2) + 12);
		memoryPatch(0x004BF8F1, ((u32)localSelectionGroup2) + 16);
		memoryPatch(0x004BF8FC, ((u32)localSelectionGroup2) + 20);
		memoryPatch(0x004BF933, localSelectionGroup2);
		memoryPatch(0x004BF942, ((u32)localSelectionGroup2) - 4);
		memoryPatch(0x004BF94A, localSelectionGroup2);
		memoryPatch(0x004BF951, localSelectionGroup2);
		memoryPatch(0x004C07E1, localSelectionGroup2);
		memoryPatch(0x004C07F4, localSelectionGroup2);
		memoryPatch(0x004C08E3, localSelectionGroup2);
		memoryPatch(0x004C08FA, localSelectionGroup2);
		memoryPatch(0x004C096F, localSelectionGroup2);
		memoryPatch(0x004C0A03, localSelectionGroup2);
		memoryPatch(0x004C0A11, localSelectionGroup2);
		memoryPatch(0x004EED2C, localSelectionGroup2);
		memoryPatch(0x004EED31, ((u32)localSelectionGroup2) + 4);
		memoryPatch(0x004EED36, ((u32)localSelectionGroup2) + 8);
		memoryPatch(0x004D083E, activeUnitSelection);
		memoryPatch(0x004EEDDD, activeUnitSelection);
		memoryPatch(0x0045D04E, CurrentUnitSelection);
		memoryPatch(0x0046FA98, CurrentUnitSelection);
		memoryPatch(0x0046FAA8, CurrentUnitSelection);
		memoryPatch(0x0046FBAB, CurrentUnitSelection);
		memoryPatch(0x0046FC96, CurrentUnitSelection);
		memoryPatch(0x0046FCF7, CurrentUnitSelection);
		memoryPatch(0x00499A73, CurrentUnitSelection);
		memoryPatch(0x00499A9B, CurrentUnitSelection);
		memoryPatch(0x0049A2C7, CurrentUnitSelection);
		memoryPatch(0x0049A305, &CurrentUnitSelection[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x0049AE49, CurrentUnitSelection);
		memoryPatch(0x0049AE77, &CurrentUnitSelection[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x0049AE95, CurrentUnitSelection);
		memoryPatch(0x0049AEB9, CurrentUnitSelection);
		memoryPatch(0x0049AEC9, CurrentUnitSelection);
		memoryPatch(0x0049B6A3, CurrentUnitSelection);
		memoryPatch(0x0049B6FA, CurrentUnitSelection);
		memoryPatch(0x0049F7C3, CurrentUnitSelection);
		memoryPatch(0x0049F7D4, CurrentUnitSelection);
		memoryPatch(0x004C38B9, CurrentUnitSelection);
		memoryPatch(0x004C3B4A, CurrentUnitSelection);
		memoryPatch(0x004C3B64, &CurrentUnitSelection[SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004D603F, CurrentUnitSelection);
		memoryPatch(0x004EED19, CurrentUnitSelection);
		memoryPatch(0x004966D4, AllPlayerSelectionGroups);
		memoryPatch(0x00496A8C, AllPlayerSelectionGroups);
		memoryPatch(0x0049A228, AllPlayerSelectionGroups);
		memoryPatch(0x0049A244, AllPlayerSelectionGroups);
		memoryPatch(0x0049A75A, AllPlayerSelectionGroups);
		memoryPatch(0x0049A772, AllPlayerSelectionGroups);
		memoryPatch(0x0049A873, AllPlayerSelectionGroups);
		memoryPatch(0x0049AFBE, AllPlayerSelectionGroups);
		memoryPatch(0x0049B729, AllPlayerSelectionGroups);
		memoryPatch(0x004C265C, AllPlayerSelectionGroups);
		memoryPatch(0x004C26B9, AllPlayerSelectionGroups);
		memoryPatch(0x004C27DA, AllPlayerSelectionGroups);
		memoryPatch(0x004C2D23, AllPlayerSelectionGroups);
		memoryPatch(0x004CEDA3, AllPlayerSelectionGroups);
		memoryPatch(0x004CEE04, AllPlayerSelectionGroups);
		memoryPatch(0x004D013F, AllPlayerSelectionGroups);
		memoryPatch(0x004EED25, AllPlayerSelectionGroups);
		memoryPatch(0x004EEDD3, AllPlayerSelectionGroups);
		memoryPatch(0x004EEDEB, AllPlayerSelectionGroups);
		memoryPatch(0x0049A193, ((u32)AllPlayerSelectionGroups) + 4);
		memoryPatch(0x0049A22E, ((u32)AllPlayerSelectionGroups) + 4);
		memoryPatch(0x004C2584, ((u32)AllPlayerSelectionGroups) + 4);
		memoryPatch(0x00499AE3, &AllPlayerSelectionGroups[8][SELECTION_ARRAY_LENGTH]);
		memoryPatch(0x004965FE, selections);
		memoryPatch(0x00496655, ((u32)selections) + 4);
		memoryPatch(0x004966A0, selections);
		memoryPatch(0x00496769, selections);
		memoryPatch(0x0049680E, ((u32)selections) + 4);
		memoryPatch(0x00496854, selections);
		memoryPatch(0x00496968, ((u32)selections) + 4);
		memoryPatch(0x004969F2, selections);
		memoryPatch(0x00496AC7, selections);
		memoryPatch(0x00496ACE, selections);
		memoryPatch(0x00496AD5, selections);
		memoryPatch(0x00496B60, selections);
		memoryPatch(0x00496D51, ((u32)selections) + (72 * SELECTION_ARRAY_LENGTH) + 4);
		memoryPatch(0x004EEC79, selections);
		memoryPatch(0x00424528, selGroupHp);
		memoryPatch(0x00424565, selGroupHp);
		memoryPatch(0x0042458F, ((u32)selGroupHp) + 4);
		memoryPatch(0x004245B8, ((u32)selGroupHp) + 8);
		memoryPatch(0x004245E2, ((u32)selGroupHp) + 12);
		memoryPatch(0x0042460C, ((u32)selGroupHp) + 16);
		memoryPatch(0x00424636, ((u32)selGroupHp) + 20);
		memoryPatch(0x00424663, selGroupHp);
		memoryPatch(0x00424987, selGroupHp);
		memoryPatch(0x00424F2E, ((u32)selGroupHp) + 4);
		memoryPatch(0x00424FCA, ((u32)selGroupHp) + 8);
		memoryPatch(0x0042514E, selGroupHp);
		memoryPatch(0x00425187, selGroupHp);
		memoryPatch(0x00425901, selGroupHp);
		memoryPatch(0x00426F1E, selGroupHp);
		memoryPatch(0x00426F5E, selGroupHp);
		memoryPatch(0x004274CE, selGroupHp);
		memoryPatch(0x0042789D, selGroupHp);
		memoryPatch(0x00424542, ((u32)selGroupType) + 2);
		memoryPatch(0x00424653, ((u32)&selGroupType[SELECTION_ARRAY_LENGTH]) + 2);
		memoryPatch(0x00424668, selGroupType);
		memoryPatch(0x00496568, ((u32)dword_63FE40) + 16);
		memoryPatch(0x00496B2E, dword_63FE40);
		memoryPatch(0x00496D83, dword_63FE40);
		memoryPatch(0x00496D8E, dword_63FE40);
		memoryPatch(0x00496DA8, dword_63FE40);
		memoryPatch(0x00496DB3, dword_63FE40);
		memoryPatch(0x00496DCE, dword_63FE40);
		memoryPatch(0x00496DD9, dword_63FE40);
		memoryPatch(0x00496DF4, dword_63FE40);
		memoryPatch(0x00496DFF, dword_63FE40);
		memoryPatch(0x00496E1A, dword_63FE40);
		memoryPatch(0x00496E25, dword_63FE40);
		memoryPatch(0x00496E40, dword_63FE40);
		memoryPatch(0x00496E4B, dword_63FE40);
		memoryPatch(0x004BFC42, dword_63FE40);
		memoryPatch(0x004C270B, ((u32)dword_63FE40) + 16);
		memoryPatch(0x004C2742, dword_63FE40);
		memoryPatch(0x004C2858, dword_63FE40);
		memoryPatch(0x004EEC85, dword_63FE40);

		memoryPatch_Byte(0x004965FB, 5);
		memoryPatch_Byte(0x0049669D, 5);
		memoryPatch_Byte(0x004966D1, 5);
		memoryPatch_Byte(0x00496761, 0xCF);	//* changed to 8
		memoryPatch_Byte(0x00496807, 5);
		memoryPatch_Byte(0x00496960, 5);
		memoryPatch_Byte(0x004969EB, 3);
		memoryPatch_Byte(0x00496A88, 0xD7);	//* changed to 8
		memoryPatch_Byte(0x00496B5D, 5);
		memoryPatch_Byte(0x0049A18A, 5);
		memoryPatch_Byte(0x0049A757, 5);
		memoryPatch_Byte(0x0049A222, 0xCB);
		memoryPatch_Byte(0x0049A240, 0xD0);
		memoryPatch_Byte(0x0049A86F, 0xC1);
		memoryPatch_Byte(0x0049AFBA, 0xD3);
		memoryPatch_Byte(0x004C2580, 5);
		memoryPatch_Byte(0x004C265A, 5);
		memoryPatch_Byte(0x004C26B5, 0xCF);
		memoryPatch_Byte(0x004C27D8, 5);

		memoryPatch_Byte(0x00428E46, SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x004563E7, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004563F2, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00456417, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00456422, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00456557, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00456563, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004565D7, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004565E3, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0046F208, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0046F2AF, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0046F33C, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00496A73, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004BF909, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004C08F2, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004D0825, SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x004D083A, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x004D0869, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x004D0886, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x004D08CD, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x004D08D5, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0045D045, SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0045D054, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0045D063, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0045D069, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0045D076, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0045D082, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0045D086, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0045D08E, -SELECTION_ARRAY_LENGTH * 4);
		// If you put a value of 128 or more in 1 byte, it becomes -, so do it separately.
		//memoryPatch_Byte(0x0046FA45, SELECTION_ARRAY_LENGTH*8);
		jmpPatch(getSelectedUnitsInBox00, Hook_GetSelectedUnitsInBox00);
		memoryPatch_Byte(0x0046FA59, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0046FA69, -SELECTION_ARRAY_LENGTH * 4);
		//memoryPatch_Byte(0x0046FAAE, -SELECTION_ARRAY_LENGTH*8);
		//memoryPatch_Byte(0x0046FAB2, -SELECTION_ARRAY_LENGTH*4);
		//memoryPatch_Byte(0x0046FAB8, -SELECTION_ARRAY_LENGTH*8);
		jmpPatch(getSelectedUnitsInBox01, Hook_GetSelectedUnitsInBox01);
		memoryPatch_Byte(0x0046FAE4, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0046FAFA, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0046FB09, -SELECTION_ARRAY_LENGTH * 4);
		//memoryPatch_Byte(0x0046FB45, SELECTION_ARRAY_LENGTH*8+12);
		jmpPatch(getSelectedUnitsAtPoint00, Hook_GetSelectedUnitsAtPoint00);
		memoryPatch_Byte(0x0046FB9A, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		//memoryPatch_Byte(0x0046FC9C, -(SELECTION_ARRAY_LENGTH*8+12));
		jmpPatch(getSelectedUnitsAtPoint01, Hook_GetSelectedUnitsAtPoint01);
		memoryPatch_Byte(0x0046FCAA, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		//memoryPatch_Byte(0x0046FCD8, -(SELECTION_ARRAY_LENGTH*4+12));
		//memoryPatch_Byte(0x0046FCDC, -(SELECTION_ARRAY_LENGTH*8+12));
		jmpPatch(getSelectedUnitsAtPoint02, Hook_GetSelectedUnitsAtPoint02);
		memoryPatch_Byte(0x0046FD09, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		memoryPatch_Byte(0x0046FD26, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		memoryPatch_Byte(0x0046FD62, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		memoryPatch_Byte(0x0046FD6A, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		memoryPatch_Byte(0x0046FD87, -(SELECTION_ARRAY_LENGTH * 4 + 8));
		memoryPatch_Byte(0x0046FD8C, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		// don't know what 0Ch of 46FD95, 49F7F5 means, probably doesn't matter.
		memoryPatch_Byte(0x0046FD9E, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		memoryPatch_Byte(0x0046FDAC, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		memoryPatch_Byte(0x0046FDC5, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		memoryPatch_Byte(0x0046FDCD, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		memoryPatch_Byte(0x0046FE3E, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		memoryPatch_Byte(0x0046FE69, -(SELECTION_ARRAY_LENGTH * 4 + 12));
		memoryPatch_Byte(0x0049F7A5, SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0049F7DA, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0049F7E9, -(SELECTION_ARRAY_LENGTH * 4 - 4));
		memoryPatch_Byte(0x0049F7EE, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0049F7FD, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0049F801, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x004C3B45, SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x004C3B5D, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x004C3B6D, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x004C3B75, -SELECTION_ARRAY_LENGTH * 4);
		memoryPatch_Byte(0x0046FD10, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0046FD1D, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00499AAB, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00499B88, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0049B70A, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0049B83C, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0049F7CE, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00496690, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00496765, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0049677B, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004969A0, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0049A1F1, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0049A215, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0049A7AF, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0049A859, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x0049AF8B, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004C256F, SELECTION_ARRAY_LENGTH);	// not sure
		memoryPatch_Byte(0x004C25C0, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004C25E2, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004C2670, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004C2687, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004C275D, SELECTION_ARRAY_LENGTH);	// not sure
		memoryPatch_Byte(0x004C27EE, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00496838, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x004968FA, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00496B45, SELECTION_ARRAY_LENGTH * 4 + 8);
		memoryPatch_Byte(0x00496B72, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00496B80, -(SELECTION_ARRAY_LENGTH * 4 + 8));
		memoryPatch_Byte(0x00496BFD, -(SELECTION_ARRAY_LENGTH * 4 + 8));
		memoryPatch_Byte(0x00496CA5, -(SELECTION_ARRAY_LENGTH * 4 + 8));
		memoryPatch_Byte(0x00496CB4, -(SELECTION_ARRAY_LENGTH * 4 + 8));
		memoryPatch_Byte(0x00496CE8, -(SELECTION_ARRAY_LENGTH * 4 + 8));
		memoryPatch_Byte(0x004BFB4E, SELECTION_ARRAY_LENGTH);
		memoryPatch_Byte(0x00458225, SELECTION_ARRAY_LENGTH * 4 + 4);
		memoryPatch_Byte(0x0045823F, -(SELECTION_ARRAY_LENGTH * 4 + 4));
		memoryPatch_Byte(0x00458289, -(SELECTION_ARRAY_LENGTH * 4 + 4));
		memoryPatch_Byte(0x004582AF, -(SELECTION_ARRAY_LENGTH * 4 + 4));
		memoryPatch_Byte(0x004582FF, -(SELECTION_ARRAY_LENGTH * 4 + 4));
		memoryPatch_Byte(0x00458347, -(SELECTION_ARRAY_LENGTH * 4 + 4));
		memoryPatch_Byte(0x0045836F, -(SELECTION_ARRAY_LENGTH * 4 + 4));
		memoryPatch_Byte(0x00458377, -(SELECTION_ARRAY_LENGTH * 4 + 4));
		memoryPatch(0x004C3897, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x004C38B4, SELECTION_ARRAY_LENGTH);//CurrentUnitSelection overlap
		memoryPatch(0x004C390A, SELECTION_ARRAY_LENGTH - 1);
		memoryPatch(0x004C3C38, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x004C07DC, SELECTION_ARRAY_LENGTH); //Need to check if the code below it needs to be modified
		//RETN argument value is also 0Ch, so confirmation is necessary
		memoryPatch(0x004C0977, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x004C09FE, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x004D0832, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x004D0843, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x004EEDD8, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x0045D049, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x0046F2B4, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x0046FA53, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x0046FAA3, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x0046FB8F, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x0046FC91, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x004EED14, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x004966AD, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x0049A18D, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x0049A75F, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x004C2D1E, SELECTION_ARRAY_LENGTH * 32);
		memoryPatch(0x004CEDA8, SELECTION_ARRAY_LENGTH * 8);
		memoryPatch(0x004CEE09, SELECTION_ARRAY_LENGTH * 8);
		memoryPatch(0x004D013A, SELECTION_ARRAY_LENGTH * 32);
		memoryPatch(0x004EED20, SELECTION_ARRAY_LENGTH * 8);
		memoryPatch(0x004EEDE6, SELECTION_ARRAY_LENGTH * 8);
		memoryPatch(0x00496B7A, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x00496D43, SELECTION_ARRAY_LENGTH * 72);
		memoryPatch(0x004EEC74, SELECTION_ARRAY_LENGTH * 144);
		memoryPatch(0x00458239, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x0045826F, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x004582F9, SELECTION_ARRAY_LENGTH);
		memoryPatch(0x0045832F, SELECTION_ARRAY_LENGTH);
		jmpPatch(CMDACT_Select00, Hook_CMDACT_Select00);
		jmpPatch(CMDACT_Select01, Hook_CMDACT_Select01);
		jmpPatch(CMDACT_Select02, Hook_CMDACT_Select02);
		jmpPatch(CMDACT_Select03, Hook_CMDACT_Select03);
		jmpPatch(CMDACT_Select04, Hook_CMDACT_Select04);
		jmpPatch(CMDACT_Select05, Hook_CMDACT_Select05);
		jmpPatch(CMDACT_Select06, Hook_CMDACT_Select06);
		jmpPatch(CMDACT_Select07, Hook_CMDACT_Select07);
	}
}

/// This function is called when the plugin is loaded into StarCraft.
/// You can enable/disable each group of hooks by commenting them.
/// You can also add custom modifications to StarCraft.exe by using:
///		memoryPatch(address_to_patch, value_to_patch_with);

BOOL WINAPI Plugin::InitializePlugin(IMPQDraftServer* lpMPQDraftServer) {

	//StarCraft.exe version check
	char exePath[300];
	const DWORD pathLen = GetModuleFileName(NULL, exePath, sizeof(exePath));

	if (pathLen == sizeof(exePath)) {
		MessageBox(NULL, "Error: Cannot check version of StarCraft.exe. The file path is too long.", NULL, MB_OK);
		return FALSE;
	}

	if (!checkStarCraftExeVersion(exePath))
		return FALSE;

	manifold::injectDll();

	hooks::injectGameHooks();
	hooks::injectDrawHook();

	//in order of creation
	hooks::injectInfestationHooks();
	hooks::injectSiegeTransformHooks();
	hooks::injectButtonSetHooks();
	hooks::injectSelectMod();
	hooks::injectMergeUnitsHooks();
	hooks::injectLarvaCreepSpawnHooks();
	hooks::injectLiftLandHooks();
	hooks::injectAttackOrdersHooks();
	hooks::injectStopHoldPosOrdersHooks();
	hooks::injectRecallSpellHooks();
	hooks::injectEnterNydusHooks();
	hooks::injectCastOrderHooks();
	hooks::injectWpnSpellHitHooks();
	hooks::injectBuildingMorphHooks();
	hooks::injectMakeNydusExitHook();
	hooks::injectUnitMorphHooks();
	hooks::injectWireframeHook();
	hooks::injectDieOrdersHook();
	hooks::injectBuildingTerranHook();
	hooks::injectBuildingProtossHooks();
	hooks::injectUnitTrainHooks();
	hooks::injectLoadUnloadProcHooks();
	hooks::injectLoadUnloadOrdersHooks();
	hooks::injectNukeOrdersHooks();
	hooks::injectBurrowTechHooks();
	hooks::injectResearchUpgradeOrdersHooks();
	hooks::injectMedicOrdersHooks();
	hooks::injectHallucinationSpellHook();
	hooks::injectFeedbackSpellHook();
	hooks::injectBtnsCondHook();
	hooks::injectRecvCmdHook();
	hooks::injectCreateInitUnitsHooks();
	hooks::injectGiveUnitHook();
	hooks::injectTrainCmdRecvHooks();
	hooks::injectCMDRECV_SiegeTankHooks();
	hooks::injectCMDRECV_MergeArchonHooks();
	hooks::injectCMDRECV_MorphHooks();
	hooks::injectCMDRECV_StopHooks();
	hooks::injectCMDRECV_CancelHooks();
	hooks::injectSelectLarvaHooks();
	hooks::injectRepairOrderHook();
	hooks::injectStatsPanelDisplayHook();
	hooks::injectUtilsHooks();
	hooks::injectMindControlSpellHook();
	hooks::injectCMDRECV_BuildHooks();
	hooks::injectExtendSightLimitMod();
	hooks::injectUpdateSelectedUnitsDataHook();
	hooks::injectStatsDisplayMainHook();
	hooks::injectUnitStatSelectionHooks();
	hooks::injectUnitStatCondHooks();
	hooks::injectUnitStatActHooks();
	hooks::injectStatusBaseTextHooks();
	hooks::injectStatusSupplyProviderHook();
	hooks::injectStatusResearchUpgradeHooks();
	hooks::injectStatusTransportHooks();
	hooks::injectStatusNukeSilo_Resources_Hooks();
	hooks::injectStatusBuildMorphTrain_Hooks();
	hooks::injectCMDRECV_SelectionHooks();
	hooks::injectCMDRECV_LiftOffHook();
	hooks::injectCMDRECV_ResearchUpgradeHooks();
	hooks::injectReplaceUnitWithTypeHook();
	hooks::injectCMDRECV_StimpackHook();
	hooks::injectCMDRECV_RightClickHooks();
	hooks::injectRightClickCMDACT_Hooks();
	hooks::injectCMDRECV_QueuedOrderHooks();
	hooks::injectResetCollisionHooks();
	hooks::injectOrdersRootHooks();
	hooks::injectShroudedOrderHook();
	hooks::injectMoveOrdersHooks();
	hooks::injectShieldRechargeOrdersHooks();
	hooks::injectSpiderMineHooks();
	hooks::injectPowerupOrderHooks();
	hooks::injectInterceptorReturnOrderHook();
	hooks::injectLarvaOrderHook();
	hooks::injectHarvestOrdersHooks();
	hooks::injectBurrowOrdersHooks();
	hooks::injectCloakNearbyUnitsOrderHook();
	hooks::injectRightClickOrderHooks();
	hooks::injectScannerOrdersHook();
	hooks::injectDefensiveMatrixHooks();
	hooks::injectPatrolOrderHook();
	hooks::injectRescuableOrderHook();
	hooks::injectCritterOrderHook();
	hooks::injectDoodadOrdersHooks();
	hooks::injectWarpinOrderHook();
	hooks::injectJunkYardDogOrderHook();

	hooks::injectApplyUpgradeFlags();
	hooks::injectAttackPriorityHooks();
	hooks::injectBunkerHooks();
	hooks::injectCloakNearbyUnits();
	hooks::injectCloakingTechHooks();
	hooks::injectDetectorHooks();
	hooks::injectHarvestResource();
	hooks::injectRallyHooks();
	hooks::injectRechargeShieldsHooks();
	hooks::injectStimPacksHooks();
	hooks::injectTechTargetCheckHooks();
	hooks::injectTransferTechAndUpgradesHooks();
	hooks::injectUnitSpeedHooks();
	hooks::injectUpdateStatusEffects();
	hooks::injectUpdateUnitState();
	hooks::injectWeaponCooldownHook();
	hooks::injectWeaponDamageHook();
	hooks::injectWeaponFireHooks();

	hooks::injectUnitDestructorSpecial();
	hooks::injectPsiFieldHooks();

	hooks::injectArmorBonusHook();
	hooks::injectSightRangeHook();
	hooks::injectUnitMaxEnergyHook();
	hooks::injectWeaponRangeHooks();

	hooks::injectUnitTooltipHook();

	hooks::injectDatLoadHooks();

	//fixes to make sc1 campaign playable from firegraft/mpqgraft self-executables
	//jmpPatch((void*)0x150182D0, 0x004101AE);
	//jmpPatch((void*)0x15017DD0, 0x004100B2);
	//jmpPatch((void*)0x15017960, 0x004100C4);
	//jmpPatch((void*)0x15014A80, 0x004100BE);

	return TRUE;
}
