#ifndef __GSDK__
#define __GSDK__

#pragma warning(disable: 4005 4305 4512 4996 4244 4800)

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <sstream>
#include <list>

#include <windows.h>
#include <stdlib.h>
#include <dwmapi.h>
#include <time.h>

#include <D3D11.h>
#include <d3dx11.h>
#include <DXErr.h>
#include <D3DX11async.h>
#include <D3Dcompiler.h>
#include <D3dx11effect.h>
#include <d3dx10math.h>
#include <D3D11Shader.h>
#include "D3D9_SDK\d3d9.h"
#include "D3D9_SDK\d3dx9.h"

#include "FW1_Source/FW1FontWrapper.h"
#include "xorstr.h"

#include "GColor.h"
#include "GMath.h"

#pragma comment(lib, "DXErr")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DX10.lib")
#pragma comment(lib, "D3DX11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "Effects11.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dxgi.lib")

#define USE_D3D9


/*
const DWORD ADR_BASE = 0x00E51E30;
const DWORD ADR_PLAYERINFO = 0x00E44630;
const DWORD ADR_TRANSFORMATIONS = 0x00EA2F44;

DWORD buf = *(DWORD*)ADR_DEVICEPOINTER;
buf += 0x18A0;
buf = *(DWORD*)buf;
buf = *(DWORD*)buf;

0x0C618B80
0x0C61B91C

00771D92    8B0D 28A6E400   mov ecx, dword ptr ds:[E4A628]
00A148F9  |.  8D8A A0180000 |lea ecx, dword ptr ds:[edx+18A0]

00430A54    F0:0FC111       lock xadd dword ptr ds:[ecx], edx        ; LOCK prefix
00430A58    8B0D 28A6E400   mov ecx, dword ptr ds:[E4A628]
00430A5E    8930            mov dword ptr ds:[eax], esi
00430A60    8B01            mov eax, dword ptr ds:[ecx]
00430A62    8B90 E0020000   mov edx, dword ptr ds:[eax+2E0]
00430A68    FFD2            call near edx
00430A6A    A1 A03FE400     mov eax, dword ptr ds:[E43FA0]
00430A6F    8B48 08         mov ecx, dword ptr ds:[eax+8]
00430A72    3BCB            cmp ecx, ebx
00430A74    74 07           je short DayZ.00430A7D
00430A76    8B11            mov edx, dword ptr ds:[ecx]
00430A78    8B42 18         mov eax, dword ptr ds:[edx+18]
00430A7B    FFD0            call near eax
00430A7D    8B4C24 2C       mov ecx, dword ptr ss:[esp+2C]
00430A81    8B11            mov edx, dword ptr ds:[ecx]
00430A83    8B92 14010000   mov edx, dword ptr ds:[edx+114]
00430A89    68 CCB0D300     push DayZ.00D3B0CC                       ; ASCII "cobraLight"

0043468C    8B0D 28A6E400   mov ecx, dword ptr ds:[E4A628]
00434692    895C24 10       mov dword ptr ss:[esp+10], ebx
00434696    8B11            mov edx, dword ptr ds:[ecx]
00434698    8B82 70010000   mov eax, dword ptr ds:[edx+170]
0043469E    53              push ebx
0043469F    FFD0            call near eax
004346A1    50              push eax
004346A2    8BCE            mov ecx, esi
004346A4    E8 87030000     call DayZ.00434A30
004346A9    A1 9445DF00     mov eax, dword ptr ds:[DF4594]
004346AE    8B7D 08         mov edi, dword ptr ss:[ebp+8]
004346B1    8B17            mov edx, dword ptr ds:[edi]
004346B3    8B92 08010000   mov edx, dword ptr ds:[edx+108]
004346B9    50              push eax
004346BA    68 1CB2D300     push DayZ.00D3B21C                       ; ASCII "viewDistance"



9fadc0
*/
//Generated using ReClass 2013 Mod by CypherPresents

const DWORD ADR_DEVICEPOINTER = 0x00E4A628;

#define M_PI		3.14159265358979323846
#define M_PI_2      1.57079632679489661923

typedef struct {
	int x, y, m_x, m_y;
} Screen_t;


struct textsize {
	int w, h;
};

struct sVars
{
	int _AIM;
	int AIM_On;
	int AIM_Player;
	int AIM_Zombie;
	int AIM_Key;
	int AIM_Mode;
	int AIM_Friends;
	int AIM_Animals;
	int AIM_Pos;
	int AIM_Lock;
	int AIM_Telekill;

	int _ESP_PLAYER;
	int _ESP_ITEM;
	int _ESP_OTHER;
	int ESP_Player;
	int ESP_Zombie;
	int ESP_Name;
	int ESP_Dot;
	int ESP_City;
	int ESP_View;
	int ESP_Radar;
	int ESP_Weapon;
	int ESP_Health;
	int ESP_ShowDead;
	int ESP_Box;
	int ESP_State;
	int ESP_Inventar;

	int ESP_Clothing;
	int ESP_House;
	int ESP_Inventory;
	int ESP_Magazine;
	int ESP_Weapons;
	int ESP_Unfiltered;
	int ESP_Other;
	int ESP_Teleport;
	int ESP_Animals;
	int ESP_Effects;
	int ESP_Far;
	int ESP_Ruined;

	int _MISC;
	int MISC_Fullscreen;
	int MISC_Healthbars;
	int MISC_Crosshair;
	int MISC_ZombieWarning;
	int MISC_Statsbox;
	int MISC_Glitcher;
	int MISC_Hover;
	int MISC_UnlimitedAmmo;
	int MISC_Removals;
	int MISC_Swim;
	int MISC_Worldtime;

	int _D3D;
	int D3D_Wallhack;
	int D3D_Fullbright;
	int D3D_Wireframe;
	int D3D_Nofog;
	int D3D_Stride;
	int D3D_NumVertices;
	int D3D_PrimCount;

	int radar_x;
	int radar_y;
	int radar_range;

	int statsbox_x;
	int statsbox_y;

	bool console;

	bool toggle_steal;

	DWORD		DevicePointer;
	Screen_t	Screen;
};

class GTimer {

public:
	GTimer() {
		freq = 1;
		start_time = 0;
	}

	void start()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);

		freq = double(li.QuadPart) / 1000000.0;

		QueryPerformanceCounter(&li);
		start_time = li.QuadPart;
	}

	double get_us() {
		return get();
	}

	double get_ms() {
		return get() / 1000;
	}

	double get_s() {
		return get() / 1000 / 1000;
	}

private:
	double freq;
	__int64 start_time;

	double get()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart - start_time) / freq;
	}
};


extern sVars cvars;

class CBase;
class CWorld;
class CObjectTable;
class CObject;
class CEntity;
class CObjectVisualState;
class CObjectClass;
class CObjectList;
class CArmaString;
class CPlayerTable;
class CNetworkManager;
class CPlayerInfo;
class CWeapon;
class CWeaponPtr;
class CObjTable;

class CD3D9
{
public:
	char _0x0000[6304];
	IDirect3DDevice9 *dev; //0x18A0 

};//Size=0x18A4

class CGameTime //E870D0
{
public:
	char _0x0000[44];
	__int16 time2; //0x002C 
	__int16 time; //0x002E 
};

class CNetworkManager
{
public:
	char _0x0000[40]; //v44 => 36, v45 => 40
	CPlayerInfo* PlayerInfo; //0x0028 

};//Size=0x002C

class CPlayerTable
{
public:
	char N0094F550[4]; //0x0000  CInfo[40]; sizeof( CInfo ) = 0xE8
	__int32 iPlayerID; //0x0004 
	char N0094F552[120]; //0x0008 
	CArmaString* PlayerName; //0x0080 
	char N009AF8FC[100]; //0x0084 

};//Size=0x00E8

class CPlayerInfo
{
public:
	char N0095DD30[12]; //0x0000 
	CPlayerTable* PlayerTable; //0x000C 
	__int32 iPlayerCount; //0x0010 

};//Size=0x0014

class CObjTable
{
public:
	CObject **obj; //0x0000 
	__int32 size; //0x0004 
	char _0x0008[160];

};//Size=0x00A8

const enum OBJ_TABLE {
	ANIMALS,
	FAR_ANIMALS,
	FARFAR_ANIMALS,
	EFFECTS,
	FAR_EFFECTS,
	ITEM,
	FAR_BUILDING,
	INVALID_ITEM,
	DROPPED_ITEM,
	FAR_DROPPED_ITEM,
	SMOKE,
	UNK,
};

class CObjTbl {
public:
	CObject **obj; //0x0000 
	__int32 size; //0x0004 
	__int32 max_size; //0x0008 
	char _0x000C[8];

};//Size=0x0014


class CWorld {
public:
	char _0x0000[1912];
	CObjectTable* Objects; //0x0778 
	char _0x077C[572];
	CObjTbl SmokeEffectsTable; //0x09B8  
	char _0x09CC[168];
	CObjTbl AnimalTable; //0x0A74 
	char _0x0A88[148];
	CObjTbl FarAnimalTable; //0x0B1C 
	char _0x0B30[316];
	CObjTbl FarFarAnimalTable; //0x0C6C   up to 1000 worldunits
	char _0x0C80[152];
	CObjTbl EffectsTable; //0x0D18 
	char _0x0D2C[148];
	CObjTbl FarEffectsTable; //0x0DC0 
	char _0x0DD4[488];
	CObjTbl DroppedItemTable; //0x0FBC 
	char _0x0FD0[148];
	CObjTbl FarDroppedItemTable; //0x1064 didnt check that one
	char _0x1078[316];
	CObjTbl ItemTable; //0x11B4 
	char _0x11C8[148];
	CObjTbl FarBuildingTable; //0x125C   PAPI, municipal office 
	char _0x1270[192];
	CObjTbl InvalidItemTable; //0x1330 
	char _0x1344[848];
	CEntity* LocalPlayer; //0x1694 

};//Size=0x1698
//v44
//class CWorld {
//public:
//	char _0x0000[1912];
//	CObjectTable* Objects; //0x0778 
//	char _0x077C[760];
//	CObjTable AnimalTable; //0x0A74 
//	CObjTable FarAnimalTable; //0x0B1C
//	char _0x0BC4[340];
//	CObjTable EffectsTable; //0x0D18 
//	CObjTable FarEffectsTable; //0x0DC0 
//	char _0x0E68[844];
//	CObjTable ItemTable; //0x11B4 
//	CObjTable FarItemTable; //0x125C 
//	CObjTable FarBuilidingTable3; //0x1304 
//	char _0x13AC[536];
//	CEntity* LocalPlayer; //0x15C4 
//
//};



//class CWorld {
//public:
//	char _0x0000[1912];
//	CObjectTable* Objects; //0x0778 
//	char _0x077C[760];
//	CObjTable AnimalTable; //0x0A74 
//	CObjTable FarAnimalTable; //0x0B1C b18
//	char _0x0BC4[340];
//	CObjTable EffectsTable; //0x0D18 
//	CObjTable FarEffectsTable; //0x0DC0 
//	char _0x0E68[340];
//	CObjTable ItemTable; //0x0FBC 
//	CObjTable FarItemTable; //0x1064 
//	char _0x110C[168];
//	CObjTable FarBuildingTable1; //0x11B4 
//	CObjTable FarBuildingTable2; //0x125C 
//	CObjTable FarBuildingTable3; //0x1304 
//	char _0x13AC[536];
//	CEntity* LocalPlayer; //0x15C4 
//
//};//Size=0x15C8
/*
0072CDD3   .  8D8E D4160000             lea ecx, dword ptr ds:[esi+16D4]
0072CDD9   .  E8 D24ACDFF               call DayZ.004018B0
0072CDDE   .  66:C786 D8160000 0000     mov word ptr ds:[esi+16D8], 0
0072CDE7   .  66:C786 5E160000 0101     mov word ptr ds:[esi+165E], 101
0072CDF0   .  891D 0875E800             mov dword ptr ds:[E87508], ebx
0072CDF6   .  8B4E 0C                   mov ecx, dword ptr ds:[esi+C]
0072CDF9   .  85C9                      test ecx, ecx
0072CDFB   .  74 05                     je short DayZ.0072CE02
0072CDFD   .  8B01                      mov eax, dword ptr ds:[ecx]
0072CDFF   .  FF50 28                   call near dword ptr ds:[eax+28]
0072CE02   >  E8 A3E80E00               call DayZ.0081B6AA
0072CE07   .  8B8E 74070000             mov ecx, dword ptr ds:[esi+774]                             ;  CObjectTable* Objects; //0x0774
0072CE0D   .  E8 202B0000               call DayZ.0072F932
0072CE12   .  8B8E 74070000             mov ecx, dword ptr ds:[esi+774]
0072CE18   .  E8 152B0000               call DayZ.0072F932
0072CE1D   .  8B86 C0150000             mov eax, dword ptr ds:[esi+15C0]                            ;  CEntity* LocalPlayer; //0x15C0
0072CE23   .  FF70 04                   push dword ptr ds:[eax+4]
0072CE26   .  E8 2598CFFF               call DayZ.00426650
0072CE2B   .  8B76 0C                   mov esi, dword ptr ds:[esi+C]
0072CE2E   .  59                        pop ecx
0072CE2F   .  85F6                      test esi, esi
0072CE31   .  74 0C                     je short DayZ.0072CE3F
0072CE33   .  85C0                      test eax, eax
0072CE35   .  74 08                     je short DayZ.0072CE3F

*/
/*
0061FE19   .  56            push esi
0061FE1A   .  6A 00         push 0
0061FE1C   .  8BF1          mov esi, ecx
0061FE1E   .  E8 716CFBFF   call DayZ.005D6A94
0061FE23   .  8BCE          mov ecx, esi
0061FE25   .  E8 7D6CFBFF   call DayZ.005D6AA7
0061FE2A   .  8B0D 301EE500 mov ecx, dword ptr ds:[E51E30]           ;  world
0061FE30   .  8B81 C0150000 mov eax, dword ptr ds:[ecx+15C0]         ;  localplayer
0061FE36   .  FF70 04       push dword ptr ds:[eax+4]
0061FE39   .  E8 42D01100   call DayZ.0073CE80
0061FE3E   .  85C0          test eax, eax
0061FE40   .  74 2D         je short DayZ.0061FE6F
0061FE42   .  8B0D 301EE500 mov ecx, dword ptr ds:[E51E30]           ;  world
0061FE48   .  8B81 C0150000 mov eax, dword ptr ds:[ecx+15C0]         ;  localplayer
0061FE4E   .  FF70 04       push dword ptr ds:[eax+4]
0061FE51   .  E8 2AD01100   call DayZ.0073CE80
0061FE56   .  8BC8          mov ecx, eax
0061FE58   .  E8 CF3F2000   call DayZ.00823E2C
0061FE5D   .  3BC6          cmp eax, esi
0061FE5F   .  75 0E         jnz short DayZ.0061FE6F
0061FE61   .  A1 301EE500   mov eax, dword ptr ds:[E51E30]
0061FE66   .  8B48 0C       mov ecx, dword ptr ds:[eax+C]
0061FE69   .  8B01          mov eax, dword ptr ds:[ecx]
0061FE6B   .  56            push esi


0072DBF0  |.  85FF          test edi, edi
0072DBF2  |.  75 0E         jnz short DayZ.0072DC02
0072DBF4  |.  8B45 08       mov eax, dword ptr ss:[ebp+8]
0072DBF7  |.  C700 FFFFFF7F mov dword ptr ds:[eax], 7FFFFFFF
0072DBFD  |.  E9 02010000   jmp DayZ.0072DD04
0072DC02  |>  8B4D 10       mov ecx, dword ptr ss:[ebp+10]
0072DC05  |.  8B86 C4070000 mov eax, dword ptr ds:[esi+7C4]
0072DC0B  |.  8BB1 AC020000 mov esi, dword ptr ds:[ecx+2AC]
0072DC11  |.  6BC0 2C       imul eax, eax, 2C
0072DC14  |.  8B53 0C       mov edx, dword ptr ds:[ebx+C]
0072DC17  |.  6BF6 48       imul esi, esi, 48
0072DC1A  |.  037410 14     add esi, dword ptr ds:[eax+edx+14]
0072DC1E  |.  8B57 20       mov edx, dword ptr ds:[edi+20]           ;  object1->movement/visualstate
0072DC21  |.  8B41 20       mov eax, dword ptr ds:[ecx+20]           ;  object2->movement/visualstate
0072DC24  |.  F3:0F1042 28  movss xmm0, dword ptr ds:[edx+28]        ;  buffer_x = obj1.x
0072DC29  |.  F3:0F5C40 28  subss xmm0, dword ptr ds:[eax+28]        ;  buffer_x -= obj2.x
0072DC2E  |.  F3:0F104A 2C  movss xmm1, dword ptr ds:[edx+2C]        ;  buffer_y = obj1.y
0072DC33  |.  F3:0F5C48 2C  subss xmm1, dword ptr ds:[eax+2C]        ;  buffer_y -= obj2.y
0072DC38  |.  F3:0F1052 30  movss xmm2, dword ptr ds:[edx+30]        ;  buffer_z = obj1.z
0072DC3D  |.  F3:0F5C50 30  subss xmm2, dword ptr ds:[eax+30]        ;  buffer_z -= obj2.z
0072DC42  |.  8B01          mov eax, dword ptr ds:[ecx]              ;  da wird wohl das delta zwischen 2 positionen ausgerechnet
0072DC44  |.  F3:0F59C0     mulss xmm0, xmm0
0072DC48  |.  F3:0F59C9     mulss xmm1, xmm1
0072DC4C  |.  F3:0F58C1     addss xmm0, xmm1
0072DC50  |.  F3:0F59D2     mulss xmm2, xmm2
0072DC54  |.  F3:0F58C2     addss xmm0, xmm2                         ;  delta_x*delta_x + delta_y*delta_y + delta_z* delta_z
0072DC58  |.  8D56 24       lea edx, dword ptr ds:[esi+24]
0072DC5B  |.  F3:0F1145 10  movss dword ptr ss:[ebp+10], xmm0
0072DC60  |.  F3:0F1046 40  movss xmm0, dword ptr ds:[esi+40]
0072DC65  |.  52            push edx
0072DC66  |.  F3:0F1145 0C  movss dword ptr ss:[ebp+C], xmm0
0072DC6B  |.  FF90 94050000 call near dword ptr ds:[eax+594]
0072DC71  |.  D95D FC       fstp dword ptr ss:[ebp-4]
0072DC74  |.  8B07          mov eax, dword ptr ds:[edi]
0072DC76  |.  8D4E 08       lea ecx, dword ptr ds:[esi+8]
0072DC79  |.  51            push ecx
0072DC7A  |.  8BCF          mov ecx, edi
0072DC7C  |.  FF90 94050000 call near dword ptr ds:[eax+594]


*/

class CObjectList
{
public:
	CEntity* Object; //0x0000 
	char N00733863[44]; //0x0004 

};//Size=0x0030

class CObjectTable
{
public:
	CObjectList* ObjectList; //0x0000 
	__int32 iObjectSize; //0x0004 

};//Size=0x0008

class CGameVariableEntry {
public:
	char _0x0000[4]; //0x0000
	CArmaString *name; //0x0004
	char _0x0008[12]; //0x0008
};

class CGameVariableEntryTable {
public:
	CGameVariableEntry table[1024];
};

class CGameVariables
{
public:
	CGameVariableEntryTable* start; //0x0004 
	__int32 count; //0x0008 
	char _0x000C[4]; //0x000C

};//Size=0x0014

class CGameVariablesTable {
public:
	CGameVariables var[1024];
};

class CEntity
{
public:
	__int32 connected; //0x0000 
	CObject* Object; //0x0004 
	BYTE test[0x200];

};//Size=0x0024


enum STANCE {
	STANDING = 0,
	KNEEING = 1,
	PRONE = 2
};


class CItemContainer
{
public:
	char _0x0000[12];
	CObjTbl items; //0x000C 

};//Size=0x0504

class CBound {
public:
	char _0x0000[352];
	GVector min; //0x0160 
	GVector max; //0x016C 
	GVector min_; //0x0178 
	GVector max_; //0x0184 
	char _0x0190[756];
};

class CItem {
public:
	char _0x0000[36];
	CObjectVisualState* visualstate; //0x0024 
	CBound* bounds; //0x0028 
	char _0x002C[68];
	CObjectClass* ObjectClass; //0x0070 
	char _0x0074[298];
	BYTE reset; //0x019E 
	char _0x019F[213];
	BYTE ruined; //0x0274 
	char _0x0275[15];
	CItemContainer* ItemContainer; //0x0284 

};//Size=0x0288


class CObject {
public:
	char _0x0000[36];
	CObjectVisualState* visualstate; //0x0024 
	CBound* bounds; //0x0028 
	char _0x002C[68];
	CObjectClass* ObjectClass; //0x0070 
	char _0x0074[76];
	DWORD time; //0x00C0 
	char _0x00C4[212];
	BYTE objectContact; //0x0198 
	BYTE landContact; //0x0199 
	BYTE waterContact; //0x019A 
	BYTE _delete; //0x019B 
	BYTE local; //0x019C 
	BYTE invisble; //0x019D 
	char _0x019E[214];
	BYTE isDead; //0x0274 
	BYTE isStopped; //0x0275 
	BYTE inFormation; //0x0276 
	BYTE showFlag; //0x0277 
	BYTE isUpsideDown; //0x0278 
	BYTE userStopped; //0x0279 
	BYTE pilotLight; //0x027A 
	BYTE collisionLight; //0x027B 
	BYTE allowDamage; //0x027C 
	BYTE locked; //0x027D 0 = moving, 1 = not moving
	BYTE lockedAsWaiting; //0x027E 
	char _0x027F[973];
	CGameVariablesTable* GameVariables; //0x064C 
	__int32 GameVariables_Count; //0x0650 
	char _0x0654[404];
	__int32 playerid; //0x07E8 
	char _0x07EC[560];
	CItem *InventorySlot[12]; //0x0A1C 
	CItem *QuickSlot[10]; //0x0A4C 
	char _0x0A74[12];
	CWeaponPtr* weapon; //0x0A80 
	char _0x0A84[8];
	float pitch[30]; //0x0A8C 
	char _0x0B04[312];
	float recoil; //0x0C3C 
	char _0x0C40[8];
	float recoilRandomX; //0x0C48 
	float recoilRandomY; //0x0C4C 
	float recoilRandomZ; //0x0C50 
	char _0x0C54[4];
	float fatigue_diff; //0x0C58 
	float fatigue; //0x0C5C 
	char _0x0C60[20];
	BYTE swim; //0x0C74 
	char _0x0C75[107];
	WORD state2; //0x0CE0 
	char _0x0CE2[6];
	WORD state; //0x0CE8 
	char _0x0CEA[1534];
	BYTE stance; //0x12E8 
	char _0x12E9[3];

};//Size=0x12EC

/*
004CBB31  \.^\EB E8         jmp short DayZ.004CBB1B
004CBB33  /$  8B4424 04     mov eax, dword ptr ss:[esp+4]
004CBB37  |.  8B89 600A0000 mov ecx, dword ptr ds:[ecx+A60]
004CBB3D  |.  6BC0 0C       imul eax, eax, 0C
004CBB40  |.  8B4401 08     mov eax, dword ptr ds:[ecx+eax+8]
004CBB44  \.  C2 0400       retn 4
004CBB47  /.  55            push ebp
004CBB48  |.  8BEC          mov ebp, esp
004CBB4A  |.  83EC 0C       sub esp, 0C
004CBB4D  |.  56            push esi
004CBB4E  |.  8BF1          mov esi, ecx
004CBB50  |.  8D4D F4       lea ecx, dword ptr ss:[ebp-C]
004CBB53  |.  E8 B5454B00   call DayZ.0098010D
004CBB58  |.  8B06          mov eax, dword ptr ds:[esi]
004CBB5A  |.  8BCE          mov ecx, esi
004CBB5C  |.  FF90 58010000 call near dword ptr ds:[eax+158]
004CBB62  |.  5E            pop esi
004CBB63  |.  85C0          test eax, eax
004CBB65  |.  74 2C         je short DayZ.004CBB93
004CBB67  |.  8D4D F4       lea ecx, dword ptr ss:[ebp-C]
004CBB6A  |.  51            push ecx
004CBB6B  |.  68 A000C100   push DayZ.00C100A0                       ;  ASCII "temperature"
004CBB70  |.  8BC8          mov ecx, eax

*/

//class CWeapon
//{
//public:
//char _0x0000[112];
//	CObjectClass* ObjectClass; //0x0070 
//char _0x0074[1552];
//	CItem **slots; //0x0684 
//	__int32 num_slots; //0x0688 
//	DWORD ammo_test1; //0x068C 
//char _0x0690[8];
//	DWORD ammo_count1; //0x0698 
//char _0x069C[4];
//	CWeaponPtr* magazine; //0x06A0 
//	DWORD ammo_test2; //0x06A4 
//char _0x06A8[8];
//	DWORD ammo_count2; //0x06B0 
//
//};//Size=0x06B4

class CWeapon {
public:
	char _0x0000[112];
	CObjectClass* ObjectClass; //0x0070 
	char _0x0074[1560];
	CItem **slots; //0x068C 
	__int32 num_slots; //0x0690 
	DWORD ammo_test1; //0x0694 
	char _0x0698[8];
	DWORD ammo_count1; //0x06A0 
	char _0x06A4[4];
	CWeaponPtr* magazine; //0x06A8 
	DWORD ammo_test2; //0x06AC 
	char _0x06B0[8];
	DWORD ammo_count2; //0x06B8 

};//Size=0x06BC

class CWeaponPtr
{
public:
	char _0x0000[4];
	CWeapon* object; //0x0004 
};//Size=0x0040

class CObjectVisualState
{
public:
	char _0x0000[4];
	GVector forward; //0x0004 
	GVector up; //0x0010 
	GVector right; //0x001C 
	GVector pos; //0x0028 
	char _0x0034[20];
	GVector speed; //0x0048 
	char _0x0054[40];
	GVector forward_; //0x007C 
	char _0x0088[12];
	GVector right_; //0x0094 
	char _0x00A0[144];
	GVector head_pos; //0x0130 
	GVector body_pos; //0x013C 

};//Size=0x0148

class CObjectClass {
public:
	char _0x0000[52];
	CArmaString* ObjectClassName; //0x0034 
	char _0x0038[4];
	CArmaString* ObjectModelPath; //0x003C 
	char _0x0040[48];
	CArmaString* ObjectName; //0x0070 
	char _0x0074[1304];
	CArmaString* CleanObjectName; //0x058C 
	CArmaString* CleanObjectName2; //0x0590 
	CArmaString* ObjectInternalName; //0x0594 
	char _0x0598[1236];
	__int32 inventorySlotId; //0x0A6C 
	char _0x0A70[144];
	BYTE showInHands; //0x0B00 
	BYTE N003B743A; //0x0B01 
	BYTE handheld; //0x0B02 
	BYTE autoQuickbar; //0x0B03 
	BYTE openable; //0x0B04 
	BYTE rotateDropped; //0x0B05 
	BYTE rotateInvert; //0x0B06 
	BYTE rotateZ; //0x0B07 
	__int32 max_stack; //0x0B08 
	float spawnDamageMax; //0x0B0C 
	BYTE melee; //0x0B10 
	char _0x0B11[11];
	BYTE visible; //0x0B1C 
	BYTE N0E3A4F96; //0x0B1D 
	BYTE N00293F77; //0x0B1E 
	BYTE N0E3A4F97; //0x0B1F 
	BYTE slots_needed; //0x0B20 
	char _0x0B21[99];
	DWORD max_ammo; //0x0B84 

};//Size=0x0B88

class CArmaString
{
public:
	char N007776CC[4]; //0x0000 
	__int32 iNameLength; //0x0004 
	char szName[256]; //0x0008 

};//Size=0x000C

class CTransformationData
{
public:
	char _0x0000[4];
	GVector right; //0x0004 
	GVector up; //0x0010 
	GVector forward; //0x001C 
	GVector translation; //0x0028 
	char _0x0034[32];
	GVector viewport; //0x0054 
	char _0x0060[304];
	GVector fov; //0x190

};//Size=0x0198

class CTransformations
{
public:
	char unknown0[0x94]; //0x0000
	CTransformationData* TransformationData; //0x0094  
};

class CTransformation
{
public:
	CTransformations* transform;
};

class CBase
{
public:
	CWorld* World; //0x0000 
};//Size=0x0004

typedef volatile INT RefCountBase;
typedef struct _RefCount RefCount, *PRefCount;
typedef struct _GameData GameData, *PGameData;
typedef struct _GameValue GameValue, *PGameValue;

struct _RefCount {
	/*000*/PVOID vFTable;
	/*004*/RefCountBase count;
};

struct _GameData {
	/*000*/RefCount refc;
	/*008*/PVOID vFTable_IDebugValue;
};


struct _GameValue {
	/*00*/PVOID vFTable;
	/*04*/PGameData data; // pointer to GameData(Type)
};

template<class Type>
class Ref {
protected:
	//! Reference to the object.
	Type *_ref;
public:
	//! make data type accessible
	typedef Type ItemType;
	//! Default constructor.
	__forceinline Ref() { _ref = NULL; }
	// Using this constructor you can get the existing reference from source parameter.
	/*!
	\param source Source pointer to an object.
	*/
	Ref(Type *source)
	{
		if (source) source->AddRef();
		_ref = source;
	}
	//! Copying of pointer.
	const Ref<Type> &operator = (Type *source)
	{
		Type *old = _ref;
		if (source) source->AddRef();
		// for MT safety of shared Ref objects we need to stop pointing to the object before we release the last ref to it
		_ref = source;
		if (old) old->Release();
		return *this;
	}
	// Copy constructor.
	/*!
	\param sRef Reference to the source pointer.
	*/
	Ref(const Ref &sRef)
	{
		Type *source = sRef._ref;
		if (source) source->AddRef();
		_ref = source;
	}
	//! Copying of pointer.
	const Ref<Type> &operator = (const Ref &sRef)
	{
		Type *source = sRef._ref;
		Type *old = _ref;
		if (source) source->AddRef();
		// for MT safety of shared Ref objects we need to stop pointing to the object before we release the last ref to it
		_ref = source;
		if (old) old->Release(); // Release will do Publish as well (part of InterlockedDecrementRelease)
		return *this;
	}
	//! Determining whether pointer is not null.
	/*!
	\return TRUE in case pointer is not null, FALSE otherwise.
	*/
	__forceinline bool NotNull() const { return _ref != NULL; }
	//! Determining whether pointer is null.
	/*!
	\return TRUE in case pointer is null, FALSE otherwise.
	*/
	__forceinline bool IsNull() const { return _ref == NULL; }
	//! Destructor - heart of automatic releasing.
	__forceinline ~Ref() { Free(); }
	//! This method will release the object (or at least decrements it's reference counter).
	/*!
	However you are not forced to use it. Object will release itself automatically.
	*/
	void Free() { if (_ref) _ref->Release(), _ref = NULL; }
	//! This method will return standard pointer to the object.
	/*!
	\return Standard pointer to the object.
	*/
	__forceinline Type *GetRef() const { return _ref; }
	//! This method will set reference to the object.
	/*!
	You must use this function with infernal caution. For instance if you set it carelessly to NULL,
	you can be sure that the previous object will never be deleted.
	\param ref New pointer to the object.
	*/
	__forceinline void SetRef(Type *ref) { _ref = ref; }
	//! This operator is suitable for accessing object members. It has the same meaning like standard -> operator.
	__forceinline Type *operator -> () const { return _ref; }
	//! Operator for type casting.
	__forceinline operator Type *() const { return _ref; }

	//! calculate memory used
	double GetMemoryUsed() const
	{
		// for each owner only appropriate part of shared object is considered
		if (!_ref) return 0;
		return double(sizeof(Type)+_ref->GetMemoryUsed()) / _ref->RefCounter();
	}
	//ClassIsMovableZeroed(Ref);
};



enum CollisionFlags {
	//!In many cases only collided=true/false is enough
	CF_FIRST_CONTACT = 1,
	//!We want only one, the nearest contact
	CF_NEAREST_CONTACT = 2,
	//!Only static objects
	CF_ONLY_STATIC = 4,
	//!Only dynamic objects
	CF_ONLY_DYNAMIC = 8,
	//!Only water components (legacy support for "walk on geometry")
	CF_ONLY_WATER = 16,
	//!Valid when CF_FIRST_CONTACT, we get first contact for each object
	CF_ALL_OBJECTS = 32,
};

enum ObjIntersect {
	ObjIntersectFire = 0,
	ObjIntersectView = 1,
	ObjIntersectGeom = 2,
	ObjIntersectIFire = 3,
	ObjIntersectNone = 4
};

class IntersectionFilter {
public:
	/// check if object collision should be tested
	virtual bool operator()(const CObject *) const = 0;
};

struct CollisionInfo {
	/// surface texture (Obsolete: use surface where possible) TODO: remove
	void *texture;
	/// surface material properties
	const void *surface;

	/// position of collision (in world coord)
	GVector pos;
	/// direction outside (in world coord) or (in case of line-object collision) direction and size of the intersection
	GVector dirOut;
	/// direction not normalized (in world coord) or (in case of line-object collision) world space plane normal in the point of intersection
	GVector dirOutNotNorm;

	/// Which hierarchy level is the collision detected at, 0 = objects in landscape, >0 = proxy
	int hierLevel;
	/// object,that we collide with (NULL if none), If hierLevel > 0 object is the proxy object
	Ref<CObject> object;
	/// If hierLevel > 0 most parent of the proxy object
	Ref<CObject> parentObject;

	/// how deep is contact (in world coord) or (in case of line-object collision) "time" of the intersection
	float under;
	/// index of component in corresponding geometry level
	int component;

	//is false if begining point was inside
	bool entry;
	//is false if end point was inside
	bool exit;

	/// which LOD level was the component found in - needed in case of secondary (typeSec) collisions
	int geomLevel;
#if _VBS3
	/// volume of intersection (but used to store modified impulse only)
	float underVolume;
#endif

	// if indestructible proxy is hit, assume that the parent object is hit instead
	//Object *HitObject() const { return (parentObject && (!object || (object->GetDestructType() == DestructNo))) ? parentObject : object; }
	//float MaxDeflectionAngleCos() const { return object ? object->MaxDeflectionAngleCos() : 0.0f; }
	//float BulletPenetrability() const { return (object && surface) ? surface->_bulletPenetrability : 0.0f; }
	//float Thickness() const { return (object && surface) ? surface->_thickness : -1.0f; }
	//float DistInside() const { return dirOut.Size(); }
};

class FilterNone : public IntersectionFilter {
public:
	bool operator()(const CObject *) const { return true; };
};

//! Filter one object. Used in RoadSurface* function
static bool FilterIgnoreOneHelper(const CObject *veh, const CObject *obj)
{
	//return !veh || veh != obj && obj->GetAttachedTo() != veh && veh->GetAttachedTo() != obj;
	return false;
}

//! Filter one object. Used in RoadSurface* function
class FilterIgnoreOne : public IntersectionFilter {
protected:
	const CObject * _obj;
public:
	FilterIgnoreOne(const CObject * obj) :_obj(obj) {}
	bool operator()(const CObject * obj) const
	{
		return FilterIgnoreOneHelper(_obj, obj);
	}
};

class FilterIgnoreTwo : public IntersectionFilter {
protected:
	const CObject * _obj1;
	const CObject * _obj2;
public:
	FilterIgnoreTwo(const CObject * obj1, const CObject * obj2) :_obj1(obj1), _obj2(obj2) {}
	bool operator()(const CObject * obj) const
	{
		return FilterIgnoreOneHelper(_obj1, obj) && FilterIgnoreOneHelper(_obj2, obj);
	}
};

//! Filter allows only primary objects. Used in RoadSurface* function
class FilterPrimary : public IntersectionFilter {
public:
	//bool operator()(const Object * obj) const { return obj->GetType() == Primary; };
};


#endif