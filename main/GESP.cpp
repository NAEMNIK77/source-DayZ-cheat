#include "GSDK.h"
#include "GDraw.h"
#include "GESP.h"
#include "GMenu.h"
#include "GPlayerManager.h"
#include "GTools.h"
#include "GAimbot.h"
#include "GOverlay.h"


void GESP::Reset()
{
	zombie_near = false;
	zombie_closer = false;
	zombie_attention = false;
	zombie_moving = false;

	count_zombies = 0;
	count_players = 0;
	count_items = 0;

	cur_speed = 0;

	inventory_box_x = 0;

	nearest_obj = NULL;
	obj_dist = 0;
}

bool list_sort_by_distance(const CObject *first, const CObject *second)
{
	float first_dist = g_pPlayerMgr->CalcDistance(first);
	float second_dist = g_pPlayerMgr->CalcDistance(second);

	return first_dist > second_dist;
}

void GESP::Render()
{
	DoTest();
	DoCity();

	g_pPlayerMgr->ClearTeleportList();

	list_items.clear();
	list_animals.clear();
	list_effects.clear();

	//CWorld *w = (CWorld*)g_pPlayerMgr->GetBase()->World;
	//if (w) {
	//	int count = w->unkTable7.size;
	//	CObject** obj = w->unkTable7.obj;

	//	for (int index = 0; index < count; index++) {
	//		if (!obj || IsBadToRead((void*)obj, 4))
	//			continue;

	//		if (IsBadToRead((void*)((DWORD)obj + index * 4), 4))
	//			continue;

	//		if (!obj[index] || IsBadToRead((void*)obj[index], 4))
	//			continue;

	//		if (!g_pPlayerMgr->IsValid(obj[index]))
	//			continue;

	//		GVector out;
	//		if (g_pPlayerMgr->WorldToScreen(obj[index], out)) {
	//			char* name = g_pPlayerMgr->GetObjectClassName(obj[index]);
	//			float dist = g_pPlayerMgr->CalcDistance(obj[index]); //hier crashts lol

	//			g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y, color_white, "%s [%.0f]", name, dist);
	//			DoItem(obj[index]);
	//		}
	//	}
	//}

	for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::ITEM); i++)
		list_items.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::ITEM, i));

	for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::DROPPED_ITEM); i++)
		list_items.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::DROPPED_ITEM, i));

	//for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::UNK); i++) {
	//	
	//	CObject *obj = g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::UNK, i);
	//	if (!obj)
	//		continue;

	//	GVector out;
	//	if (g_pPlayerMgr->WorldToScreen(obj, out)) {
	//		char* name = g_pPlayerMgr->GetObjectClassName(obj);
	//		float dist = g_pPlayerMgr->CalcDistance(obj); //hier crashts lol

	//		g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y, color_white, "%s %s[%.0f]", name, g_pPlayerMgr->GetObjectName(obj), dist);
	//	}
	//}

	if (cvars.ESP_Far) {

		for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::FAR_DROPPED_ITEM); i++) 
			list_items.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::FAR_DROPPED_ITEM, i));

		if (cvars.ESP_House) {
			
			for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::FAR_BUILDING); i++) 
				list_items.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::FAR_BUILDING, i));

			/*for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::FAR_BUILDING_2); i++)
				list_items.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::FAR_BUILDING_2, i));

			for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::FAR_BUILDING_3); i++)
				list_items.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::FAR_BUILDING_3, i));*/
		}
	}

	if (cvars.ESP_Animals) {

		for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::ANIMALS); i++)
			list_animals.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::ANIMALS, i));

		if (cvars.ESP_Far) {
			for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::FAR_ANIMALS); i++)
				list_animals.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::FAR_ANIMALS, i));

			for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::FARFAR_ANIMALS); i++)
				list_animals.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::FARFAR_ANIMALS, i));
		}
	}

	if (cvars.ESP_Effects) {

		for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::EFFECTS); i++)
			list_effects.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::EFFECTS, i));

		for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::SMOKE); i++)
			list_effects.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::SMOKE, i));

		if (cvars.ESP_Far)
			for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::FAR_EFFECTS); i++)
				list_effects.push_back(g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::FAR_EFFECTS, i));
	}

	list_items.unique();
	list_animals.unique();
	list_effects.unique();

	list_items.sort(list_sort_by_distance);
	list_animals.sort(list_sort_by_distance);
	list_effects.sort(list_sort_by_distance);

	for (auto &it : list_items)
		DoItem(it);

	for (auto &it : list_animals)
		DoAnimals(it);

	for (auto &it : list_effects)
		DoEffects(it);

	if (g_pInput->IsKeyDown(VK_NUMPAD1) & 1)
		g_pPlayerMgr->DoTeleport();
}

const DWORD diff_time = 1000;

struct velocity {
	GVector pos;
	GVector old_pos;
	float speed;
	bool nextFrame;
	GTimer timer;
}; velocity vel = { 0 };

//16 248-267; 47 315-331; 99 370-385 weapon eingesteckt
//25 268-283; 74 335-350; 386-401; 25, weapon gezogen
//122 eating/drinking

bool IstWaffeGezogen(CObject *obj)
{
	int s = obj->state; //0xCCC

	//if (s >= 248 && s <= 267 ||
	//	s >= 315 && s <= 331 ||
	//	s >= 370 && s <= 385 ||
	//	s == 16 || s == 47 || s == 99)
	//	return "Waffe eingesteckt";

	if (s >= 268 && s <= 283 ||
		s >= 335 && s <= 350 ||
		s >= 386 && s <= 401 ||
		s == 25 || s == 74 || s == 25)
		return true;

	return false;
}

char *szState[] = {"", "Running", "Consuming", "Moving", "Moving (Kneeling)", "Moving (Prone)", "Standing", "Kneeling", "Prone", "Unconscious", "Bleeding", "Bleeding/Unconscious", "Shock"};

char GetState(CObject *obj, int i)
{

	player_info info = { 0 };
	g_pPlayerMgr->GetPlayerHealthBlood(obj, info);

	if ((info.unconscious > 0 || obj->state == 825) && info.bleedingeffects > 0)
		return 11;

	if (info.bleedingeffects > 0)
		return 10;

	if (info.unconscious > 0)
		return 9;

	if (obj->state == 825)
		return 9;

	if (info.shock > 0) {
		return 12;
	}
	
	if (obj->state == 179 || obj->state == 451)
		return 1;

	if (obj->state == 122)
		return 2;

	//if (obj->state2 == 2) //0xCC4 37 23 8 179 182 116 177
	//	return "Jumping";

	//if (obj->state == 37 || obj->state == 23 || obj->state == 8 || 
	//	obj->state == 179 || obj->state == 182 || obj->state == 116 || 
	//	obj->state == 177)
	//	return "Jumping";

	if (obj->stance == STANCE::STANDING && (obj->visualstate->speed.Mag() || g_pPlayerMgr->GetVelocity(i)))
		return 3;

	if (obj->stance == STANCE::KNEEING && (obj->visualstate->speed.Mag() || g_pPlayerMgr->GetVelocity(i)))
		return 4;

	if (obj->stance == STANCE::PRONE && (obj->visualstate->speed.Mag() || g_pPlayerMgr->GetVelocity(i)))
		return 5;

	if (obj->stance == STANCE::STANDING)
		return 6;

	if (obj->stance == STANCE::KNEEING)
		return 7;

	if (obj->stance == STANCE::PRONE)
		return 8;

	return 0;
}

void GESP::DoPlayer(int i)
{
	if (!cvars.ESP_Player && !cvars.ESP_Zombie && !cvars.ESP_Radar && !cvars.ESP_View && !cvars.AIM_On)
		return;

	CObject *pLocalPlayer = g_pPlayerMgr->GetLocalPlayer();
	if (!pLocalPlayer)
		return;

	if (!g_pPlayerMgr->IsConnected()) {
		vel.speed = 0;
		return;
	}

	float c = vel.timer.get_ms();
	if (vel.nextFrame && c >= 1000) {
		vel.pos = pLocalPlayer->visualstate->pos;
		vel.speed = g_pPlayerMgr->CalcDistance(vel.old_pos, vel.pos) / (c / 1000);
		vel.nextFrame = false;
	}

	if (!vel.nextFrame) {
		vel.old_pos = pLocalPlayer->visualstate->pos;
		vel.timer.start();
		vel.nextFrame = true;
	}

	cur_speed = vel.speed;


	CObject *pPlayer = g_pPlayerMgr->GetPlayerByIndex(i);
	if (!pPlayer)
		return;

	if (pPlayer == pLocalPlayer)
		return;

	bool zombie = g_pPlayerMgr->IsZombie(pPlayer);

	if (pPlayer->isDead && !cvars.ESP_ShowDead)
		return;

	if (zombie && pPlayer->isDead)
		return;

	float dist = g_pPlayerMgr->CalcDistance(pPlayer);

	if (dist >= 1000 && !pPlayer->isDead)
		return;

	g_pPlayerMgr->CalcVelocity(i);

	GColor color = color_lightblue;

	if (zombie)
		count_zombies++;
	else
		count_players++;

	if (zombie && !cvars.ESP_Zombie)
		return;

	if (!zombie && !cvars.ESP_Player)
		return;

	if (zombie)
		color = color_firebrick;

	char *name = g_pPlayerMgr->GetPlayerName(pPlayer);
	char *buf = pPlayer->isDead ? "(DEAD)" : (zombie ? "Z" : (name ? name : "(LOADING)"));

	if (cvars.ESP_Radar && !pPlayer->isDead) {

		GVector p_view = g_pPlayerMgr->GetPlayerViewangles(pPlayer);
		GVector view = g_pPlayerMgr->GetViewangles();

		GVector pos_view = pPlayer->visualstate->head_pos;
		pos_view += g_pPlayerMgr->GetPlayerForwardVector(pPlayer, 50);

		GVector pt1, pt2;

		CalcRadarPoint(pPlayer->visualstate->head_pos, pLocalPlayer->visualstate->head_pos, view.y, pt1);
		CalcRadarPoint(pos_view, pLocalPlayer->visualstate->head_pos, view.y, pt2);

		g_pOverlay->DirectX->Draw->FilledQuad(pt1.x - 1, pt1.y - 1, 3, 3, color);
		if (dist < cvars.radar_range)
			g_pOverlay->DirectX->Draw->Line(pt1.x, pt1.y, pt2.x, pt2.y, color);
	}

	int h = 0;
	int height = g_pOverlay->DirectX->Draw->GetFontHeight();

	if (g_pPlayerMgr->EnemyAimingAtYou(pPlayer)) {

		g_pOverlay->DirectX->Draw->TextCenteredBg(cvars.Screen.x / 2, 50 + h, color_green, "%s IS AIMING AT YOU", zombie ? "Zombie" : buf);
	}

	if (!zombie && !pPlayer->isDead) {

		float k = g_pPlayerMgr->GetVelocity(i) * 3.6;
		if (k > 50 && k < 500) {
			h += height;
			g_pOverlay->DirectX->Draw->TextCenteredBg(cvars.Screen.x / 2, 50 + h, color_green, "%s IS TELEPORTING WITH A SPEED OF %.0f km/h", buf, k);
		}
	}

	if (cvars.MISC_ZombieWarning && !pPlayer->isDead && zombie) {

		if (dist < 30) {

			zombie_near = true;

			if (dist <= 20 && dist >= 10)
				zombie_closer = true;
			else if (dist < 10 && dist >= 0)
				zombie_attention = true;
		}
		else if (dist >= 30 && dist < cvars.radar_range) {

			if (g_pPlayerMgr->GetVelocity(i) > 1)
				zombie_moving = true;
		}
	}

	GVector out;
	if (!g_pPlayerMgr->WorldToScreen(pPlayer->visualstate->head_pos, out))
		return;

	if (cvars.ESP_View && dist < 50) {

		GVector pos_view = pPlayer->visualstate->head_pos;
		pos_view += g_pPlayerMgr->GetPlayerForwardVector(pPlayer);

		GVector screen_1;
		if (g_pPlayerMgr->WorldToScreen(pos_view, screen_1))
			g_pOverlay->DirectX->Draw->Line(out.x, out.y, screen_1.x, screen_1.y, color);
	}

	if (cvars.ESP_Box && dist < 100) {

		GVector up_left(pPlayer->visualstate->head_pos);
		GVector down_right(pPlayer->visualstate->pos);

		up_left.y += 0.1;

		GVector view = g_pPlayerMgr->GetViewangles();
		float breite = 0.5;

		up_left -= g_pPlayerMgr->GetRightVector(breite);
		down_right += g_pPlayerMgr->GetRightVector(breite);

		if (pPlayer->stance == STANCE::PRONE || pPlayer->isDead) {

			up_left = pPlayer->visualstate->head_pos - g_pPlayerMgr->GetUpVector(breite);
			down_right = pPlayer->visualstate->pos;
		}

		GVector out_upleft, out_downright;

		bool res1 = g_pPlayerMgr->WorldToScreen(up_left, out_upleft);
		bool res2 = g_pPlayerMgr->WorldToScreen(down_right, out_downright);

		if (res1 && res2) {

			int box[] = { 
				out_upleft.x, 
				out_upleft.y,
				out_downright.x - out_upleft.x, 
				out_downright.y - out_upleft.y
			};

			g_pOverlay->DirectX->Draw->Quad(box[0], box[1], box[2], box[3], 1, color);
		}
	}

	player_info info = { 0 };
	g_pPlayerMgr->GetPlayerHealthBlood(pPlayer, info);


//	bool isVisible = g_pPlayerMgr->IsVisible(pLocalPlayer->visualstate->head_pos, pPlayer->visualstate->head_pos);

	if ((dist < 50 || g_pPlayerMgr->IsFriend(pPlayer) || g_pPlayerMgr->InCenterArea(out))) {

		if (cvars.ESP_Name || g_pPlayerMgr->IsFriend(pPlayer)) 
				g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y, color, "%s[%.0f] %d", buf, dist, pPlayer->state);

			
		if (cvars.ESP_Dot)
			g_pOverlay->DirectX->Draw->FilledQuad(out.x, out.y, 3, 3, color);

		textsize size;
		g_pOverlay->DirectX->Draw->GetTextLength(buf, &size);

		int j = 0;


		if (!zombie) {

			CollectWeapon(pPlayer);

			char *weapon_name = g_pPlayerMgr->GetPlayerWeaponName(pPlayer);
			if (weapon_name && cvars.ESP_Weapon && pPlayer->weapon && pPlayer->weapon->object) {

				j += size.h;

				/*static int freeslot = -1;

				if (freeslot == -1) {
					for (int k = 0; k < 12; k++) {
						if (g_pPlayerMgr->GetLocalPlayer()->InventarSlot[k] == 0) {
							freeslot = k;
							break;
						}
					}
				}

				if (GetAsyncKeyState(VK_NUMPAD6) && freeslot != -1) {
					g_pPlayerMgr->GetLocalPlayer()->InventarSlot[freeslot] = (CItem*)pPlayer->weapon->object;
				}

				if (GetAsyncKeyState(VK_NUMPAD7) && freeslot != -1) {
					g_pPlayerMgr->GetLocalPlayer()->InventarSlot[freeslot] = 0;
					freeslot = -1;
				}*/

				GItem item(pPlayer->weapon->object);

				if (item.GetType() == GItem::ITEM_TYPE::T_WEAPON) {

					if (!pPlayer->weapon->object->magazine && !pPlayer->weapon->object->magazine->object) {

						g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y + j, color, "-%s%s%s (no magazine)-", IstWaffeGezogen(pPlayer) ? "*" : "", weapon_name, IstWaffeGezogen(pPlayer) ? "*" : "");
					}
					else if (pPlayer->weapon && pPlayer->weapon->object){

						int maxammo = 0;
						int curammo = g_pPlayerMgr->GetMagazineAmmo((CObject*)pPlayer->weapon->object, &maxammo);

						if (maxammo == 0 || pPlayer->weapon->object->magazine && pPlayer->weapon->object->magazine->object) {
							curammo = g_pPlayerMgr->GetMagazineAmmo((CObject*)pPlayer->weapon->object->magazine->object, &maxammo);
						}

						if (maxammo)
							g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y + j, color, "-%s%s%s (%d|%d)-", IstWaffeGezogen(pPlayer) ? "*" : "", weapon_name, IstWaffeGezogen(pPlayer) ? "*" : "", curammo, maxammo);
						else
							g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y + j, color, "-%s%s%s-", IstWaffeGezogen(pPlayer) ? "*" : "", weapon_name, IstWaffeGezogen(pPlayer) ? "*" : "");
					}
				}
				else {

					g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y + j, color, "-%s-", weapon_name);
				}
			}

			if (cvars.ESP_Inventar && GetAsyncKeyState(VK_LSHIFT) && g_pPlayerMgr->InCenterArea(out)) {
				DoPlayerInventory(pPlayer);
			}

			if (cvars.ESP_Health && info.health > 0 && info.blood > 0) {
				j += size.h;
				g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y + j, color, "-H(%.0f)|B(%.0f) %.2f", info.health / 50, info.blood / 50, info.shock);
			}
		}

		char state = GetState(pPlayer, i);
		if (cvars.ESP_State && state) {
			j += size.h;

			if (state == 1 || state == 3 || state == 4 || state == 5)
				g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y + j, color, "-%s (%.0f km/h)-", szState[state], g_pPlayerMgr->GetVelocity(i)*3.6);
			else
				g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y + j, color, "-%s-", szState[state]);
		}
	}
	else if (!cvars.ESP_Box || dist >= 100)
		g_pOverlay->DirectX->Draw->CircleCross(out.x, out.y, color);
}

void GESP::DoCity()
{
	if (!cvars.ESP_City)
		return;

	if (!g_pPlayerMgr->IsConnected())
		return;

	int size = sizeof(city_pos) / sizeof(city_pos_t);

	for (register int i = 0; i < size; i++) {

		GVector out;
		if (!g_pPlayerMgr->WorldToScreen(city_pos[i].pos, out))
			continue;

		float dist = g_pPlayerMgr->CalcDistance(city_pos[i].pos);

		if (dist < city_pos[i].seeing_dist) {
			if (g_pPlayerMgr->InCenterArea(out) || (dist < CITY_MID_DIST)) {
				g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y, color_deeppink, "%s[%.0f]", city_pos[i].name, dist);
				g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y + 12, color_deeppink, "(%.0f sec)", dist / (vel.speed > 2.2 ? vel.speed : 6.4));
			}
			else
				g_pOverlay->DirectX->Draw->CircleCross(out.x, out.y, color_deeppink);		
		}
	}
}

void GESP::DoItem(CObject *obj2)
{

	CItem *obj = reinterpret_cast<CItem*>(obj2);

	if (!obj)
		return;

	if (!g_pPlayerMgr->IsValid(obj))
		return;

	if (!cvars.ESP_Clothing && !cvars.ESP_House && !cvars.ESP_Inventory && !cvars.ESP_Magazine && !cvars.ESP_Unfiltered && !cvars.ESP_Weapons && !cvars.ESP_Other)
		return;


	CObject *pLocalPlayer = g_pPlayerMgr->GetLocalPlayer();
	if (!pLocalPlayer)
		return;

	if (!g_pPlayerMgr->IsConnected())
		return;

	count_items++;

	float dist = g_pPlayerMgr->CalcDistance(obj); // da crashts?

	if (dist > 150 && !cvars.ESP_Far)
		return;

	GColor color = color_grey;
	GItem item(obj);

	if (!item.Valid())
		return;

	if (item.IsRuined() && !cvars.ESP_Ruined)
		return;


	char item_type = item.GetType();
	char* item_name = item.GetClearName();

	if ((item_type == GItem::ITEM_TYPE::NONE && !cvars.ESP_Unfiltered) || !item_name)
		return;

	if (item_type == GItem::ITEM_TYPE::T_CLOTHING && cvars.ESP_Clothing) {

		char item_type_clothing = item.GetTypeClothing();
		color = color_darkgreen;

		if (item_type_clothing == GItem::ITEM_TYPE_CLOTHING::ATHLETICS_SHOES ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::BAG_CHILDS ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::BANDANA ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::BASEBALL_CAP ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::BEANIE_HAT ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::BOONIE_HAT ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::COWBOY_HAT ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::DALLAS_MASK ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::FLAT_CAP ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::HIKING_BOOTS ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::JOGGING_SHOES ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::JEANS ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::HOODIE ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::JUNGLE_BOOTS ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::LEATHER_SHOES ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::RADAR_CAP ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::RAIN_COAT ||
			//item_type_clothing == GItem::ITEM_TYPE_CLOTHING::SHIRT ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::TSHIRT ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::ZMIJOVKA_CAP ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::WOOL_COAT ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::CANVAS_PANTS ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::USHANKA ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::CONSTRUCTION_HELMET ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::WORKING_GLOVES ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::WORKING_BOOTS ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::MOTO_HELMET ||
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::BUBBLE_GOOSE || 
			item_type_clothing == GItem::ITEM_TYPE_CLOTHING::WELLIES)
			return;

		if (cvars.ESP_Teleport == 7)
			g_pPlayerMgr->AddTeleportList(obj);
	}
	else if (item_type == GItem::ITEM_TYPE::T_INVENTORY && cvars.ESP_Inventory) {

		char item_type_inventory = item.GetTypeInventory();
		color = color_chocolate;

		if (item_type_inventory == GItem::ITEM_TYPE_INVENTORY::FRUIT ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::FOOD ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::DRINK) {

			if (strstr(item_name, "Rotten")/* ||
				strstr(item_name, "_Open")*/)
				return;

			if (cvars.ESP_Teleport == 4)
					g_pPlayerMgr->AddTeleportList(obj);

			color = color_cadetblue;
		}
		else if (item_type_inventory == GItem::ITEM_TYPE_INVENTORY::CRAFTING ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::COOKER ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::COOKWARE ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::FARMING_HOE ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::BASEBALL_BAT ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::COMBAT_KNIFE ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::PEN ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::SPRAY_CAN ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::GLASSES ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::PIPE_WRENCH ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::PITCH_FORK ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::ROCKET_AVIATORS)
			return;

		else if (item_type_inventory == GItem::ITEM_TYPE_INVENTORY::AMMO ||
			item_type_inventory == GItem::ITEM_TYPE_INVENTORY::ATTACHMENT) {
			color = color_orange;

			if (cvars.ESP_Teleport == 3)
					g_pPlayerMgr->AddTeleportList(obj);
		}

		else if (item_type_inventory == GItem::ITEM_TYPE_INVENTORY::CONSUMEABLE) {
				
			color = color_yellow;

			if (strstr(item_name, "Duct Tape") || strstr(item_name, "Firewood") ||
				strstr(item_name, "Stone") || strstr(item_name, "Rags") || strstr(item_name, "Paper") ||
				strstr(item_name, "Matchbox"))
				return;
		}

		else if (item_type_inventory == GItem::ITEM_TYPE_INVENTORY::CONTAINER) {

			if (strstr(item_name, "First Aid Kit")) {

				color = color_white;

				if (cvars.ESP_Teleport == 5)
					g_pPlayerMgr->AddTeleportList(obj);
			}
			else {

				if (strstr(item_name, "Round Box")) {

					if (cvars.ESP_Teleport == 2)
						g_pPlayerMgr->AddTeleportList(obj);
				}

				color = color_orange;
			}
		}

		else if (item_type_inventory == GItem::ITEM_TYPE_INVENTORY::LIGHT) {
			color = color_yellow;

			if (cvars.ESP_Teleport == 6)
					g_pPlayerMgr->AddTeleportList(obj);
		}

		else if (item_type_inventory == GItem::ITEM_TYPE_INVENTORY::MEDICAL) {
			color = color_white;

			if (cvars.ESP_Teleport == 5)
					g_pPlayerMgr->AddTeleportList(obj);
		}

		else if (item_type_inventory == GItem::ITEM_TYPE_INVENTORY::TOOL) {
			color = color_yellow;

			if (strstr(item_name, "Hammer") || strstr(item_name, "Fire Extinguisher") ||
				strstr(item_name, "Wrench") || strstr(item_name, "Sewing Kit") ||
				strstr(item_name, "Shovel") || strstr(item_name, "Pliers") ||
				strstr(item_name, "Weapon Cleaning Kit"))
				return;

			if (cvars.ESP_Teleport == 6)
					g_pPlayerMgr->AddTeleportList(obj);
		}

	}
	else if (item_type == GItem::ITEM_TYPE::T_MAGAZINE && cvars.ESP_Magazine) {
		color = color_orange;

		if (strstr(item_name, "(coupled)")) 
			color = color_deeppink;

		int ammo = g_pPlayerMgr->GetMagazineAmmo(reinterpret_cast<CObject*>(obj));
		if (ammo == 0 && !cvars.ESP_Ruined)
			return;

		if (cvars.ESP_Teleport == 2)
				g_pPlayerMgr->AddTeleportList(obj);
	}
	else if (item_type == GItem::ITEM_TYPE::T_WEAPON && cvars.ESP_Weapons) {
		color = color_maroon;

		if (cvars.ESP_Teleport == 1)
				g_pPlayerMgr->AddTeleportList(obj);

	}
	else if (item_type == GItem::ITEM_TYPE::T_COMPASS && cvars.ESP_Other) {
		color = color_chocolate;
	}
	else if (item_type == GItem::ITEM_TYPE::T_HOUSE && cvars.ESP_House) {
		color = color_pink;
	}
	else if (item_type == GItem::ITEM_TYPE::T_OPTICS && cvars.ESP_Weapons) {
		color = color_orange;

		if (cvars.ESP_Teleport == 2)
			g_pPlayerMgr->AddTeleportList(obj);
	}
	else if (item_type == GItem::ITEM_TYPE::T_BOOK && cvars.ESP_Other)
		return;
	else if (item_type == GItem::ITEM_TYPE::T_MAP && cvars.ESP_Other)
		return;
	else if (item_type == GItem::ITEM_TYPE::T_TRANSMITTER && cvars.ESP_Other)
		return;
	else if (!cvars.ESP_Unfiltered)
		return;


	if (cvars.ESP_Radar && item_type == GItem::ITEM_TYPE::T_WEAPON) {

		GVector view = g_pPlayerMgr->GetViewangles();

		GVector pt1;
		CalcRadarPoint(obj->visualstate->pos, pLocalPlayer->visualstate->pos, view.y, pt1);

		g_pOverlay->DirectX->Draw->FilledQuad(pt1.x - 1, pt1.y - 1, 2, 2, color_pink);
	}


	GVector out;
	if (!g_pPlayerMgr->WorldToScreen(obj->visualstate->pos, out))
		return;

	bool inarea = g_pPlayerMgr->InCenterArea(out);

	char itemname[512] = "";

	if (strstr(item.GetName(), /*FuelStation*/XorStr<0xd1, 12, 0x97fe6835>("\x97\xa7\xb6\xb8\x86\xa2\xb6\xac\xb0\xb5\xb5" + 0x97fe6835).s))
		strcpy(itemname, /*Fuel Station*/XorStr<0x58, 13, 0xefbe56f2>("\x1e\x2c\x3f\x37\x7c\xe\x2a\x3e\x14\x8\xd\xd" + 0xefbe56f2).s);
	else
		strcpy(itemname, item_name);

	if (item.GetType() == GItem::ITEM_TYPE::T_MAGAZINE) {
		
		int max_ammo = 0;
		int ammo = g_pPlayerMgr->GetMagazineAmmo(reinterpret_cast<CObject*>(obj), &max_ammo);

		char buf[100] = { 0 };
		sprintf_s(buf, 100, "(%d|%d)", ammo, max_ammo);
		strcat(itemname, buf);
	}

	if (cvars.ESP_Box && dist < 50) {

		int boxsize = 125/dist;
		g_pOverlay->DirectX->Draw->Quad(out.x - boxsize / 2, out.y - boxsize / 2, boxsize, boxsize, 1, color);
	}

	textsize size;
	g_pOverlay->DirectX->Draw->GetTextLength(itemname, &size);

	int j = 0;

	if (dist < 50 || inarea) {

		if (GetAsyncKeyState(VK_NUMPAD4)) {
			//g_pPlayerMgr->ItemMoveIn(obj);
			g_pPlayerMgr->ItemMoveOut(g_pPlayerMgr->GetBase()->World->LocalPlayer, obj);
			//g_pPlayerMgr->ItemMoveIn(obj);
		}

		//if (GetAsyncKeyState(VK_NUMPAD9)) {
		//	g_pPlayerMgr->DropItem(pLocalPlayer, obj);
		//}

		j += size.h;

		g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y + j - 12, color, "%s%s[%.0f]", item.IsRuined() ? "*" : "", itemname, dist);

		for (auto &it : g_pPlayerMgr->GetItemContainerContent(obj)) {

			j += size.h;

			GItem sub_item(it);
			g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y + j - 12, color, "-%s%s-", sub_item.IsRuined() ? "*" : "", sub_item.GetClearName());
		}
	}
	else if (!cvars.ESP_Box || dist >= 50)
		g_pOverlay->DirectX->Draw->Cross(out.x, out.y, color);
}

void GESP::DoEffects(CObject *obj)
{
	if (!obj)
		return;

	float dist = g_pPlayerMgr->CalcDistance(obj);
	if (dist > 950)
		return;

	char *class_name = g_pPlayerMgr->GetObjectClassName(obj);
	if (!class_name)
		return;

	if (strstr(class_name, "#mark") || strstr(class_name, "FxWindPollen") ||
		strstr(class_name, "#particlesource") || strstr(class_name, "#crater")) //#particlesource #crateronvehicle #crater
		return;

	GVector out;
	if (!g_pPlayerMgr->WorldToScreen(obj, out))
		return;

	g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y, color_white, "%s", class_name);
}

void GESP::DoAnimals(CObject *obj)
{
	if (!obj)
		return;

	float dist = g_pPlayerMgr->CalcDistance(obj);

	if (dist > 950)
		return;

	if (dist > 100 && !cvars.ESP_Far)
		return;

	if (obj->isDead && !cvars.ESP_Ruined)
		return;

	if (!g_pPlayerMgr->IsAnimal(obj))
		return;

	GVector out;
	if (!g_pPlayerMgr->WorldToScreen(obj->visualstate->body_pos, out))
		return;

	g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y, color_white, "%s[%.0f]", g_pPlayerMgr->GetObjectName(obj), dist);
}

std::list<GVector*> log_posi;

void GESP::DoTest()
{
	CObject *pLocalPlayer = g_pPlayerMgr->GetLocalPlayer();
	if (!pLocalPlayer)
		return;

	if (!g_pPlayerMgr->IsConnected())
		return;

	if (g_pInput->IsKeyDown(VK_NUMPAD5) & 1) {

		GVector* tmp = new GVector(pLocalPlayer->visualstate->pos);

		g_pTools->Log(/*Position: %.2f, %.2f, %.2f*/XorStr<0x55, 27, 0x51ecb920>("\x5\x39\x24\x31\x2d\x33\x34\x32\x67\x7e\x7a\x4e\x53\x4\x4f\x44\x40\x48\x55\xe\x45\x4a\x4e\x42\x5f\x8" + 0x51ecb920).s, tmp->x, tmp->y, tmp->z);

		log_posi.push_back(tmp);
	}

	for (auto &it : log_posi) {

		GVector* tmp = it;
		GVector out;

		if (!g_pPlayerMgr->WorldToScreen(*tmp, out))
			continue;

		g_pOverlay->DirectX->Draw->FilledQuad(out.x, out.y, 5, 5, color_pink);

		float dist = g_pPlayerMgr->CalcDistance(pLocalPlayer->visualstate->pos, *tmp);

		GVector angle = GMath::CalcAngle(*tmp, pLocalPlayer->visualstate->head_pos);

		g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y, color_pink, "[%.0f] (%.2f %.2f)", dist, angle.x, angle.y);
		g_pOverlay->DirectX->Draw->TextCenteredBg(out.x, out.y + 12, color_pink, "(%.2f sec)", dist / (vel.speed > 2.2 ? vel.speed : 6.4));

	}
}

void GESP::DoPlayerInventory(CObject *obj)
{
	int max_width = 0;
	int lines = 0;

/*	for (int s = 0; s < 12; s++) {

		CItem *item = obj->InventarSlot[s];
		if (!g_pPlayerMgr->IsValid(item))
			continue;

		lines++;

		CItemContainer *container = item->ItemContainer;

		textsize size;
		g_pOverlay->DirectX->Draw->GetTextLength(GItem(item).GetClearName(), &size);

		if (item->ruined) {

			textsize size2;
			g_pOverlay->DirectX->Draw->GetTextLength("*", &size2);
			size.w += size2.w;
		}

		if (size.w > max_width)
			max_width = size.w;

		for (auto &lt : g_pPlayerMgr->GetWeaponSlots(reinterpret_cast<CWeapon*>(item))) {

			g_pOverlay->DirectX->Draw->GetTextLength(GItem(lt).GetClearName(), &size);

			if (lt->ruined) {

				textsize size2;
				g_pOverlay->DirectX->Draw->GetTextLength("*", &size2);
				size.w += size2.w;
			}

			size.w += 12;
			if (size.w > max_width)
				max_width = size.w;

			lines++;
		}

		for (auto &it : g_pPlayerMgr->GetItemContainerContent(item)) {

			g_pOverlay->DirectX->Draw->GetTextLength(GItem(it).GetClearName(), &size);

			if (it->ruined) {

				textsize size2;
				g_pOverlay->DirectX->Draw->GetTextLength("*", &size2);
				size.w += size2.w;
			}

			int max_ammo = 0;
			int cur_ammo = g_pPlayerMgr->GetMagazineAmmo((CObject*)it, &max_ammo);
			if (max_ammo && GItem(it).GetType() == GItem::ITEM_TYPE::T_MAGAZINE) {

				char buf2[100] = "";
				sprintf_s(buf2, 100, " [%d|%d]", cur_ammo, max_ammo);

				textsize size2;
				g_pOverlay->DirectX->Draw->GetTextLength(buf2, &size2);
				size.w += size2.w;
			}

			size.w += 12;
			if (size.w > max_width)
				max_width = size.w;

			lines++;

			for (auto &jt : g_pPlayerMgr->GetItemContainerContent(it)) {

				g_pOverlay->DirectX->Draw->GetTextLength(GItem(jt).GetClearName(), &size);

				if (jt->ruined) {

					textsize size2;
					g_pOverlay->DirectX->Draw->GetTextLength("*", &size2);
					size.w += size2.w;
				}

				int max_ammo = 0;
				int cur_ammo = g_pPlayerMgr->GetMagazineAmmo((CObject*)jt, &max_ammo);
				if (max_ammo && GItem(jt).GetType() == GItem::ITEM_TYPE::T_MAGAZINE) {

					char buf2[100] = "";
					sprintf_s(buf2, 100, " [%d|%d]", cur_ammo, max_ammo);

					textsize size2;
					g_pOverlay->DirectX->Draw->GetTextLength(buf2, &size2);
					size.w += size2.w;
				}

				size.w += 24;
				if (size.w > max_width)
					max_width = size.w;

				lines++;
			}

		}
	}*/


	int font_height = g_pOverlay->DirectX->Draw->GetFontHeight();
	int cur_height = 0;

	for (int s = 0; s < 12; s++) {

		CItem *item = obj->InventorySlot[s];
		if (!g_pPlayerMgr->IsValid(item))
			continue;

		cur_height += font_height;

		GColor green = color_green;

		if (obj->weapon && reinterpret_cast<CWeapon*>(item) == obj->weapon->object)
			green = color_darkgreen;
	
		g_pOverlay->DirectX->Draw->AddText(60 + inventory_box_x, 65 + cur_height, green, FW1_LEFT, "%s%s", item->ruined ? "*" : "", GItem(item).GetClearName());

		for (auto &lt : g_pPlayerMgr->GetWeaponSlots(reinterpret_cast<CWeapon*>(item))) {

			cur_height += 12;
			g_pOverlay->DirectX->Draw->AddText(60 + inventory_box_x, 65 + cur_height, color_white, FW1_LEFT, "  %s%s", lt->ruined ? "*" : "", GItem(lt).GetClearName());
		}

		for (auto &it : g_pPlayerMgr->GetItemContainerContent(item)) {

			cur_height += font_height;

			int max_ammo = 0;
			int cur_ammo = g_pPlayerMgr->GetMagazineAmmo((CObject*)it, &max_ammo);

			if (max_ammo && GItem(it).GetType() == GItem::ITEM_TYPE::T_MAGAZINE)
				g_pOverlay->DirectX->Draw->AddText(60 + inventory_box_x, 65 + cur_height, color_white, FW1_LEFT, "  %s%s [%d|%d]", it->ruined ? "*" : "", GItem(it).GetClearName(), cur_ammo, max_ammo);
			else
				g_pOverlay->DirectX->Draw->AddText(60 + inventory_box_x, 65 + cur_height, color_white, FW1_LEFT, "  %s%s", it->ruined ? "*" : "", GItem(it).GetClearName());

			for (auto &jt : g_pPlayerMgr->GetItemContainerContent(it)) {

				cur_height += font_height;
				int max_ammo = 0;
				int cur_ammo = g_pPlayerMgr->GetMagazineAmmo((CObject*)jt, &max_ammo);

				if (max_ammo && GItem(jt).GetType() == GItem::ITEM_TYPE::T_MAGAZINE)
					g_pOverlay->DirectX->Draw->AddText(60 + inventory_box_x, 65 + cur_height, color_white, FW1_LEFT, "    %s%s [%d|%d]", it->ruined ? "*" : "", GItem(jt).GetClearName(), cur_ammo, max_ammo);
				else
					g_pOverlay->DirectX->Draw->AddText(60 + inventory_box_x, 65 + cur_height, color_white, FW1_LEFT, "    %s%s", it->ruined ? "*" : "", GItem(jt).GetClearName());
			}
		}
	}

	for (auto &it : g_pOverlay->DirectX->Draw->GetText()) {

		textsize buf;
		g_pOverlay->DirectX->Draw->GetTextLength(it.text, &buf);

		if (buf.w > max_width)
			max_width = buf.w;

		lines++;
	}


	char *name = g_pPlayerMgr->GetPlayerName(obj);
	char *buf = obj->isDead ? "(DEAD)" : (g_pPlayerMgr->IsZombie(obj) ? "Z" : (name ? name : "(LOADING)"));

	GColor black = GColor(0, 0, 0, 180);
	if (lines) {

		g_pOverlay->DirectX->Draw->FilledQuad(50 + inventory_box_x, 50, max_width + 18, lines * font_height + 35, black);
		g_pOverlay->DirectX->Draw->Quad(50 + inventory_box_x, 50, max_width + 18, lines * font_height + 35, 2, color_white);
		g_pOverlay->DirectX->Draw->Quad(50 + inventory_box_x, 70, max_width + 18, 1, 1, color_white);
		g_pOverlay->DirectX->Draw->TextCentered(50 + max_width / 2 + inventory_box_x, 54, color_lightblue, "%s", buf);

		inventory_box_x += max_width + 25;
	}

	g_pOverlay->DirectX->Draw->RenderText();
}

void GESP::CollectWeapon(CObject *obj)
{
	if (g_pPlayerMgr->IsFriend(obj))
		return;

	float dist = g_pPlayerMgr->CalcDistance(obj);


	//int weapon_index = -1;

	//if (!g_pPlayerMgr->IsFriend(obj)) {

	//	for (register int p = 0; p < 12; p++) {

	//		CItem *cur_slot = obj->InventorySlot[p];
	//		if (!cur_slot)
	//			continue;

	//		GItem item(cur_slot);

	//		for (auto &it : g_pPlayerMgr->GetItemContainerContent(cur_slot)) {

	//			if (!it) 
	//				continue;

	//			GItem item2(it);
	//			if (item2.GetType() == GItem::ITEM_TYPE::T_WEAPON) {

	//				weapon_index = p;
	//				break;
	//			}
	//		}

	//		if (item.GetType() == GItem::ITEM_TYPE::T_WEAPON) {

	//			weapon_index = p;
	//			break;
	//		}
	//	}
	//}

	//if (obj->weapon && obj->weapon->object) {

	//	GItem item(obj->weapon->object);
	//	if (item.GetType() == GItem::ITEM_TYPE::T_WEAPON) {

	//		if (dist > obj_dist && weapon_index == -1) {

	//			obj_dist = dist;
	//			nearest_obj = obj;
	//		}
	//	}
	//}

	if (dist > obj_dist/* && weapon_index != -1*/) {

		obj_dist = dist;
		nearest_obj = obj;
	}
}

void GESP::DoWeaponSteal()
{
	CObject *pLocalPlayer = g_pPlayerMgr->GetLocalPlayer();
	if (!pLocalPlayer)
		return;

	static int free_slot = -1;
	if (free_slot == -1) {

		for (int p = 0; p < 12; p++) {

			if (!pLocalPlayer->InventorySlot[p]) {

				free_slot = p;
				break;
			}
		}

	}

	//if (free_slot != -1 && g_pPlayerMgr->FindItem(pLocalPlayer->InventorySlot[free_slot])) {

	//	pLocalPlayer->InventorySlot[free_slot] = 0;
	//	free_slot = -1;
	//}

	if (!nearest_obj)
		return;

	if (g_pPlayerMgr->IsFriend(nearest_obj))
		return;

	//int weapon_index = -1;
	//CItem *buf = NULL;

	//if (!g_pPlayerMgr->IsFriend(nearest_obj)) {

	//	for (register int p = 0; p < 12; p++) {

	//		CItem *cur_slot = nearest_obj->InventorySlot[p];
	//		if (!cur_slot)
	//			continue;

	//		GItem item(cur_slot);
	//		if (item.GetType() == GItem::ITEM_TYPE::T_WEAPON) {

	//			weapon_index = p;
	//		}

	//		for (auto &it : g_pPlayerMgr->GetItemContainerContent(cur_slot)) {

	//			if (!it)
	//				continue;

	//			GItem item2(it);
	//			if (item2.GetType() == GItem::ITEM_TYPE::T_WEAPON) {

	//				buf = it;
	//				break;
	//			}
	//		}
	//	}
	//}

	if ((GetAsyncKeyState(VK_NUMPAD3) & 1) && !g_pPlayerMgr->IsFriend(nearest_obj)) {

		for (int i = 0; i < 12; i++) {

			if (!nearest_obj->InventorySlot[i])
				continue;

			pLocalPlayer->InventorySlot[11] = nearest_obj->InventorySlot[i];
			nearest_obj->InventorySlot[i] = 0;

			//g_pPlayerMgr->MoveItem(pLocalPlayer, pLocalPlayer->InventorySlot[free_slot]);
			//g_pPlayerMgr->ItemMoveIn(pLocalPlayer->InventorySlot[free_slot]);
			//g_pPlayerMgr->AskForDropItem(nearest_obj, pLocalPlayer->InventorySlot[free_slot]);
			g_pPlayerMgr->DropItem(pLocalPlayer, pLocalPlayer->InventorySlot[11]);
			//g_pPlayerMgr->MoveToGround(pLocalPlayer->InventorySlot[free_slot]);
			

			pLocalPlayer->InventorySlot[11] = 0;
			//g_pPlayerMgr->MoveToGround(nearest_obj->InventorySlot[i]);
			//g_pPlayerMgr->DropItem(nearest_obj, nearest_obj->InventorySlot[i]);

			//nearest_obj->InventorySlot[i] = 0;
		}

		free_slot = -1;
	}

	//if ((GetAsyncKeyState(VK_NUMPAD3) & 1) && !g_pPlayerMgr->IsFriend(nearest_obj)) {

	//	if (weapon_index != -1 && !IsBadToRead((void*)nearest_obj->InventorySlot[weapon_index], 4)) {

	//		pLocalPlayer->InventorySlot[free_slot] = nearest_obj->InventorySlot[weapon_index];
	//		nearest_obj->InventorySlot[weapon_index] = 0;
	//	}
	//	else if (buf && !IsBadToRead((void*)buf, 4)) {

	//		pLocalPlayer->InventorySlot[free_slot] = (CItem*)buf;

	//		for (register int p = 0; p < 12; p++) {

	//			CItem *cur_slot = nearest_obj->InventorySlot[p];
	//			if (!cur_slot)
	//				continue;

	//			if (g_pPlayerMgr->IsInItemContainer(cur_slot, buf))
	//				g_pPlayerMgr->RemoveFromItemContainer(cur_slot, buf);
	//		}

	//	}
	//	else if(nearest_obj->weapon && nearest_obj->weapon->object){
	//		pLocalPlayer->InventorySlot[free_slot] = (CItem*)nearest_obj->weapon->object;
	//		//nearest_obj->weapon->object = 0;
	//	}
	//	return;
	//}

	if (GetAsyncKeyState(VK_NUMPAD4)) {

		pLocalPlayer->InventorySlot[free_slot] = 0;
		free_slot = -1;
	}
}

void GESP::DoZombieWarning()
{
	if (zombie_near && !zombie_moving)
		g_pOverlay->DirectX->Draw->TextCenteredBg(cvars.Screen.x / 2, cvars.Screen.y / 2 - 350, color_red, /*!!! ZOMBIE IS NEAR YOUR POSITION !!!*/XorStr<0xf1, 37, 0xed54f056>("\xd0\xd3\xd2\xd4\xaf\xb9\xba\xba\xb0\xbf\xdb\xb5\xae\xde\xb1\x45\x40\x50\x23\x5d\x4a\x53\x55\x28\x59\x45\x58\x45\x59\x47\x40\x5e\x31\x33\x32\x35" + 0xed54f056).s);

	if (!zombie_near && zombie_moving) {
		g_pOverlay->DirectX->Draw->TextCenteredBg(cvars.Screen.x / 2, cvars.Screen.y / 2 - 350, color_red, /*!!! ZOMBIE IS MOVING !!!*/XorStr<0xfc, 25, 0x2c93fafb>("\xdd\xdc\xdf\xdf\x5a\x4e\x4f\x41\x4d\x40\x26\x4e\x5b\x29\x47\x44\x5a\x44\x40\x48\x30\x30\x33\x32" + 0x2c93fafb).s);
	}
	else if (zombie_near && zombie_closer && zombie_attention) {
		g_pOverlay->DirectX->Draw->TextCenteredBg(cvars.Screen.x / 2, cvars.Screen.y / 2 - 335, color_red, /*!!! COMING CLOSER !!!*/XorStr<0x7f, 22, 0x15474919>("\x5e\xa1\xa0\xa2\xc0\xcb\xc8\xcf\xc9\xcf\xa9\xc9\xc7\xc3\xde\xcb\xdd\xb0\xb0\xb3\xb2" + 0x15474919).s);
		g_pOverlay->DirectX->Draw->TextCenteredBg(cvars.Screen.x / 2, cvars.Screen.y / 2 - 320, color_red, /*!!! ATTENTION !!!*/XorStr<0xf5, 18, 0xc027f429>("\xd4\xd7\xd6\xd8\xb8\xae\xaf\xb9\xb3\xaa\xb6\x4f\x4f\x22\x22\x25\x24" + 0xc027f429).s);
	}
	else if (zombie_near && zombie_closer && !zombie_attention) {
		g_pOverlay->DirectX->Draw->TextCenteredBg(cvars.Screen.x / 2, cvars.Screen.y / 2 - 335, color_red, /*!!! COMING CLOSER !!!*/XorStr<0xfc, 22, 0x5d5fc5fb>("\xdd\xdc\xdf\xdf\x43\x4e\x4f\x4a\x4a\x42\x26\x44\x44\x46\x59\x4e\x5e\x2d\x2f\x2e\x31" + 0x5d5fc5fb).s);
	}
	else if (zombie_near && !zombie_closer && zombie_attention) {
		g_pOverlay->DirectX->Draw->TextCenteredBg(cvars.Screen.x / 2, cvars.Screen.y / 2 - 335, color_red, /*!!! ATTENTION !!!*/XorStr<0xdd, 18, 0x74fdc12>("\xfc\xff\xfe\xc0\xa0\xb6\xb7\xa1\xab\xb2\xae\xa7\xa7\xca\xca\xcd\xcc" + 0x74fdc12).s);
	}
}

void GESP::CalcRadarPoint(GVector object_pos, GVector local_pos, float local_yaw, GVector &out)
{
	float dx = object_pos.z - local_pos.z;
	float dy = object_pos.x - local_pos.x;

	dy *= -1;

	float SinYaw = sin(local_yaw);
	float CosYaw = cos(local_yaw);

	float x = dy*(-CosYaw) + dx*SinYaw;
	float y = dx*(-CosYaw) - dy*SinYaw;

	if (fabs(x)>cvars.radar_range || fabs(y)>cvars.radar_range) {
		if (y>x) {
			if (y>-x) {
				x = cvars.radar_range*x / y;
				y = cvars.radar_range;
			}
			else  {
				y = -cvars.radar_range*y / x;
				x = -cvars.radar_range;
			}
		}
		else {
			if (y>-x) {
				y = cvars.radar_range*y / x;
				x = cvars.radar_range;
			}
			else  {
				x = -cvars.radar_range*x / y;
				y = -cvars.radar_range;
			}
		}
	}

	out.x = ((cvars.Screen.x - cvars.radar_x) + 82) + int(x / cvars.radar_range * float(82));
	out.y = cvars.radar_y + 82 + int(y / cvars.radar_range * float(82));
}

bool GItem::Valid()
{
	if (!g_pPlayerMgr->IsValid(obj))
		return false;

	if (!obj || IsBadToRead((void*)obj, 4))
		return false;

	CObject *lol = (CObject*)obj;
	if (!lol->time)
	return false;

	if (!GetTypeName() || !GetName() || !GetClearName())
		return false;

	return true;
}

char *GItem::GetTypeName()
{
	return g_pPlayerMgr->GetObjectTypeName(reinterpret_cast<CObject*>(obj));
}

char *GItem::GetName()
{
	return g_pPlayerMgr->GetObjectClassName(reinterpret_cast<CObject*>(obj));
}

char *GItem::GetClearName()
{
	return g_pPlayerMgr->GetObjectName(reinterpret_cast<CObject*>(obj));
}