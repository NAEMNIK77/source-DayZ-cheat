#include "GSDK.h"
#include "GDraw.h"
#include "GESP.h"
#include "GAimbot.h"
#include "GMenu.h"
#include "GPlayerManager.h"
#include "GTools.h"
#include "GOverlay.h"

GPlayerManager *g_pPlayerMgr = new GPlayerManager();


GPlayerManager::GPlayerManager()
{
	base = 0;
	networkmgr = 0;
	transform = 0;
	time = 0;

	memset(vel_enemy, 0, sizeof(vel_enemy));

	ESP = new GESP();
	Aimbot = new GAimbot();
}

void GPlayerManager::Init()
{
	DoPointerSearch();
}

void GPlayerManager::Release()
{
	delete ESP;
	delete Aimbot;
}

CBase *GPlayerManager::GetBase()
{
	if (IsBadToRead((void*)base, 4))
		return NULL;;

	return base;
}

CNetworkManager *GPlayerManager::GetNetworkMgr()
{
	if (IsBadToRead((void*)networkmgr, 4))
		return NULL;;

	return networkmgr;
}

CTransformationData *GPlayerManager::GetTransformation()
{
	if (IsBadToRead((void*)transform, 4))
		return NULL;;

	if (!transform || !transform->transform || IsBadToRead((void*)transform->transform, 4) ||
		!transform->transform->TransformationData || IsBadToRead((void*)transform->transform->TransformationData, 4))
		return NULL;

	return transform->transform->TransformationData;
}

CGameTime *GPlayerManager::GetTime()
{
	if (IsBadToRead((void*)time, 4))
		return NULL;;

	return time;
}

bool GPlayerManager::IsValidWorld()
{
	if (!GetBase())
		return false;

	CWorld *World = GetBase()->World;

	if (!World || IsBadToRead((void*)World, 4))
		return false;

	if (!World->Objects || IsBadToRead((void*)World->Objects, 4))
		return false;

	if (!World->LocalPlayer || IsBadToRead((void*)World->LocalPlayer, 4))
		return false;

	if (!World->LocalPlayer->Object || IsBadToRead((void*)World->LocalPlayer->Object, 4))
		return false;

	if (!IsValid(World->LocalPlayer->Object))
		return false;

	return true;
}

bool GPlayerManager::IsValid(const CObject *obj)
{
	if (!obj || IsBadToRead((void*)obj, 4))
		return false;

	if (!obj->visualstate || IsBadToRead((void*)obj->visualstate, sizeof(CObjectVisualState)))
		return false;

	if (!obj->ObjectClass || IsBadToRead((void*)obj->ObjectClass, sizeof(CObjectClass)))
		return false;

	//if (obj->isdead)
	//return false;

	return true;
}

bool GPlayerManager::IsValid(const CItem *obj)
{
	return IsValid(reinterpret_cast<const CObject*>(obj));
}

CObject *GPlayerManager::GetLocalPlayer()
{
	if (!IsValidWorld())
		return NULL;

	CObject *tmp = GetBase()->World->LocalPlayer->Object;

	if (!IsValid(tmp))
		return NULL;

	return tmp;
}

void GPlayerManager::GetFixedView(GVector &forward, GVector &up, GVector &right)
{
	GVector fov, translation, viewport;
	GetFixedView(forward, up, right, fov, translation, viewport);
}

void GPlayerManager::GetFixedView(GVector &forward, GVector &up, GVector &right, GVector &fov, GVector &translation, GVector &viewport)
{
	static GVector buf_forward, buf_up, buf_right, buf_fov, buf_translation, buf_viewport;

	CTransformationData *data = GetTransformation();
	if (!data)
		return;

	bool isFucked =
		data->right == GVector(1, 0, 0) &&
		data->up == GVector(0, 1, 0) &&
		data->forward == GVector(0, 0, 1);

	bool isFucked2 = data->fov == GVector(0.30, 0.19, 0);

	if (!isFucked && !isFucked2) {

		buf_right = data->right;
		buf_up = data->up;
		buf_forward = data->forward;
		buf_fov = data->fov;
		buf_translation = data->translation;
		buf_viewport = data->viewport;
	}

	forward = buf_forward;
	up = buf_up;
	right = buf_right;
	fov = buf_fov;
	translation = buf_translation;
	viewport = buf_viewport;
}

GVector GPlayerManager::GetViewangles()
{
	GVector forward, up, right;
	GetFixedView(forward, up, right);

	GVector view;
	view.x = asin(forward.y); //pitch x=right
	view.y = atan2f(right.z, right.x); //yaw y=up
	view.z = asin(right.y); //roll z=forward

	return view;
}

GVector GPlayerManager::GetForwardVector(float factor)
{
	GVector view = GetViewangles();

	GVector move;
	move.x = sin(-view.y) * factor;
	move.z = cos(view.y) * factor;
	move.y = sin(view.x) * factor;

	return move;
}

GVector GPlayerManager::GetRightVector(float factor)
{
	GVector view = GetViewangles();
	view.y -= PI_2;

	GVector move;
	move.x = sin(-view.y) * factor;
	move.z = cos(view.y) * factor;
	move.y = sin(view.x) * factor;

	return move;
}

GVector GPlayerManager::GetUpVector(float factor)
{
	GVector view = GetViewangles();
	view.x -= factor;

	GVector move;
	move.x = sin(-view.y) * factor;
	move.z = cos(view.y) * factor;
	move.y = sin(view.x) * factor;

	return move;
}

GVector GPlayerManager::GetPlayerViewangles(const CObject *obj)
{
	if (!IsValid(obj))
		return 0;

	GVector view;
	view.x = obj->pitch[20];
	view.y = atan2f(obj->visualstate->right.x, obj->visualstate->right.z);
	view.z = 0;

	return view;
}

GVector GPlayerManager::GetPlayerForwardVector(const CObject *obj, float factor)
{
	if (!IsValid(obj))
		return 0;

	GVector view = GetPlayerViewangles(obj);

	GVector move;
	move.x = sin(view.y) * factor;
	move.z = cos(view.y) * factor;
	move.y = sin(view.x) * factor;

	return move;
}

GVector GPlayerManager::GetPlayerRightVector(const CObject *obj, float factor)
{
	if (!IsValid(obj))
		return 0;

	GVector view = GetPlayerViewangles(obj);
	view.y -= PI_2;

	GVector move;
	move.x = sin(view.y) * factor;
	move.z = cos(view.y) * factor;
	move.y = sin(view.x) * factor;

	return move;
}

float GPlayerManager::CalcDistance(const CObject *from, const CObject *to)
{
	if (!IsValid(from) || !IsValid(to))
		return 0;

	return CalcDistance(from->visualstate->pos, to->visualstate->pos);
}

float GPlayerManager::CalcDistance(const CObject *to)
{
	CObject *local = GetLocalPlayer();

	if (!local || !IsValid(to))
		return 0;

	return CalcDistance(local, to);
}

float GPlayerManager::CalcDistance(const CItem *to)
{
	return CalcDistance(reinterpret_cast<const CObject*>(to));
}

float GPlayerManager::CalcDistance(GVector from, GVector to)
{
	return from.Dist(to);
}

float GPlayerManager::CalcDistance(GVector to)
{
	CObject *local = GetLocalPlayer();
	if (!local)
		return 0;

	return CalcDistance(local->visualstate->pos, to);
}

bool GPlayerManager::WorldToScreen(const CObject *in, GVector &out)
{
	if (!IsValid(in))
		return false;

	return WorldToScreen(in->visualstate->pos, out);
}

bool GPlayerManager::WorldToScreen(GVector in, GVector &out)
{
	GVector forward, up, right, fov, translation, viewport;
	GetFixedView(forward, up, right, fov, translation, viewport);

	GVector temp = translation - in;

	float x = temp.Dot(right);
	float y = temp.Dot(up);
	float z = temp.Dot(forward);

	out.x = viewport.x * (1 + (x / fov.x / z));
	out.y = viewport.y * (1 - (y / fov.y / z));
	out.z = z;

	return (out.z < 1 && out.x > 0 && out.y > 0);
}

GVector GPlayerManager::GetViewPosition()
{
	GVector forward, up, right, fov, translation, viewport;
	GetFixedView(forward, up, right, fov, translation, viewport);

	return translation;
}

char *GPlayerManager::GetPlayerName(const CObject *obj)
{
	CNetworkManager* pBase = GetNetworkMgr();

	if (!pBase)
		return NULL;

	if (!IsValid(obj))
		return NULL;

	if (!pBase->PlayerInfo || IsBadToRead((void*)pBase->PlayerInfo, 4))
		return NULL;

	if (!pBase->PlayerInfo->PlayerTable || IsBadToRead((void*)pBase->PlayerInfo->PlayerTable, 4))
		return NULL;

	DWORD dwPlayerTable = reinterpret_cast<DWORD>(pBase->PlayerInfo->PlayerTable);
	int dwPlayerCount = pBase->PlayerInfo->iPlayerCount;

	if (dwPlayerCount == 0)
		return NULL;

	for (register int i = 0; i < dwPlayerCount; i++) {

		CPlayerTable* pInfo = reinterpret_cast<CPlayerTable*>(dwPlayerTable + (i * playerinfo_offset));

		if (!pInfo || IsBadToRead((void*)pInfo, 4))
			continue;

		if (pInfo->iPlayerID != obj->playerid)
			continue;

		if (!pInfo->PlayerName || IsBadToRead((void*)pInfo->PlayerName, 4))
			continue;

		return pInfo->PlayerName->szName;
	}

	return NULL;
}

char *GPlayerManager::GetPlayerName(int playerid)
{
	CNetworkManager* pBase = GetNetworkMgr();
	CObject* obj = GetPlayerByPlayerId(playerid);

	if (!pBase)
		return NULL;

	if (!IsValid(obj))
		return NULL;

	if (!pBase->PlayerInfo || IsBadToRead((void*)pBase->PlayerInfo, 4))
		return NULL;

	if (!pBase->PlayerInfo->PlayerTable || IsBadToRead((void*)pBase->PlayerInfo->PlayerTable, 4))
		return NULL;

	DWORD dwPlayerTable = reinterpret_cast<DWORD>(pBase->PlayerInfo->PlayerTable);
	int dwPlayerCount = pBase->PlayerInfo->iPlayerCount;

	if (dwPlayerCount == 0)
		return NULL;

	for (register int i = 0; i < dwPlayerCount; i++) {

		CPlayerTable* pInfo = reinterpret_cast<CPlayerTable*>(dwPlayerTable + (i * playerinfo_offset));

		if (!pInfo)
			continue;

		if (pInfo->iPlayerID != obj->playerid)
			continue;

		if (!pInfo->PlayerName)
			continue;

		return pInfo->PlayerName->szName;
	}

	return NULL;
}

CObject *GPlayerManager::GetPlayerByIndex(int index)
{
	if (!IsValidWorld())
		return NULL;

	if (index > GetMaxObjectIndex())
		return NULL;

	CObjectList *ObjectList = GetBase()->World->Objects->ObjectList;

	if (!ObjectList || IsBadToRead((void*)ObjectList, 4))
		return NULL;

	DWORD dwObject = (DWORD)ObjectList + (index * 0x2C);
	if (IsBadToRead((void*)dwObject, 4))
		return NULL;

	CEntity* ent = (CEntity*)*(DWORD*)dwObject;
	if (!ent || IsBadToRead((void*)ent, 4))
		return NULL;

	if (!IsValid(ent->Object))
		return NULL;

	return ent->Object;
}

CEntity *GPlayerManager::GetPlayerByIndex1(int index)
{
	if (!IsValidWorld())
		return NULL;

	if (index > GetMaxObjectIndex())
		return NULL;

	CObjectList *ObjectList = GetBase()->World->Objects->ObjectList;

	if (!ObjectList || IsBadToRead((void*)ObjectList, 4))
		return NULL;

	DWORD dwObject = (DWORD)ObjectList + (index * 0x2C);
	if (IsBadToRead((void*)dwObject, 4))
		return NULL;

	CEntity* ent = (CEntity*)*(DWORD*)dwObject;
	if (!ent || IsBadToRead((void*)ent, 4))
		return NULL;

	return ent;
}

CObject *GPlayerManager::GetPlayerByPlayerId(int playerid)
{
	if (!GetBase())
		return NULL;

	for (register int i = 0; i < GetMaxObjectIndex(); i++) {

		CObject* obj = GetPlayerByIndex(i);
		if (!obj)
			continue;

		if (obj->playerid != playerid)
			continue;

		return obj;
	}

	return NULL;
}

int GPlayerManager::GetMaxObjectIndex()
{
	if (!IsValidWorld())
		return 0;

	return GetBase()->World->Objects->iObjectSize;
}

bool GPlayerManager::IsZombie(const CObject *obj)
{
	char *name = GetObjectClassName(obj);
	if (!name)
		return false;

	if (strstr(name, /*Zombie*/XorStr<0x7c, 7, 0x4678794a>("\x26\x12\x13\x1d\xe9\xe4" + 0x4678794a).s))
		return true;

	return false;
}

bool GPlayerManager::IsPlayer(const CObject *obj)
{
	char *name = GetObjectClassName(obj);
	if (!name)
		return false;

	if (strstr(name, /*SurvivorParts*/XorStr<0x74, 14, 0x9d6a7d>("\x27\x0\x4\x1\x11\xf\x15\x9\x2c\x1c\xc\xb\xf3" + 0x9d6a7d).s))
		return true;

	return false;
}

bool GPlayerManager::IsAnimal(const CObject *obj)
{
	char *name = GetObjectTypeName(obj);
	if (!name)
		return false;

	if (strstr(name, /*animal*/XorStr<0x58, 7, 0xbaec8723>("\x39\x37\x33\x36\x3d\x31" + 0xbaec8723).s))
		return true;

	return false;
}

bool GPlayerManager::IsFriend(const CObject *obj)
{
	char *name = g_pPlayerMgr->GetPlayerName(obj);

	if (!name || IsBadToRead((void*)name, 4) || strlen(name) == 0)
		return false;

	if (strstr(name, /*ZeaS*/XorStr<0xb8, 5, 0x80eaeb1d>("\xe2\xdc\xdb\xe8" + 0x80eaeb1d).s) ||
		strstr(name, /*NeoIII*/XorStr<0xaf, 7, 0x41117813>("\xe1\xd5\xde\xfb\xfa\xfd" + 0x41117813).s) ||
		strstr(name, /*Silex*/XorStr<0xce, 6, 0x30659732>("\x9d\xa6\xbc\xb4\xaa" + 0x30659732).s) ||
		strstr(name, /*ShadowEternal*/XorStr<0x8d, 14, 0xbef18bc1>("\xde\xe6\xee\xf4\xfe\xe5\xd6\xe0\xf0\xe4\xf9\xf9\xf5" + 0xbef18bc1).s))
		return true;

	return false;
}

CObject *GPlayerManager::GetTableObjectByIndex(char object_type, int index)
{
	if (!IsValidWorld())
		return NULL;

	//CWorld *World = GetBase()->World;

	CObject **obj = 0;

	if (object_type == OBJ_TABLE::ANIMALS)
		obj = GetBase()->World->AnimalTable.obj;
	else if (object_type == OBJ_TABLE::FAR_ANIMALS)
		obj = GetBase()->World->FarAnimalTable.obj;
	else if (object_type == OBJ_TABLE::FARFAR_ANIMALS)
		obj = GetBase()->World->FarFarAnimalTable.obj;
	else if (object_type == OBJ_TABLE::SMOKE)
		obj = GetBase()->World->SmokeEffectsTable.obj;
	else if (object_type == OBJ_TABLE::EFFECTS)
		obj = GetBase()->World->EffectsTable.obj;
	else if (object_type == OBJ_TABLE::FAR_EFFECTS)
		obj = GetBase()->World->FarEffectsTable.obj;
	else if (object_type == OBJ_TABLE::DROPPED_ITEM)
		obj = GetBase()->World->DroppedItemTable.obj;
	else if (object_type == OBJ_TABLE::FAR_DROPPED_ITEM)
		obj = GetBase()->World->FarDroppedItemTable.obj;
	else if (object_type == OBJ_TABLE::ITEM)
		obj = GetBase()->World->ItemTable.obj;
	else if (object_type == OBJ_TABLE::FAR_BUILDING)
		obj = GetBase()->World->FarBuildingTable.obj;
	else if (object_type == OBJ_TABLE::INVALID_ITEM)
		obj = GetBase()->World->InvalidItemTable.obj;
	else
		return NULL;

	if (index > GetMaxTableObjectIndex(object_type))
		return NULL;

	if (!obj || IsBadToRead((void*)obj, 4))
		return NULL;

	if (IsBadToRead((void*)((DWORD)obj + index * 4), 4))
		return NULL;

	if (!obj[index] || IsBadToRead((void*)obj[index], 4))
		return NULL;

	if (!IsValid(obj[index]))
		return NULL;

	return obj[index];
}
//E68
int GPlayerManager::GetMaxTableObjectIndex(char object_type)
{
	if (!IsValidWorld())
		return 0;

	int max_index = 0;

	if (object_type == OBJ_TABLE::ANIMALS)
		max_index = GetBase()->World->AnimalTable.size;
	else if (object_type == OBJ_TABLE::FAR_ANIMALS)
		max_index = GetBase()->World->FarAnimalTable.size;
	else if (object_type == OBJ_TABLE::FARFAR_ANIMALS)
		max_index = GetBase()->World->FarFarAnimalTable.size;
	else if (object_type == OBJ_TABLE::SMOKE)
		max_index = GetBase()->World->SmokeEffectsTable.size;
	else if (object_type == OBJ_TABLE::EFFECTS)
		max_index = GetBase()->World->EffectsTable.size;
	else if (object_type == OBJ_TABLE::FAR_EFFECTS)
		max_index = GetBase()->World->FarEffectsTable.size;
	else if (object_type == OBJ_TABLE::DROPPED_ITEM)
		max_index = GetBase()->World->DroppedItemTable.size;
	else if (object_type == OBJ_TABLE::FAR_DROPPED_ITEM)
		max_index = GetBase()->World->FarDroppedItemTable.size;
	else if (object_type == OBJ_TABLE::ITEM)
		max_index = GetBase()->World->ItemTable.size;
	else if (object_type == OBJ_TABLE::FAR_BUILDING)
		max_index = GetBase()->World->FarBuildingTable.size;
	else if (object_type == OBJ_TABLE::INVALID_ITEM)
		max_index = GetBase()->World->InvalidItemTable.size;

	return max_index;
}

bool GPlayerManager::InCenterArea(GVector screen)
{
	int w, h;
	g_pOverlay->DirectX->Draw->GetViewport(w, h);

	int center_x = w / 2;
	int center_y = h / 2;
	int padding = 25;

	if (screen.x > (center_x - padding) &&
		screen.x < (center_x + padding) &&
		screen.y >(center_y - padding) &&
		screen.y < (center_y + padding)
		)
		return true;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_pOverlay->GetAppWindow(), &pt);

	center_x = pt.x;
	center_y = pt.y;

	padding = 10;

	if (screen.x >(center_x - padding) &&
		screen.x < (center_x + padding) &&
		screen.y >(center_y - padding) &&
		screen.y < (center_y + padding)
		)
		return true;

	return false;
}

bool GPlayerManager::IsConnected()
{
	if (!GetBase())
		return false;

	if (!GetLocalPlayer())
		return false;

	return GetBase()->World->LocalPlayer->connected > 18;
}

void GPlayerManager::DoPointerSearch()
{
	DWORD start = g_pTools->GetModuleAddress();
	DWORD size = g_pTools->GetModuleSize();

	BYTE base_pattern[] = "\x8B\x0D\x00\x00\x00\x00\x56\x81\xC1\x10\x0D\x00\x00";
	char base_mask[32] = { 0 }; strcpy(base_mask, /*xx????xxxxxx*/XorStr<0xbb, 13, 0x1d831eeb>("\xc3\xc4\x82\x81\x80\xff\xb9\xba\xbb\xbc\xbd\xbe" + 0x1d831eeb).s);
	long base_fix = 0x02;

	//004E98D0    B8 50510701     mov eax, DayZ_0_4.01075150               ; b8????????C3CCCCCCCCCCCC

	//0050A0ED    68 40ACE400     push DayZ_0_4.00E4AC40                   ; ASCII "Unknown player"
	//0050A1D5    68 50ACE400     push DayZ_0_4.00E4AC50                   ; ASCII "%d.%d.%d.%d:%d"
	//0050A389    68 6810E200     push DayZ_0_4.00E21068                   ; ASCII "name"
	//0050A3DD    68 BC9AE400     push DayZ_0_4.00E49ABC                   ; ASCII "content"

	BYTE networkmgr_pattern[] = "\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\xC6\x5E";
	char networkmgr_mask[32] = { 0 }; strcpy(networkmgr_mask, /*x????x????xxx*/XorStr<0xf7, 14, 0x8e8e275c>("\x8f\xc7\xc6\xc5\xc4\x84\xc2\xc1\xc0\x3f\x79\x7a\x7b" + 0x8e8e275c).s);
	long networkmgr_fix = 0x01;

	BYTE transform_pattern[] = "\x8B\x0D\x00\x00\x00\x00\x8B\x81\x94";
	char transform_mask[32] = { 0 }; strcpy(transform_mask, /*xx????xxx*/XorStr<0xed, 10, 0x1d4feab7>("\x95\x96\xd0\xcf\xce\xcd\x8b\x8c\x8d" + 0x1d4feab7).s);
	long transform_fix = 0x02;

	BYTE time_pattern[] = "\xDB\x05\x00\x00\x00\x00\x83\xEC";
	char time_mask[32] = { 0 }; strcpy(time_mask, /*xx????xx*/XorStr<0xbb, 9, 0xebed1f20>("\xc3\xc4\x82\x81\x80\xff\xb9\xba" + 0xebed1f20).s);
	long time_fix = 0x02;

	BYTE d3ddevice_pattern[] = "\x8B\x0D\x00\x00\x00\x00\x8B\x11\x8B\x82";
	char d3ddevice_mask[32] = { 0 }; strcpy(d3ddevice_mask, /*xx????xxxx*/XorStr<0x6a, 11, 0x9a06768>("\x12\x13\x53\x52\x51\x50\x8\x9\xa\xb" + 0x9a06768).s);
	long d3ddevice_fix = 0x02;

	BYTE d3ddevice_offset_pattern[] = "\x8D\x8A\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x47";
	char d3ddevice_offset_mask[32] = { 0 }; strcpy(d3ddevice_offset_mask, /*xx????x????xx*/XorStr<0xba, 14, 0x50515221>("\xc2\xc3\x83\x82\x81\x80\xb8\xfe\xfd\xfc\xfb\xbd\xbe" + 0x50515221).s);
	long d3ddevice_offset_fix = 0x02;


	DWORD search_base = g_pTools->FindPattern(start, size, base_pattern, base_mask);
	DWORD search_networkmgr = g_pTools->FindPattern(start, size, networkmgr_pattern, networkmgr_mask);
	DWORD search_transform = g_pTools->FindPattern(start, size, transform_pattern, transform_mask);
	DWORD search_time = g_pTools->FindPattern(start, size, time_pattern, time_mask);
	DWORD search_d3ddevice = g_pTools->FindPattern(start, size, d3ddevice_pattern, d3ddevice_mask);
	DWORD search_d3ddevice_offset = g_pTools->FindPattern(start, size, d3ddevice_offset_pattern, d3ddevice_offset_mask);

	if (search_base) {

		search_base += base_fix;
		base = reinterpret_cast<CBase*>(*(DWORD*)search_base);
	}

	if (search_networkmgr) {

		search_networkmgr += networkmgr_fix;

		/*__asm {
			call search_playerinfo
			mov search_playerinfo, eax
		}*/
		search_networkmgr = *(DWORD*)search_networkmgr;

		networkmgr = reinterpret_cast<CNetworkManager*>(search_networkmgr);
	}

	if (search_transform) {

		search_transform += transform_fix;
		transform = reinterpret_cast<CTransformation*>(*(DWORD*)search_transform);
	}

	if (search_time) {

		search_time += time_fix;
		time = reinterpret_cast<CGameTime*>(*(DWORD*)search_time);
	}

	if (search_d3ddevice) {

		search_d3ddevice += d3ddevice_fix;
		g_pOverlay->DirectX->Draw->dwD3D9Address = *reinterpret_cast<DWORD*>(search_d3ddevice);
	}

	if (search_d3ddevice_offset) {

		search_d3ddevice_offset += d3ddevice_offset_fix;
		g_pOverlay->DirectX->Draw->dwD3D9Offset = *reinterpret_cast<DWORD*>(search_d3ddevice_offset);
	}

	/*g_pTools->Log("Base: %08X", base);
	g_pTools->Log("NetworkManager: %08X", networkmgr);
	g_pTools->Log("Transformations: %08X", transform);
	g_pTools->Log("Worldtime: %08X", time);
	g_pTools->Log("D3D9 Device + Offset: %08X + %08X", g_pOverlay->DirectX->Draw->dwD3D9Address, g_pOverlay->DirectX->Draw->dwD3D9Offset);*/


	/*
	Base:

	00418AC1    8B0D 301EE500   mov ecx, dword ptr ds:[E51E30]
	00418AC7    56              push esi
	00418AC8    81C1 0C0D0000   add ecx, 0D0C
	00418ACE    E8 6DF23200     call DayZ.00747D40
	00418AD3    8B75 10         mov esi, dword ptr ss:[ebp+10]

	004CD902    68 9C74E800     push DayZ.00E8749C                       ; ASCII "ChernarusPlus"
	004CD907    50              push eax
	004CD908    E8 637E3400     call DayZ.00815770
	004CD90D    59              pop ecx
	004CD90E    59              pop ecx
	004CD90F    85C0            test eax, eax
	004CD911  ^ 75 D9           jnz short DayZ.004CD8EC
	004CD913    8B0D 301EE500   mov ecx, dword ptr ds:[E51E30]

	8B 0D ?? ?? ?? ?? 56 81 C1 0C 0D 00 00 +2
	*/

	/*
	Playerinfo:

	004F6447    B8 3046E400     mov eax, DayZ.00E44630
	004F644C    C3              retn
	004F644D >  D9EE            fldz
	004F644F    C2 0800         retn 8
	004F6452    6A 64           push 64
	004F6454    58              pop eax
	004F6455    C3              retn


	B8 ?? ?? ?? ?? C3 D9 EE C2 08 00 <-- call, oder +1
	*/

	/*
	Transformations:

	00418BAC    8B0D 442FEA00   mov ecx, dword ptr ds:[EA2F44]
	00418BB2    8B81 94000000   mov eax, dword ptr ds:[ecx+94]
	00418BB8    F3:0F1040 30    movss xmm0, dword ptr ds:[eax+30]
	00418BBD    F3:0F5C47 08    subss xmm0, dword ptr ds:[edi+8]

	00A00334    8B0D 442FEA00   mov ecx, dword ptr ds:[EA2F44]
	00A0033A    8B81 94000000   mov eax, dword ptr ds:[ecx+94]
	00A00340    F3:0F1040 28    movss xmm0, dword ptr ds:[eax+28]
	00A00345    F3:0F1048 2C    movss xmm1, dword ptr ds:[eax+2C]
	00A0034A    F3:0F1050 30    movss xmm2, dword ptr ds:[eax+30]
	00A0034F    6A 01           push 1
	00A00351    6A 01           push 1

	8B 0D ?? ?? ?? ?? 8B 81 94 +2


	GameTime:

	004F5916    DB05 D070E800   fild dword ptr ds:[E870D0]
	004F591C    51              push ecx
	004F591D    51              push ecx
	004F591E    D80D E0C5D300   fmul dword ptr ds:[D3C5E0]
	004F5924    DD1C24          fstp qword ptr ss:[esp]
	004F5927    68 70BCC100     push DayZ.00C1BC70                       ; ASCII "Network simulation, time = %.3f"
	004F592C    E8 1CE14D00     call DayZ.009D3A4D

	DB 05 ?? ?? ?? ?? 51 51
	*/

	//DWORD search_test1 = g_pTools->FindPattern(start, size, (BYTE*)"\xF3\x0F\x10\x41\x34\xF3\x0F\x11", /*xxxxxxxx*/XorStr<0x98, 9, 0x94602dfc>("\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7" + 0x94602dfc).s);
	//if (search_test1) {
	//	search_test1 -= 0x17;
	//	g_pTools->Log("World::Localplayer Offset: %08X", *(DWORD*)(search_test1 + 0x8));
	//	g_pTools->Log("CEntity::object Offset: %08X", *(BYTE*)(search_test1 + 0xE));
	//}

	//DWORD search_test2 = g_pTools->FindPattern(start, size, (BYTE*)"\x8D\x86\x00\x00\x00\x00\x8B\x48\x00\x8B\x11\x50\xFF\x12\x8B", /*xx????xx?xxxxxx*/XorStr<0xad, 16, 0xda10aa46>("\xd5\xd6\x90\x8f\x8e\x8d\xcb\xcc\x8a\xce\xcf\xc0\xc1\xc2\xc3" + 0xda10aa46).s);
	//if (search_test2) {
	//	g_pTools->Log("World::ItemTable Offset: %08X", *(DWORD*)(search_test2 + 0x2));
	//	g_pTools->Log("CObject::ObjectVisualState Offset: %08X", *(BYTE*)(search_test2 + 0x8));
	//}

	//DWORD search_test3 = g_pTools->FindPattern(start, size, (BYTE*)"\x8B\x8E\x00\x00\x00\x00\x57\xE8\x00\x00\x00\x00\x5F\x5E", /*xx????xx????xx*/XorStr<0xf8, 15, 0x278d5bf7>("\x80\x81\xc5\xc4\xc3\xc2\x86\x87\x3f\x3e\x3d\x3c\x7c\x7d" + 0x278d5bf7).s);
	//if (search_test3) {
	//	g_pTools->Log("World::objects: %08X", *(DWORD*)(search_test3 + 0x2));
	//}

	//DWORD search_test4 = g_pTools->FindPattern(start, size, (BYTE*)"\x35\xB6\xC8\xBA\xBA", /*xxxxx*/XorStr<0xbd, 6, 0xb889efbc>("\xc5\xc6\xc7\xb8\xb9" + 0xb889efbc).s);
	//if (search_test4) {
	//	g_pTools->Log("CWeapon::magazine::ammo_count1: %08X", *(DWORD*)(search_test4 + 0xD));
	//	g_pTools->Log("CWeapon::magazine::ammo_count1: %08X", *(DWORD*)(search_test4 + 0x13));
	//	g_pTools->Log("CWeapon::magazine: %08X", *(DWORD*)(search_test4 + 0x88));
	//}
	/*
	8B 15 ?? ?? ?? ?? 8B 82 ?? ?? ?? ?? 3B 78 ?? 8B 84 24
	CBase: + 2
	CWorld::localplayeroffset: +8
	CEntity::object offset: +E

	8D 86 ?? ?? ?? ?? 8B 48 ?? 8B 11 50 FF 12 8B
	CWorld::neartable offset: +2
	CObject::movement: +8

	8B 8E ?? ?? ?? ?? 57 E8 ?? ?? ?? ?? 5F 5E
	CWorld::objects: +2

	35 B6 C8 BA BA
	CWeapon::ammo_count1: +D
	CWeapon::ammo_count2: +0x13
	CWeapon::magazine: +0x88
	*/
}

char *GPlayerManager::GetPlayerWeaponName(const CObject *obj)
{
	if (!IsValid(obj))
		return NULL;

	CWeaponPtr *ent = obj->weapon;
	if (!ent || IsBadToRead((void*)ent, 4))
		return NULL;

	CWeapon *weapon = ent->object;
	if (!IsValid(reinterpret_cast<CObject*>(weapon)))
		return NULL;

	return GetObjectName(reinterpret_cast<CObject*>(weapon));
}

void GPlayerManager::GetPlayerHealthBlood(const CObject *obj, player_info &info)
{
	DWORD dwGameVariables = (DWORD)obj->GameVariables;

	if (!dwGameVariables || IsBadToRead((void*)dwGameVariables, 4))
		return;

	for (int n = 0; n < obj->GameVariables_Count; n++) {

		DWORD firstArray = *(DWORD*)(dwGameVariables + 0x0C * n);
		if (!firstArray || IsBadToRead((void*)firstArray, 0x4))
			continue;

		if (!obj->GameVariables->var || IsBadToRead((void*)obj->GameVariables->var, 4))
			continue;

		for (int k = 0; k < obj->GameVariables->var[n].count; k++) {

			CArmaString* name = (CArmaString*)*(DWORD*)(firstArray + k * 0x14 + 0x4);
			if (!name || IsBadToRead((void*)name, 0x4))
				continue;

			DWORD arrayEntity = *(DWORD*)(firstArray + k * 0x14 + 0xC);
			if (!arrayEntity || IsBadToRead((void*)arrayEntity, 0x4))
				continue;

			float *val = (float*)(arrayEntity + 0xC);
			if (!val || IsBadToRead((void*)val, 4))
				continue;

			if (strstr(name->szName, /*health*/XorStr<0xca, 7, 0x9162942f>("\xa2\xae\xad\xa1\xba\xa7" + 0x9162942f).s) != 0)
				info.health = *val;
			else if (strstr(name->szName, /*blood*/XorStr<0xbc, 6, 0xe984ebbb>("\xde\xd1\xd1\xd0\xa4" + 0xe984ebbb).s) != 0)
				info.blood = *val;
			else if (strstr(name->szName, /*morale*/XorStr<0xe8, 7, 0xe3b016b2>("\x85\x86\x98\x8a\x80\x88" + 0xe3b016b2).s) != 0)
				info.morale = *val;
			else if (strstr(name->szName, /*shock*/XorStr<0x13, 6, 0xa641a878>("\x60\x7c\x7a\x75\x7c" + 0xa641a878).s) != 0)
				info.shock = *val;
			else if (strstr(name->szName, /*bleedingeffects*/XorStr<0xe9, 16, 0xb14be77f>("\x8b\x86\x8e\x89\x89\x87\x81\x97\x94\x94\x95\x91\x96\x82\x84" + 0xb14be77f).s) != 0)
				info.bleedingeffects = *val;
			else if (strstr(name->szName, /*bleedingsources*/XorStr<0x41, 16, 0xd6d870a6>("\x23\x2e\x26\x21\x21\x2f\x29\x2f\x3a\x25\x3e\x3e\x2e\x2b\x3c" + 0xd6d870a6).s) != 0)
				info.bleedingsources = *val;
			else if (strstr(name->szName, /*unconscious*/XorStr<0xb, 12, 0x7713ea4>("\x7e\x62\x6e\x61\x61\x63\x72\x7b\x7c\x61\x66" + 0x7713ea4).s) != 0)
				info.unconscious = *val;
		}
	}
}

void GPlayerManager::SetViewangles(const GVector angles)
{
	CObject *pLocal = g_pPlayerMgr->GetLocalPlayer();
	if (!pLocal)
		return;

	float x = sin(-angles.y);
	float z = cos(angles.y);

	GVector v1(x, 0, z);
	GVector v2(z, 0, -x);
	GVector v3(z, 0, x);
	GVector v4(-x, 0, z);

	pLocal->visualstate->right = v1;
	pLocal->visualstate->forward = v2;
	pLocal->visualstate->forward_ = v3;
	pLocal->visualstate->right_ = v4;

	//CTransformationData *data = GetTransformation();
	//if (data && !isnan(v1.x) && !isnan(v2.x) && !isnan(v1.z) && !isnan(v2.z)) {
	//	data->right.x = v2.x;
	//	data->right.z = v2.z;

	//	data->forward.x = v1.x;
	//	data->forward.z = v1.z;

	//	data->forward.y = angles.x;
	//}
	//2 3 6 7 20 21 28 29
	for (auto i : { 2, 3, 6, 7, 20, 21, 28, 29 })
		pLocal->pitch[i] = angles.x;
}

void GPlayerManager::AimToPoint(const GVector pos)
{
	CObject *pLocal = g_pPlayerMgr->GetLocalPlayer();
	if (!pLocal)
		return;

	GVector angles = GMath::CalcAngle(pos, pLocal->visualstate->head_pos);
	SetViewangles(angles);
}

void GPlayerManager::DrawHealthbars()
{
	CObject *obj = GetLocalPlayer();
	if (!obj)
		return;

	if (!IsConnected())
		return;

	player_info info = { 0 };
	GetPlayerHealthBlood(obj, info);

	float percentage_width = float(cvars.Screen.x - 20) / 5000;

	g_pOverlay->DirectX->Draw->FilledQuad(10, cvars.Screen.y - 9, cvars.Screen.x - 20, 9, color_black);
	g_pOverlay->DirectX->Draw->Quad(10, cvars.Screen.y - 9, cvars.Screen.x - 20, 9, 1, color_white);
	g_pOverlay->DirectX->Draw->FilledQuad(12, cvars.Screen.y - 7, (info.health*percentage_width) - 3, 2, color_green);
	g_pOverlay->DirectX->Draw->FilledQuad(12, cvars.Screen.y - 4, (info.blood*percentage_width) - 3, 2, color_green);
}

void GPlayerManager::DrawRadar()
{
	if (!IsConnected())
		return;

	int w, h;
	g_pOverlay->DirectX->Draw->GetViewport(w, h);

	g_pOverlay->DirectX->Draw->FilledQuad((w - cvars.radar_x), cvars.radar_y, 82 * 2 + 1, 82 * 2 + 1, GColor(0, 0, 0, 180));
	g_pOverlay->DirectX->Draw->Quad((w - cvars.radar_x), cvars.radar_y, 82 * 2 + 1, 82 * 2 + 1, 2, color_white);
	g_pOverlay->DirectX->Draw->FilledQuad((w - cvars.radar_x), cvars.radar_y + (82) + 1, 82 * 2 + 1, 1, color_green);
	g_pOverlay->DirectX->Draw->FilledQuad((w - cvars.radar_x) + (82), cvars.radar_y, 1, 82 * 2 + 1, color_green);
}

void GPlayerManager::DrawStatsbox()
{
	if (cvars.ESP_Radar && IsConnected()) {

		cvars.statsbox_x = 200;
		cvars.statsbox_y = 230;
	}
	else {

		cvars.statsbox_x = 200;
		cvars.statsbox_y = 50;
	}

	int w, h;
	g_pOverlay->DirectX->Draw->GetViewport(w, h);

	int statsbox_x = w - cvars.statsbox_x;
	int statsbox_y = cvars.statsbox_y;

	time_t rawtime;
	struct tm * timeinfo;
	::time(&rawtime);
	timeinfo = localtime(&rawtime);


	g_pOverlay->DirectX->Draw->FilledQuad(statsbox_x, statsbox_y, 164, 70, GColor(0, 0, 0, 180));
	g_pOverlay->DirectX->Draw->Quad(statsbox_x, statsbox_y, 164, 70, 2, color_white);
	g_pOverlay->DirectX->Draw->Quad(statsbox_x, statsbox_y + 20, 164, 1, 1, color_white);
	g_pOverlay->DirectX->Draw->TextCentered(statsbox_x + 82, statsbox_y + 5, color_green, /*Statsbox (%.0f FPS)*/XorStr<0x72, 20, 0x87D573A7>("\x21\x07\x15\x01\x05\x15\x17\x01\x5A\x53\x59\x53\x4E\x19\xA0\xC7\xD2\xD0\xAD" + 0x87D573A7).s, g_pTools->GetFramerate());

	g_pOverlay->DirectX->Draw->Text(statsbox_x + 10, statsbox_y + 25, color_white, /*Time: %02d:%02d:%02d*/XorStr<0xB2, 21, 0x45DCF52B>("\xE6\xDA\xD9\xD0\x8C\x97\x9D\x89\x88\xDF\x86\x98\x8E\x8D\xA4\xFB\xE7\xF3\xF6\xA1" + 0x45DCF52B).s, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	g_pOverlay->DirectX->Draw->Text(statsbox_x + 10, statsbox_y + 38, color_white, /*P/Z/I Count: %d/%d/%d*/XorStr<0x34, 22, 0x44E4F697>("\x64\x1A\x6C\x18\x71\x19\x79\x54\x49\x53\x4A\x05\x60\x64\x26\x6C\x61\x21\x69\x62\x2C" + 0x44E4F697).s, ESP->count_players, ESP->count_zombies, ESP->count_items);
	g_pOverlay->DirectX->Draw->Text(statsbox_x + 10, statsbox_y + 51, color_white, /*Speed: %.2f km/h*/XorStr<0x4e, 17, 0xe4194b4d>("\x1d\x3f\x35\x34\x36\x69\x74\x70\x78\x65\x3e\x79\x31\x36\x73\x35" + 0xe4194b4d).s, ESP->cur_speed*3.6);
}

void GPlayerManager::FlyToView(GVector &pos)
{
	float fps = g_pTools->GetFramerate();

	float Speed = (static_cast<float>(cvars.MISC_Glitcher) / 100) * (60 / fps ? fps : 1);
	pos += GetForwardVector(Speed);
}

void GPlayerManager::RefillAmmo()
{
	CObject *pLocal = g_pPlayerMgr->GetLocalPlayer();
	if (!pLocal)
		return;

	if (!pLocal->weapon || IsBadToRead((void*)pLocal->weapon, 4))
		return;

	if (!pLocal->weapon->object || IsBadToRead((void*)pLocal->weapon->object, 4))
		return;

	GItem item((CObject*)pLocal->weapon->object);
	if (!item.Valid())
		return;

	if (item.GetType() != GItem::ITEM_TYPE::T_WEAPON)
		return;

	if (!pLocal->weapon->object->magazine || IsBadToRead((void*)pLocal->weapon->object->magazine, 4))
		return;

	CWeapon* obj = pLocal->weapon->object->magazine->object;
	if (!IsValid(reinterpret_cast<CObject*>(obj)))
		return;

	int curAmmo = ((obj->ammo_count1 + obj->ammo_count2) ^ 0xBABAC8B6);

	CObjectClass* pClass = obj->ObjectClass;
	if (!pClass || IsBadReadPtr((void*)pClass, 4))
		return;

	int maxAmmo = pClass->max_ammo;
	if (curAmmo < maxAmmo) {

		DWORD dwTmp = (maxAmmo ^ 0xBABAC8B6);
		maxAmmo = dwTmp << 1;
		obj->ammo_count1 = (dwTmp - maxAmmo);
		obj->ammo_count2 = (maxAmmo);
	}
}

int GPlayerManager::GetMagazineAmmo(CObject *mag, int *maxammo)
{
	if (!IsValid(mag))
		return 0;

	GItem item(mag);
	if (!item.Valid())
		return 0;

	CWeapon *obj = reinterpret_cast<CWeapon*>(mag);

	int curammo = (obj->ammo_count1 + obj->ammo_count2) ^ 0xBABAC8B6;

	if (maxammo && obj->ObjectClass)
		*maxammo = obj->ObjectClass->max_ammo;

	return curammo;
}

void GPlayerManager::SetMagazineAmmo(CObject *mag)
{
	if (!IsValid(mag))
		return;

	GItem item(mag);
	if (!item.Valid())
		return;

	CWeapon *obj = reinterpret_cast<CWeapon*>(mag);

	int curammo = (obj->ammo_count1 + obj->ammo_count2) ^ 0xBABAC8B6;
	obj->ammo_test1 = 0xEEEEEEEE;
	obj->ammo_test2 = 0xEEEEEEEE;
	obj->ammo_count1 = 0xEEEEEEEE;
	obj->ammo_count2 = 0xEEEEEEEE;
	//obj->test1 = 1;
	//obj->test2 = 1;
}

void GPlayerManager::CalcVelocity(int i)
{
	CObject *obj = GetPlayerByIndex(i);
	if (!obj)
		return;

	float k = vel_enemy[i].timer.get_ms();

	if (vel_enemy[i].pos == GVector(0, 0, 0) || vel_enemy[i].old_pos == GVector(0, 0, 0)) {
		vel_enemy[i].pos = obj->visualstate->pos;
		vel_enemy[i].old_pos = obj->visualstate->pos;

		return;
	}

	if (vel_enemy[i].nextFrame && k >= 1000) {
		if (vel_enemy[i].old_pos == GVector(0, 0, 0))
			vel_enemy[i].old_pos = obj->visualstate->pos;
		vel_enemy[i].pos = obj->visualstate->pos;
		vel_enemy[i].speed = g_pPlayerMgr->CalcDistance(vel_enemy[i].old_pos, vel_enemy[i].pos) / (k / 1000);
		vel_enemy[i].nextFrame = false;
	}

	if (!vel_enemy[i].nextFrame) {
		vel_enemy[i].old_pos = obj->visualstate->pos;
		vel_enemy[i].timer.start();
		vel_enemy[i].nextFrame = true;
	}
}

float GPlayerManager::GetVelocity(int i)
{
	return vel_enemy[i].speed;
}

void GPlayerManager::RemoveFiringSpeed()
{
	CObject *pLocal = g_pPlayerMgr->GetLocalPlayer();
	if (!pLocal)
		return;

	if (!pLocal->weapon || IsBadToRead((void*)pLocal->weapon, 4))
		return;

	if (!IsValid(reinterpret_cast<CObject*>(pLocal->weapon->object)))
		return;

//	pLocal->weapon->object->test1 = 1;

	GItem item(pLocal->weapon->object);
	if (!item.Valid())
		return;

	if (item.GetType() != GItem::ITEM_TYPE::T_WEAPON)
		return;

	if (!pLocal->weapon->object->magazine)
		return;

	CWeapon* obj = pLocal->weapon->object->magazine->object;
	if (!obj || !IsValid(reinterpret_cast<CObject*>(obj)))
		return;

	obj->ammo_test2 = 0xFFFFFFFF;
}

//void __thiscall Object__IntersectLine(Object *this, 
//ObjectVisualState *pos, CollisionBuffer *result, 
//Vector3P *beg, Vector3P *end, float radius, ObjIntersect type,
//int hierLevel, unsigned int collisionFlags)

//007F6EA0    8B41 24         mov eax, dword ptr ds : [ecx + 24]

//typedef void(__stdcall* IntersectLine_t)(CObjectVisualState* pos, BYTE* result, GVector *beg, GVector *end,
//	float radius, int type, int hierLevel, DWORD collisionsFlags);
//
//IntersectLine_t pIntersectLine = (IntersectLine_t)0x007F6EA0;

bool GPlayerManager::IsVisible(CObject *obj, GVector start, GVector end)
{
	typedef void(__stdcall *IntersectLine_t)(CObject *obj, float age, void *result, GVector begin, GVector end, float radius, int type, int collisionFlags);
	IntersectLine_t pIntersectLine = (IntersectLine_t)0x007DF958;

	BYTE objCol[1000] = { 0 };

	__asm pushad
	__asm mov ecx, obj

	pIntersectLine(obj, 0, objCol, start, end, 0, 1, 0);

	__asm popad

	return false;
}

char *GPlayerManager::GetObjectName(const CObject *obj)
{
	if (!IsValid(obj))
		return NULL;

	if (!obj->ObjectClass || IsBadToRead((void*)obj->ObjectClass, 4))
		return NULL;

	if (!obj->ObjectClass->CleanObjectName2 || IsBadToRead((void*)obj->ObjectClass->CleanObjectName2, 4))
		return NULL;

	if (obj->ObjectClass->CleanObjectName2->iNameLength == 0)
		return NULL;

	return obj->ObjectClass->CleanObjectName2->szName;
}

char *GPlayerManager::GetObjectClassName(const CObject *obj)
{
	if (!IsValid(obj))
		return NULL;

	if (!obj->ObjectClass || IsBadToRead((void*)obj->ObjectClass, 4))
		return NULL;

	if (!obj->ObjectClass->ObjectClassName || IsBadToRead((void*)obj->ObjectClass->ObjectClassName, 4))
		return NULL;

	if (obj->ObjectClass->ObjectClassName->iNameLength == 0)
		return NULL;

	return obj->ObjectClass->ObjectClassName->szName;
}

char *GPlayerManager::GetObjectTypeName(const CObject *obj)
{
	if (!IsValid(obj))
		return NULL;

	if (!obj->ObjectClass || IsBadToRead((void*)obj->ObjectClass, 4))
		return NULL;

	if (!obj->ObjectClass->ObjectName || IsBadToRead((void*)obj->ObjectClass->ObjectName, 4))
		return NULL;

	if (obj->ObjectClass->ObjectName->iNameLength == 0)
		return NULL;

	return obj->ObjectClass->ObjectName->szName;
}

void GPlayerManager::SetWorldTime(const int no)
{
	if (!IsConnected())
		return;

	CGameTime *t = GetTime();

	if (!t || no == 0)
		return;

	t->time = no + 10;
	t->time2 = 0;
}

void GPlayerManager::DoTeleport()
{
	CObject *local = GetLocalPlayer();
	if (!local)
		return;

	list_teleport.unique();

	int max_count = list_teleport.size();
	float add_angle = (D3DX_PI * 2) / max_count ? max_count : 1;
	float radius = 1.4;

	int i = 0;

	for (auto &it : list_teleport) {

		float theta = i++ * add_angle;

		GVector kreis = local->visualstate->pos;
		kreis.x += cos(theta) * radius;
		kreis.y += 0.2;
		kreis.z += sin(theta) * radius;

		it->visualstate->pos = kreis;
	}
}

void GPlayerManager::AddTeleportList(const CItem *item)
{
	list_teleport.push_back(item);
}

void GPlayerManager::ClearTeleportList()
{
	list_teleport.clear();
}

bool GPlayerManager::EnemyAimingAtYou(const CObject* obj)
{
	if (obj->isDead)
		return false;

	if (!GetLocalPlayer())
		return false;

	GVector pos = obj->visualstate->head_pos;
	GVector my_pos = GetLocalPlayer()->visualstate->head_pos;
	float dist = CalcDistance(my_pos, pos);

	if (dist > cvars.radar_range)
		return false;

	pos += GetPlayerForwardVector(obj, dist);
	dist = CalcDistance(my_pos, pos);

	return dist < 0.3 ? true : false;
}

std::list<CItem*> GPlayerManager::GetItemContainerContent(const CItem *obj)
{
	std::list<CItem*>buf;

	CItemContainer *container = obj->ItemContainer;

	if (!container)
		return buf;

	for (register int i = 0; i < container->items.size; i++) {

		CItem *subitem = reinterpret_cast<CItem*>(container->items.obj[i]);
		if (!IsValid(subitem))
			continue;

		buf.push_back(subitem);
	}

	return buf;
}

bool GPlayerManager::IsInItemContainer(const CItem *item, const CItem *obj)
{
	CItemContainer *container = item->ItemContainer;

	if (!container || !item)
		return false;

	for (register int i = 0; i < container->items.size; i++) {

		CItem *subitem = reinterpret_cast<CItem*>(container->items.obj[i]);
		if (!IsValid(subitem))
			continue;

		if (subitem == obj)
			return true;
	}

	return false;
}

void GPlayerManager::RemoveFromItemContainer(const CItem *item, const CItem *remove)
{
	CItemContainer *container = item->ItemContainer;

	if (!container || !item)
		return;

	for (register int i = 0; i < container->items.size; i++) {

		CItem *subitem = reinterpret_cast<CItem*>(container->items.obj[i]);
		if (!IsValid(subitem))
			continue;

		if (subitem == remove) {

			container->items.obj[i] = NULL;
			container->items.size--;
			break;
		}
	}
}

std::list<CItem*> GPlayerManager::GetWeaponSlots(const CWeapon *obj)
{
	std::list<CItem*>buf;

	if (!obj->slots)
		return buf;

	for (register int i = 0; i < obj->num_slots; i++) {

		CItem *cur_slot = obj->slots[i];
		if (!cur_slot)
			continue;

		buf.push_back(cur_slot);
	}

	return buf;
}

bool GPlayerManager::FindItem(const CItem *item)
{
	for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::ITEM); i++) {

		if (reinterpret_cast<const CObject*>(item) == g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::ITEM, i)) 
			return true;
	}

	for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::DROPPED_ITEM); i++) {

		if (reinterpret_cast<const CObject*>(item) == g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::DROPPED_ITEM, i))
			return true;
	}

	for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::FAR_DROPPED_ITEM); i++) {

		if (reinterpret_cast<const CObject*>(item) == g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::FAR_DROPPED_ITEM, i))
			return true;
	}


	return false;
}

void GPlayerManager::DropItem(CObject *player, CItem *item)
{
	//typedef char (__stdcall *DropItem)(CItem *item);
	//DropItem pDropItem = (DropItem)0x62F240;
	//00630AB0    56              push esi

	//0062F810    56              push esi
	//0062F670    56              push esi
	//006346A0    56              push esi
	//006355B0    56              push esi
	//006415B0    56              push esi
	//00644F60    56              push esi
	//00646FF0    56              push esi





	DWORD address = 0x00646FF0; //81 ?? ?? F8 FF FF E9 -F8
	//81C1????FFFFE8????????C20800
	//5F5EB0015BC20400CCCC56

	typedef char(__thiscall *_DropItem)(CObject *person, CItem *item);
	_DropItem pDropItem = (_DropItem)address;

	pDropItem(player, item);
	//__asm {
	//	pushad
	//	pushfd
	//	//mov byte ptr[item + 0x19D], 1
	//	//mov byte ptr[player + 0x19D], 1

	//	//mov byte ptr[item + 0x19E], 1
	//	//mov byte ptr[player + 0x19E], 1

	//	mov ecx, player
	//	push item
	//	call address
	//	popfd
	//	popad
	//}
}

void GPlayerManager::DropItem(CEntity *player, CItem *item)
{
	//////008DA870    8B4424 0C       mov eax, dword ptr ss:[esp+C]                ; movetoground
	//typedef GameValue*(__cdecl* bla_t)(DWORD state, GameValue *oper);
	//bla_t bla = (bla_t)0x008DA870;

	////010A6858

	//GameValue oper;
	//oper.data = (GameData*)item;

	//GameValue *result = bla(0x010A6858, &oper);
}

void GPlayerManager::MoveItem(CObject *player, CItem *item)
{
	/*typedef char(__stdcall *MoveItem)(CItem *item);
	MoveItem pMoveItem = (MoveItem)0x062DA30;

	__asm {
		pushad
		mov ecx, player
		push item
		call pMoveItem
		popad
	}*/
}

void GPlayerManager::ItemMoveIn(CItem *item)
{
	//004A8F20    56              push esi
	/*DWORD address = 0x004A8F20;

	__asm {
		mov byte ptr[item + 0x19D], 1
		mov byte ptr[item + 0x19E], 1
		pushad
		mov ecx, item
		call address
		popad
	}*/
}

void GPlayerManager::ItemMoveOut(CEntity *player, CItem *item)
{
	//004A8F20    56              push esi
	//DWORD address = 0x004A8F90;
	//CObject* lol = player->Object;
	//__asm {
	//	//mov byte ptr[item + 0x19D], 1
	//	//mov byte ptr[item + 0x19E], 1
	//	pushad
	//	mov ecx, item
	//	push lol
	//	call address
	//	popad
	//}
}

void GPlayerManager::MoveToGround(CItem *item)
{
	//DWORD address = 0x004B34C0;
	////004B34C0    56              push esi

	//__asm {
	//	pushad
	//	mov byte ptr[item + 0x19D], 1
	//	mov byte ptr[item + 0x19E], 1

	//	mov ecx, item
	//	call address
	//	popad
	//}
}

/*
004EB750    8B49 28         mov ecx, dword ptr ds:[ecx+28]                                     ; CNetworkManager::AskForDropItem
004EB753    85C9            test ecx, ecx
004EB755    74 05           je short DayZ.004EB75C
004EB757    E9 34D10300     jmp DayZ.00528890
004EB75C    C2 0800         retn 8
004EB750    8B49 28         mov ecx, dword ptr ds:[ecx+28]                       ; NetworkManager::AskForDropItem


004EB010    8B49 28         mov ecx, dword ptr ds:[ecx+28]
004EB000    8B49 28         mov ecx, dword ptr ds:[ecx+28]
004EAFE0    8B49 28         mov ecx, dword ptr ds:[ecx+28]
*/

void GPlayerManager::AskForDropItem(CObject *player, CItem *item)
{
	
	//DWORD address = 0x04F0990;

	//__asm {
	//	pushad
	//	mov ecx, networkmgr
	//	push item
	//	push player
	//	call address
	//	popad
	//}

	typedef void(__thiscall* Ask_t)(CNetworkManager *networkmgr, CObject *person, CItem *item);
	Ask_t pAsk = (Ask_t)0x4F0D00;

	__asm pushad
	__asm mov ecx, networkmgr
	pAsk(networkmgr, player, item);
	__asm popad

	/*
	 void __thiscall NetworkManager__AskForTakeItemToHands(NetworkManager *this, Person *person, InventoryItem *item)

	 004F0960    8B49 28         mov ecx, dword ptr ds:[ecx+28]           ; AskForDropItem
	 004F0980    8B49 28         mov ecx, dword ptr ds:[ecx+28]           ; AskForTakeItemToHands
	 004F0990    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F0C60    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F0CE0    8B49 28         mov ecx, dword ptr ds:[ecx+28] ;
	 004F0CF0    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F0D00    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F0DC0    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F0DD0    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F0E20    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F0F20    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F0F70    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F0F90    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F10E0    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F1100    8B49 28         mov ecx, dword ptr ds:[ecx+28]
	 004F1180    8B49 28         mov ecx, dword ptr ds:[ecx+28]

	*/
}

/*
008C8597    52              push edx                                 ; oper
008C8598    8D4424 28       lea eax, dword ptr ss:[esp+28]
008C859C    894C24 44       mov dword ptr ss:[esp+44], ecx
008C85A0    0FAE5424 44     ldmxcsr dword ptr ss:[esp+44]
008C85A5    68 30680A01     push DayZ.010A6830                       ; gamestate
008C85AA    50              push eax                                 ; result
008C85AB    E8 2073FFFF     call DayZ.008BF8D0
*/
void GPlayerManager::ShowHint(char *text)
{
	typedef int(__cdecl* ShowHint_t)(void *result, DWORD gamestate, GameValue *oper);
	ShowHint_t pShowHint = (ShowHint_t)0x8BF8D0;

	char buf[1000];

	CArmaString bla;
	strcpy(bla.szName, text);
	bla.iNameLength = strlen(text);

	GameValue val;
	val.vFTable = (void*)&bla;

	pShowHint(buf, 0x10A6830, &val);
}

/*
0092F9F0    83EC 54         sub esp, 54

xor     esi, esi
.text:005E187F 56                          push    esi             ; collisionFlags
.text:005E1880 6A 04                       push    4               ; typeSec
.text:005E1882 56                          push    esi             ; typePri
.text:005E1883 51                          push    this            ; radius
.text:005E1884 89 45 10                    mov     [ebp+74h+var_64], eax
.text:005E1887 D9 1C 24                    fstp    [esp+230h+var_230]
.text:005E188A 8D 45 34                    lea     eax, [ebp+74h+newPos]
.text:005E188D 50                          push    eax             ; end
.text:005E188E 8D 45 14                    lea     eax, [ebp+74h+origPos]
.text:005E1891 50                          push    eax             ; beg
.text:005E1892 8D 45 08                    lea     eax, [ebp+74h+f]
.text:005E1895 50                          push    eax             ; f
.text:005E1896 8D 45 EC                    lea     eax, [ebp+74h+collision]
.text:005E1899 50                          push    eax             ; retVal
.text:005E189A 8B 03                       mov     eax, [ebx]
.text:005E189C 8B CB                       mov     this, ebx
.text:005E189E C7 45 08 64+                mov     [ebp+74h+f.vfptr], offset ??_7FilterIgnoreTwo@Landscape@@6B@ ; const Landscape::FilterIgnoreTwo::`vftable'
.text:005E18A5 89 5D 0C                    mov     [ebp+74h+var_68], ebx
.text:005E18A8 FF 90 C8 00+                call    dword ptr [eax+0C8h]
.text:005E18AE 51                          push    this            ; age
.text:005E18AF 8B 0D 48 12+                mov     this, ?GLandscape@@3PAVLandscape@@A ; this
.text:005E18B5 D9 1C 24                    fstp    [esp+244h+var_244]
.text:005E18B8 E8 B3 3B E3+                call    ?ObjectCollisionLine@Landscape@@QBEXMAAVCollisionBuffer@@ABVIntersectionFilter@@ABVVector3P@@2MW4ObjIntersect


006AD25D    8B0D 18380A01   mov ecx, dword ptr ds:[10A3818] ;GLandScape

void __thiscall Landscape__ObjectCollisionLine(Landscape *this, float age, CollisionBuffer *retVal, IntersectionFilter *f, Vector3P *beg, Vector3P *end, float radius, ObjIntersect typePri, ObjIntersect typeSec, unsigned int collisionFlags)
*/

//struct CollisionBuffer {
//	char pad[100];
//};
//
//struct IntersectionFilter {
//	void *vfptr;
//};
//
//typedef GVector Vector3P;
//typedef int ObjIntersect;
//typedef void* Landscape;
//
//
//char GPlayerManager::Intersect(GVector start, GVector end)
//{
//	//0092F9F0    83EC 54         sub esp, 54
//	typedef void(__thiscall* ObjectCollisionLine_t)(Landscape *thisptr, float age, CollisionBuffer *retVal, IntersectionFilter *f, Vector3P *beg, Vector3P *end, float radius, ObjIntersect typePri, ObjIntersect typeSec, unsigned int collisionFlags);
//	ObjectCollisionLine_t pObjectCollisionLine = (ObjectCollisionLine_t)0x0092F9F0;
//
//	CollisionBuffer result = { 0 };
//	IntersectionFilter filter;
//
//	filter.vfptr = reinterpret_cast<void*>(0x00E3C2BC);
//
//	//008274B3    8B0D 18380A01   mov ecx, dword ptr ds:[10A3818]
//	__asm pushad
//	__asm mov ecx, dword ptr ds : [0x10A3818]
//	pObjectCollisionLine(reinterpret_cast<Landscape*>(0x10A3818), 0.0f, &result, &filter, &start, &end, 0.0f, 0, 4, 0);
//	__asm popad
//
//	return 1;
//}