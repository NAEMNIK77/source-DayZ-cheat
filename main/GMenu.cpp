#include "GDraw.h"
#include "GESP.h"
#include "GMenu.h"
#include "GPlayerManager.h"
#include "GTools.h"
#include "GOverlay.h"
#include "xorstr.h"


char* keylist[] = {"", "Left mouse button", "Right mouse button", "Control-break", 
"Middle mouse button", "X1 mouse button", "X2 mouse button", "Not available", "Backspace", 
"TAB", "Not available", "Not available", "Clear", "Return/Enter", 
"Not available", "Not available", "Shift", "CTRL", "ALT", 
"PAUSE", "CAPS LOCK", "Not available", "Not available", "Not available", 
"Not available", "Not available", "Not available", "ESC", "Not available", "Not available", 
"Not available", "Not available", "Spacebar", "Page up", "Page down", 
"End", "Home", "Left arrow", "Up arrow", 
"Right arrow", "Down arrow", "Select", "Print", "Execute", 
"Print screen", "Insert", "Delete", "Help", 
"0", "1", "2", "3", "4", 
"5", "6", "7", "8", "9", 
"Not available", "Not available", "Not available", "Not available", 
"Not available", "Not available", "Not available", "A", "B", 
"C", "D", "E", "F", "G", 
"H", "I", "J", "K", "L", 
"M", "N", "O", "P", "Q", 
"R", "S", "T", "U", "V", 
"W", "X", "Y", "Z", "Left windows key", 
"Right windows key", "Application key", "Not available", "Sleep", "Numpad 0", 
"Numpad 1", "Numpad 2", "Numpad 3", "Numpad 4", "Numpad 5", 
"Numpad 6", "Numpad 7", "Numpad 8", "Numpad 9", "Multiply", 
"Add", "Seperator", "Subtract", "Decimal", 
"Divide", "F1", "F2", "F3", "F4", 
"F5", "F6", "F7", "F8", "F9", 
"F10", "F11", "F12", "F13", "F14", 
"F15", "F16", "F17", "F18", "F19", 
"F20", "F21", "F22", "F23", "F24", 
"Not available", "Not available", "Not available", "Not available", "Not available", 
"Not available", "Not available", "Not available", "Numlock", "Scroll lock", 
"Not available", "Not available", "Not available", "Not available", 
"Not available", "Not available", "Not available", "Not available", "Not available", 
"Not available", "Not available", "Not available", "Not available", "Not available", 
"Left Shift", "Right Shift", "Left Control", "Right Control", 
"Left Menu", "Right Menu", "Browser back", "Browser forward", "Browser refresh"};

GMenu::GMenu(char* szTitelName, int x, int y, int w, bool bOnHighlighting, bool bShowValue)
{
	iElements = 0;
	iSelected = 0;
	this->x = x;
	this->y = y;
	this->w = w;
	this->bOnHighlighting = bOnHighlighting;
	this->bShowValue = bShowValue;
	this->bMenuActive = false;
	bActive = true;
	strcpy(szTitel, szTitelName);
	memset(&subitem, 0, sizeof(subitem));
	window = NULL;
}

GMenu::~GMenu()
{
	iElements = 0;
	iSelected = 0;
}

void GMenu::AddEntry(char* szName, int* pValue, int iMin, int iMax, int iStep, char* desc)
{
	strcpy(list[iElements].szName, szName);
	list[iElements].pValue = pValue;
	list[iElements].iMin = iMin;
	list[iElements].iMax = iMax;
	list[iElements].iStep = iStep;
	if(desc) {
		strcpy(list[iElements].szDescription, desc);
	}

	++iElements;
}

bool GMenu::KeyHandling(int key)
{
	if (key == VK_INSERT) {
		bMenuActive = !bMenuActive;
	}

	if(!bMenuActive) 
		return false;

	if (key == VK_UP || (!window && (GetAsyncKeyState(VK_UP) & 1)))
	{
		if (iSelected <= 0) iSelected = iElements - 1;
		else iSelected--;
		return true;
	}
	if (key == VK_DOWN || (!window && (GetAsyncKeyState(VK_DOWN) & 1)))
	{
		if (iSelected >= (iElements - 1)) iSelected = 0;
		else iSelected++;
		return true;
	}
	if ((key == VK_LEFT || key == VK_RBUTTON) || (!window && (GetAsyncKeyState(VK_LEFT) & 1)))
	{
		if (!IsBadReadPtr((void*)list[iSelected].pValue, 4)) {
			*list[iSelected].pValue -= list[iSelected].iStep;
			if (*list[iSelected].pValue < list[iSelected].iMin) *list[iSelected].pValue = list[iSelected].iMax;
			return true;
		}
	}
	if ((key == VK_RIGHT || key == VK_LBUTTON) || (!window && (GetAsyncKeyState(VK_RIGHT) & 1)))
	{
		if (!IsBadReadPtr((void*)list[iSelected].pValue, 4)) {
			*list[iSelected].pValue += list[iSelected].iStep;
			if (*list[iSelected].pValue > list[iSelected].iMax) *list[iSelected].pValue = list[iSelected].iMin;
			return true;
		}
	}

	return false;
}

void GMenu::Draw()
{
	textsize size;
	g_pOverlay->DirectX->Draw->GetTextLength("A", &size);
	size.h += 2;

	g_pOverlay->DirectX->Draw->FilledQuad(x + 2, y + 15, w, size.h * iElements + 40, GColor(0, 0, 0, 180));
	g_pOverlay->DirectX->Draw->Quad(x + 2, y + 15, w, size.h * iElements + 40, 2, color_white);
	g_pOverlay->DirectX->Draw->FilledQuad(x + 2, y + 40, w, 2, color_white);
	g_pOverlay->DirectX->Draw->TextCentered(x + w * 0.5, y + 22, color_green, szTitel);

	if(list[iSelected].szDescription && strlen(list[iSelected].szDescription)) {
		g_pOverlay->DirectX->Draw->FilledQuad(0, cvars.Screen.y - 20, cvars.Screen.x, 20, GColor(0, 0, 0, 180));
		g_pOverlay->DirectX->Draw->FilledQuad(0, cvars.Screen.y - 20, cvars.Screen.x, 1, color_white);
		g_pOverlay->DirectX->Draw->TextCentered(cvars.Screen.x / 2, cvars.Screen.y - 15, color_white, list[iSelected].szDescription);
	}

	for(int i = 0; i < iElements; i++)	{

		if(i != iSelected) {

				if(!IsBadToRead((void*)list[iSelected].pValue, 4)) {

					g_pOverlay->DirectX->Draw->Text(x + 10, y + 46 + (size.h * i), color_white, list[i].szName);
					if (bShowValue && list[i].szName[0] != '-') 
						g_pOverlay->DirectX->Draw->TextFlags(x + 125, y + 46 + (size.h * i), color_white, FW1_RIGHT, /*%d*/XorStr<0x5d, 3, 0x8dbfc15b>("\x78\x3a" + 0x8dbfc15b).s, *list[i].pValue);
				}
		}
		else {
			if(!IsBadToRead((void*)list[iSelected].pValue, 4)) {

				g_pOverlay->DirectX->Draw->Text(x + 10, y + 46 + (size.h * i), color_green, list[i].szName);

				if(bShowValue && list[i].szName[0] != '-')  {
					
					g_pOverlay->DirectX->Draw->TextFlags(x + 125, y + 46 + (size.h * i), color_green, FW1_RIGHT, /*%d*/XorStr<0xe4, 3, 0xe047e17d>("\xc1\x81" + 0xe047e17d).s, *list[i].pValue);

					if(list[i].iMax == 255) {

						if(*list[i].pValue <= 0xA8 && strlen(keylist[*list[i].pValue])) {

							g_pOverlay->DirectX->Draw->FilledQuad(x + 137 + 17, y + 44 + (size.h * i), 120, 17, color_black);
							g_pOverlay->DirectX->Draw->Quad(x + 137 + 17, y + 44 + (size.h * i), 120, 17, 1, color_white);
							g_pOverlay->DirectX->Draw->Quad(x + w + 5, y + 44 + (size.h * i) + 8, 12, 1, 1, color_white);
							g_pOverlay->DirectX->Draw->TextCentered(x + 137 + 77, y + 46 + (size.h * i), color_green, keylist[*list[i].pValue]);
						}
					}
				}
			}
		}
	}
}

void GMenu::Do()
{
	if (!window && (g_pInput->IsKeyDown(VK_INSERT) &1))
		bMenuActive = !bMenuActive;

	if (bMenuActive)
	{
		Draw();
		KeyHandling(0);

		iElements = 0;

		AddEntry(/*-Aimbot*/XorStr<0x4A, 8, 0xF1C010E2>("\x67\x0A\x25\x20\x2C\x20\x24" + 0xF1C010E2).s, &cvars._AIM, 0, 1, 1);
		if (cvars._AIM) {
			AddEntry(/*  Toggle*/XorStr<0x55, 9, 0xD09A59EF>("\x75\x76\x03\x37\x3E\x3D\x37\x39" + 0xD09A59EF).s, &cvars.AIM_On, 0, 1, 1, /*Enable/Disable the whole bot*/XorStr<0xB7, 29, 0x90EB9103>("\xF2\xD6\xD8\xD8\xD7\xD9\x92\xFA\xD6\xB3\xA0\xA0\xAF\xA1\xE5\xB2\xAF\xAD\xE9\xBD\xA3\xA3\xA1\xAB\xEF\xB2\xBE\xA6" + 0x90EB9103).s);
			AddEntry(/*  Player*/XorStr<0x71, 9, 0xBF218DEF>("\x51\x52\x23\x18\x14\x0F\x12\x0A" + 0xBF218DEF).s, &cvars.AIM_Player, 0, 1, 1, /*Aim at players*/XorStr<0x20, 15, 0x7443C039>("\x61\x48\x4F\x03\x45\x51\x06\x57\x44\x48\x53\x4E\x5E\x5E" + 0x7443C039).s);
			AddEntry(/*  Zombie*/XorStr<0x33, 9, 0xB62F8F84>("\x13\x14\x6F\x59\x5A\x5A\x50\x5F" + 0xB62F8F84).s, &cvars.AIM_Zombie, 0, 1, 1, /*Aim at zombies*/XorStr<0x34, 15, 0x3C6859D7>("\x75\x5C\x5B\x17\x59\x4D\x1A\x41\x53\x50\x5C\x56\x25\x32" + 0x3C6859D7).s);
			AddEntry(/*  Animals*/XorStr<0xcd, 10, 0xcaff9a0>("\xed\xee\x8e\xbe\xb8\xbf\xb2\xb8\xa6" + 0xcaff9a0).s, &cvars.AIM_Animals, 0, 1, 1);
			AddEntry(/*  Friends*/XorStr<0x26, 10, 0x8155C011>("\x06\x07\x6E\x5B\x43\x4E\x42\x49\x5D" + 0x8155C011).s, &cvars.AIM_Friends, 0, 1, 1, /*Should friends be excepted?*/XorStr<0xA2, 28, 0x8EB221C2>("\xF1\xCB\xCB\xD0\xCA\xC3\x88\xCF\xD8\xC2\xC9\xC3\xCA\xDC\x90\xD3\xD7\x93\xD1\xCD\xD5\xD2\xC8\xCD\xDF\xDF\x83" + 0x8EB221C2).s);
			AddEntry(/*  Mode*/XorStr<0x3C, 7, 0x7335E1FD>("\x1C\x1D\x73\x50\x24\x24" + 0x7335E1FD).s, &cvars.AIM_Mode, 0, 2, 1, /*0 = by distance; 1 = by crosshair; 2 = by health*/XorStr<0xDD, 49, 0x47308D75>("\xED\xFE\xE2\xC0\x83\x9B\xC3\x80\x8C\x95\x93\x89\x87\x89\x8E\xD7\xCD\xDF\xCF\xCD\xD1\x90\x8A\xD4\x96\x84\x98\x8B\x8A\x92\x9A\x95\x8F\xC5\xDF\x32\x21\x3F\x23\x66\x7C\x26\x6F\x6D\x68\x66\x7F\x64" + 0x47308D75).s);
			AddEntry(/*  Key*/XorStr<0x6D, 6, 0x10B6B8D5>("\x4D\x4E\x24\x15\x08" + 0x10B6B8D5).s, &cvars.AIM_Key, 0, 255, 1, /*Defines the key that enables the bot*/XorStr<0xC2, 37, 0xC6C25473>("\x86\xA6\xA2\xAC\xA8\xA2\xBB\xE9\xBE\xA3\xA9\xED\xA5\xAA\xA9\xF1\xA6\xBB\xB5\xA1\xF6\xB2\xB6\xB8\xB8\xB7\xB9\xAE\xFE\xAB\x88\x84\xC2\x81\x8B\x91" + 0xC6C25473).s);
			AddEntry(/*  Position*/XorStr<0x56, 11, 0xB627067B>("\x76\x77\x08\x36\x29\x32\x28\x34\x31\x31" + 0xB627067B).s, &cvars.AIM_Pos, 0, 2, 1, /*0 = head; 1 = body; 2 = foot*/XorStr<0x8F, 29, 0x9F2DBCF1>("\xBF\xB0\xAC\xB2\xFB\xF1\xF4\xF2\xAC\xB8\xA8\xBA\xA6\xBC\xFF\xF1\xFB\xD9\x9A\x82\x91\x84\x98\x86\xC1\xC7\xC6\xDE" + 0x9F2DBCF1).s);
			AddEntry(/*  Lock*/XorStr<0x92, 7, 0xFE675960>("\xB2\xB3\xD8\xFA\xF5\xFC" + 0xFE675960).s, &cvars.AIM_Lock, 0, 1, 1);
			AddEntry(/*  Telekill*/XorStr<0x14, 11, 0x40114344>("\x34\x35\x42\x72\x74\x7c\x71\x72\x70\x71" + 0x40114344).s, &cvars.AIM_Telekill, 0, 1, 1);
		}
		AddEntry(/*-Player ESP*/XorStr<0xA0, 12, 0x9C56C571>("\x8D\xF1\xCE\xC2\xDD\xC0\xD4\x87\xED\xFA\xFA" + 0x9C56C571).s, &cvars._ESP_PLAYER, 0, 1, 1);
		if (cvars._ESP_PLAYER) {
			AddEntry(/*  Player*/XorStr<0x2C, 9, 0x70436067>("\x0C\x0D\x7E\x43\x51\x48\x57\x41" + 0x70436067).s, &cvars.ESP_Player, 0, 1, 1, /*Show players with a LIGHTBLUE crosscircle/name*/XorStr<0x34, 47, 0xDA832150>("\x67\x5D\x59\x40\x18\x49\x56\x5A\x45\x58\x4C\x4C\x60\x36\x2B\x37\x2C\x65\x27\x67\x04\x00\x0D\x03\x18\x0F\x02\x1A\x15\x71\x31\x21\x3B\x26\x25\x34\x31\x2B\x39\x37\x39\x72\x30\x3E\x0D\x04" + 0xDA832150).s);
			AddEntry(/*  Zombie*/XorStr<0x3F, 9, 0x93BC1450>("\x1F\x60\x1B\x2D\x2E\x26\x2C\x23" + 0x93BC1450).s, &cvars.ESP_Zombie, 0, 1, 1, /*Show zombies with a MAROONRED crosscircle/name*/XorStr<0x2A, 47, 0x58EF0081>("\x79\x43\x43\x5A\x0E\x55\x5F\x5C\x50\x5A\x51\x46\x16\x40\x51\x4D\x52\x1B\x5D\x1D\x73\x7E\x12\x0E\x0D\x0D\x16\x00\x02\x67\x2B\x3B\x25\x38\x3F\x2E\x27\x3D\x33\x3D\x37\x7C\x3A\x34\x3B\x32" + 0x58EF0081).s);
			AddEntry(/*  Name*/XorStr<0x27, 7, 0xA35E3C0A>("\x07\x08\x67\x4B\x46\x49" + 0xA35E3C0A).s, &cvars.ESP_Name, 0, 1, 1, /*Show player names*/XorStr<0x4C, 18, 0x6DB19F7D>("\x1F\x25\x21\x38\x70\x21\x3E\x32\x2D\x30\x24\x77\x36\x38\x37\x3E\x2F" + 0x6DB19F7D).s);
			AddEntry(/*  Box*/XorStr<0x7e, 6, 0xdf1516af>("\x5e\x5f\xc2\xee\xfa" + 0xdf1516af).s, &cvars.ESP_Box, 0, 1, 1, /*Show a box around the player*/XorStr<0x50, 29, 0x82e9b483>("\x3\x39\x3d\x24\x74\x34\x76\x35\x37\x21\x7a\x3a\x2e\x32\x2b\x31\x4\x41\x16\xb\x1\x45\x16\xb\x9\x10\xf\x19" + 0x82e9b483).s);
			AddEntry(/*  Dot*/XorStr<0x56, 6, 0xE1707A5E>("\x76\x77\x1C\x36\x2E" + 0xE1707A5E).s, &cvars.ESP_Dot, 0, 1, 1, /*Show a litte dot on the head*/XorStr<0x33, 29, 0x675F83EE>("\x60\x5C\x5A\x41\x17\x59\x19\x56\x52\x48\x49\x5B\x1F\x24\x2E\x36\x63\x2B\x2B\x66\x33\x20\x2C\x6A\x23\x29\x2C\x2A" + 0x675F83EE).s);
			AddEntry(/*  Health*/XorStr<0xE8, 9, 0x0364F7B7>("\xC8\xC9\xA2\x8E\x8D\x81\x9A\x87" + 0x0364F7B7).s, &cvars.ESP_Health, 0, 1, 1, /*Show current health and blood of players*/XorStr<0x15, 41, 0x49268191>("\x46\x7E\x78\x6F\x39\x79\x6E\x6E\x6F\x7B\x71\x54\x01\x4A\x46\x45\x49\x52\x4F\x08\x48\x44\x4F\x0C\x4F\x42\x40\x5F\x55\x12\x5C\x52\x15\x46\x5B\x59\x40\x5F\x49\x4F" + 0x49268191).s);
			AddEntry(/*  Weapon*/XorStr<0x6F, 9, 0xFC9417BE>("\x4F\x50\x26\x17\x12\x04\x1A\x18" + 0xFC9417BE).s, &cvars.ESP_Weapon, 0, 1, 1, /*Show players currently equipped weapon*/XorStr<0xC3, 39, 0xBC225532>("\x90\xAC\xAA\xB1\xE7\xB8\xA5\xAB\xB2\xA9\xBF\xBD\xEF\xB3\xA4\xA0\xA1\xB1\xBB\xA2\xBB\xA1\xF9\xBF\xAA\xA9\xB4\xAE\xAF\x85\x85\xC2\x94\x81\x84\x96\x88\x86" + 0xBC225532).s);
			AddEntry(/*  State*/XorStr<0xda, 8, 0xd63cd773>("\xfa\xfb\x8f\xa9\xbf\xab\x85" + 0xd63cd773).s, &cvars.ESP_State, 0, 1, 1);
			AddEntry(/*  Inventar*/XorStr<0xad, 11, 0x7510ac13>("\x8d\x8e\xe6\xde\xc7\xd7\xdd\xc0\xd4\xc4" + 0x7510ac13).s, &cvars.ESP_Inventar, 0, 1, 1);
			AddEntry(/*  Viewpoint*/XorStr<0xD0, 12, 0x3EFDD96A>("\xF0\xF1\x84\xBA\xB1\xA2\xA6\xB8\xB1\xB7\xAE" + 0x3EFDD96A).s, &cvars.ESP_View, 0, 1, 1, /*Show a line where a player/zombie is looking at*/XorStr<0xD1, 48, 0x056F2D29>("\x82\xBA\xBC\xA3\xF5\xB7\xF7\xB4\xB0\xB4\xBE\xFC\xAA\xB6\xBA\x92\x84\xC2\x82\xC4\x95\x8A\x86\x91\x8C\x98\xC4\x96\x82\x83\x8D\x99\x94\xD2\x9A\x87\xD5\x9A\x98\x97\x92\x93\x95\x9B\xDD\x9F\x8B" + 0x056F2D29).s);
			AddEntry(/*  Radar*/XorStr<0xD5, 8, 0xCA2266DC>("\xF5\xF6\x85\xB9\xBD\xBB\xA9" + 0xCA2266DC).s, &cvars.ESP_Radar, 0, 1, 1, /*Show a radar with player/zombie/items marked on it*/XorStr<0x26, 51, 0xFF7BF95C>("\x75\x4F\x47\x5E\x0A\x4A\x0C\x5F\x4F\x4B\x51\x43\x12\x44\x5D\x41\x5E\x17\x48\x55\x5B\x42\x59\x4F\x11\x45\x2F\x2C\x20\x2A\x21\x6A\x2F\x33\x2D\x24\x39\x6B\x21\x2C\x3C\x24\x35\x35\x72\x3C\x3A\x75\x3F\x23" + 0xFF7BF95C).s);
			AddEntry(/*  Range*/XorStr<0x23, 8, 0xE13A20B0>("\x03\x04\x77\x47\x49\x4F\x4C" + 0xE13A20B0).s, &cvars.radar_range, 0, 10000, 100, /*Sets the radar range*/XorStr<0x31, 21, 0x1FC28BF1>("\x62\x57\x47\x47\x15\x42\x5F\x5D\x19\x48\x5A\x58\x5C\x4C\x1F\x32\x20\x2C\x24\x21" + 0x1FC28BF1).s);
			AddEntry(/*  Dead*/XorStr<0x11, 7, 0xd873a944>("\x31\x32\x57\x71\x74\x72" + 0xd873a944).s, &cvars.ESP_ShowDead, 0, 1, 1, /*Dont filter dead players*/XorStr<0x41, 25, 0x6f3fa672>("\x5\x2d\x2d\x30\x65\x20\x2e\x24\x3d\x2f\x39\x6c\x29\x2b\x2e\x34\x71\x22\x3f\x35\x2c\x33\x25\x2b" + 0x6f3fa672).s);
		}
		AddEntry(/*-Item ESP*/XorStr<0xCA, 10, 0xB37B96BE>("\xE7\x82\xB8\xA8\xA3\xEF\x95\x82\x82" + 0xB37B96BE).s, &cvars._ESP_ITEM, 0, 1, 1);
		if (cvars._ESP_ITEM) {
			AddEntry(/*  Clothing*/XorStr<0x50, 11, 0xF07ACE23>("\x70\x71\x11\x3F\x3B\x21\x3E\x3E\x36\x3E" + 0xF07ACE23).s, &cvars.ESP_Clothing, 0, 1, 1);
			AddEntry(/*  House*/XorStr<0x91, 8, 0x2FB7E3EC>("\xB1\xB2\xDB\xFB\xE0\xE5\xF2" + 0x2FB7E3EC).s, &cvars.ESP_House, 0, 1, 1);
			AddEntry(/*  Inventory*/XorStr<0x2C, 12, 0x613556BB>("\x0C\x0D\x67\x41\x46\x54\x5C\x47\x5B\x47\x4F" + 0x613556BB).s, &cvars.ESP_Inventory, 0, 1, 1);
			AddEntry(/*  Magazine*/XorStr<0x48, 11, 0xF2EADF6B>("\x68\x69\x07\x2A\x2B\x2C\x34\x26\x3E\x34" + 0xF2EADF6B).s, &cvars.ESP_Magazine, 0, 1, 1);
			AddEntry(/*  Weapon*/XorStr<0x62, 9, 0x17840AFC>("\x42\x43\x33\x00\x07\x17\x07\x07" + 0x17840AFC).s, &cvars.ESP_Weapons, 0, 1, 1);
			AddEntry(/*  Other*/XorStr<0x44, 8, 0x50752592>("\x64\x65\x09\x33\x20\x2C\x38" + 0x50752592).s, &cvars.ESP_Other, 0, 1, 1);
			AddEntry(/*  Uncatched*/XorStr<0xEC, 12, 0x40F0A320>("\xCC\xCD\xBB\x81\x93\x90\x86\x90\x9C\x90\x92" + 0x40F0A320).s, &cvars.ESP_Unfiltered, 0, 1, 1);
			AddEntry(/*  Ruined*/XorStr<0xca, 9, 0x5fc69363>("\xea\xeb\x9e\xb8\xa7\xa1\xb5\xb5" + 0x5fc69363).s, &cvars.ESP_Ruined, 0, 1, 1);
			AddEntry(/*  Teleport*/XorStr<0x31, 11, 0xc3f99461>("\x11\x12\x67\x51\x59\x53\x47\x57\x4b\x4e" + 0xc3f99461).s, &cvars.ESP_Teleport, 0, 7, 1, /*teleport items; press NUMPAD1 to activate; 1=weapons 2=mags, 3=ammo, 4=food 5=medical 6=tools 7=clothing*/XorStr<0x8c, 105, 0x58be8bc1>("\xf8\xe8\xe2\xea\xe0\xfe\xe0\xe7\xb4\xfc\xe2\xf2\xf5\xea\xa1\xbb\xec\xef\xfb\xec\xd3\x81\xec\xf6\xe9\xf5\xe7\xe3\x99\x89\xde\xc4\x8c\xcc\xcd\xdb\xd9\xc7\xd3\xc7\xd1\x8e\x96\x86\x85\xce\xdf\xda\xcc\xd2\xd0\xcc\xe0\xf3\xff\xae\xa5\xa2\xb5\xeb\xe8\xfa\xf7\xaa\xa1\xa0\xa1\xe3\xf0\xe5\xef\xb5\xbb\xba\xb2\xf7\xed\xe4\xb7\xbe\xb8\xb4\xbd\xbe\x8c\xc1\xd4\xde\x90\x8a\x89\x8b\x9b\xc9\xdd\xd6\x8f\x81\x81\x9b\x98\x98\x9c\x94" + 0x58be8bc1).s);
		}
		AddEntry(/*-Other ESP*/XorStr<0x5f, 11, 0x268c27c3>("\x72\x2f\x15\xa\x6\x16\x45\x23\x34\x38" + 0x268c27c3).s, &cvars._ESP_OTHER, 0, 1, 1);
		if (cvars._ESP_OTHER) {
			AddEntry(/*  City*/XorStr<0x92, 7, 0x9DDF1D2F>("\xB2\xB3\xD7\xFC\xE2\xEE" + 0x9DDF1D2F).s, &cvars.ESP_City, 0, 1, 1, /*Show cities marked in PINK*/XorStr<0x84, 27, 0xC634DF3C>("\xD7\xED\xE9\xF0\xA8\xEA\xE3\xFF\xE5\xE8\xFD\xAF\xFD\xF0\xE0\xF8\xF1\xF1\xB6\xFE\xF6\xB9\xCA\xD2\xD2\xD6" + 0xC634DF3C).s);
			AddEntry(/*  Animals*/XorStr<0xcd, 10, 0xcaff9a0>("\xed\xee\x8e\xbe\xb8\xbf\xb2\xb8\xa6" + 0xcaff9a0).s, &cvars.ESP_Animals, 0, 1, 1, /*Show animals*/XorStr<0x42, 13, 0x3ea540d>("\x11\x2b\x2b\x32\x66\x26\x26\x20\x27\x2a\x20\x3e" + 0x3ea540d).s);
			AddEntry(/*  Effects*/XorStr<0x1b, 10, 0x17b31ab5>("\x3b\x3c\x58\x78\x79\x45\x42\x56\x50" + 0x17b31ab5).s, &cvars.ESP_Effects, 0, 1, 1, /*Show birds and insects*/XorStr<0x26, 23, 0x5522f259>("\x75\x4f\x47\x5e\xa\x49\x45\x5f\x4a\x5c\x10\x50\x5c\x57\x14\x5c\x58\x44\x5d\x5a\x4e\x48" + 0x5522f259).s);
			AddEntry(/*  Far*/XorStr<0xda, 6, 0xa06ea33e>("\xfa\xfb\x9a\xbc\xac" + 0xa06ea33e).s, &cvars.ESP_Far, 0, 1, 1);
		}
		AddEntry(/*-D3D*/XorStr<0x61, 5, 0x92197F2E>("\x4C\x26\x50\x20" + 0x92197F2E).s, &cvars._D3D, 0, 1, 1);
		if (cvars._D3D) {
			AddEntry(/*  Wallhack*/XorStr<0x70, 11, 0xCA9FB569>("\x50\x51\x25\x12\x18\x19\x1E\x16\x1B\x12" + 0xCA9FB569).s, &cvars.D3D_Wallhack, 0, 1, 1);
			AddEntry(/*  Wireframe*/XorStr<0xA7, 12, 0x139BE203>("\x87\x88\xFE\xC3\xD9\xC9\xCB\xDC\xCE\xDD\xD4" + 0x139BE203).s, &cvars.D3D_Wireframe, 0, 2, 1);
			//pMenu->AddEntry("  Fullbright", &cvars.D3D_Fullbright, 0, 1, 1);
			//pMenu->AddEntry("  Nofog", &cvars.D3D_Nofog, 0, 1, 1);
			//pMenu->AddEntry("  Stride", &cvars.D3D_Stride, 0, 5000, 1);
			/*pMenu->AddEntry("  NumVertices", &cvars.D3D_NumVertices, 0, 5000, 1);
			pMenu->AddEntry("  PrimCount", &cvars.D3D_PrimCount, 0, 5000, 1);*/
		}
		AddEntry(/*-Misc*/XorStr<0x77, 6, 0x869429D7>("\x5A\x35\x10\x09\x18" + 0x869429D7).s, &cvars._MISC, 0, 1, 1);
		if (cvars._MISC) {
			AddEntry(/*  Fullscreen*/XorStr<0x9F, 13, 0x7046755A>("\xBF\x80\xE7\xD7\xCF\xC8\xD6\xC5\xD5\xCD\xCC\xC4" + 0x7046755A).s, &cvars.MISC_Fullscreen, 0, 1, 1, /*Force the game into fake fullscreen mode*/XorStr<0x2E, 41, 0xFC31A938>("\x68\x40\x42\x52\x57\x13\x40\x5D\x53\x17\x5F\x58\x57\x5E\x1C\x54\x50\x4B\x2F\x61\x24\x22\x2F\x20\x66\x21\x3D\x25\x26\x38\x2F\x3F\x2B\x2A\x3E\x71\x3F\x3C\x30\x30" + 0xFC31A938).s);
			AddEntry(/*  Crosshair*/XorStr<0xDF, 12, 0xFA8417FB>("\xFF\xC0\xA2\x90\x8C\x97\x96\x8E\x86\x81\x9B" + 0xFA8417FB).s, &cvars.MISC_Crosshair, 0, 5, 1, /*Choose one from different crosshairs*/XorStr<0x71, 37, 0xA16B7DAC>("\x32\x1A\x1C\x1B\x06\x13\x57\x17\x17\x1F\x5B\x1A\x0F\x11\x12\xA0\xE5\xEB\xE5\xE2\xE0\xF4\xE2\xE6\xFD\xAA\xE8\xFE\xE2\xFD\xFC\xF8\xF0\xFB\xE1\xE7" + 0xA16B7DAC).s);
			AddEntry(/*  Healthbars*/XorStr<0xEA, 13, 0xC5ECD084>("\xCA\xCB\xA4\x88\x8F\x83\x84\x99\x90\x92\x86\x86" + 0xC5ECD084).s, &cvars.MISC_Healthbars, 0, 1, 1, /*Show health and blood bars*/XorStr<0xB1, 27, 0x9AAA6EE9>("\xE2\xDA\xDC\xC3\x95\xDE\xD2\xD9\xD5\xCE\xD3\x9C\xDC\xD0\xDB\xE0\xA3\xAE\xAC\xAB\xA1\xE6\xA5\xA9\xBB\xB9" + 0x9AAA6EE9).s);
			AddEntry(/*  Zombie Alert*/XorStr<0x90, 15, 0x5F6A83C4>("\xB0\xB1\xC8\xFC\xF9\xF7\xFF\xF2\xB8\xD8\xF6\xFE\xEE\xE9" + 0x5F6A83C4).s, &cvars.MISC_ZombieWarning, 0, 1, 1, /*Show a text message to warn about near zombies*/XorStr<0x68, 47, 0xFE9B882F>("\x3B\x01\x05\x1C\x4C\x0C\x4E\x1B\x15\x09\x06\x53\x19\x10\x05\x04\x19\x1E\x1F\x5B\x08\x12\x5E\x08\xE1\xF3\xEC\xA3\xE5\xE7\xE9\xF2\xFC\xA9\xE4\xEE\xED\xFF\xAE\xF5\xFF\xFC\xF0\xFA\xF1\xE6" + 0xFE9B882F).s);
			AddEntry(/*  Statsbox*/XorStr<0xFE, 11, 0x16DFBE71>("\xDE\xDF\x53\x75\x63\x77\x77\x67\x69\x7F" + 0x16DFBE71).s, &cvars.MISC_Statsbox, 0, 1, 1, /*Show some info*/XorStr<0xA4, 15, 0x8E39B863>("\xF7\xCD\xC9\xD0\x88\xDA\xC5\xC6\xC9\x8D\xC7\xC1\xD6\xDE" + 0x8E39B863).s);
			AddEntry(/*  Glitcher*/XorStr<0x34, 11, 0x65728FC5>("\x14\x15\x71\x5B\x51\x4D\x59\x53\x59\x4F" + 0x65728FC5).s, &cvars.MISC_Glitcher, 0, 500, 2, /*Move into view direction. Key = SPACE*/XorStr<0xA2, 38, 0xC5EDFF8D>("\xEF\xCC\xD2\xC0\x86\xCE\xC6\xDD\xC5\x8B\xDA\xC4\xCB\xD8\x90\xD5\xDB\xC1\xD1\xD6\xC2\xDE\xD7\xD7\x94\x9B\xF7\xD8\xC7\x9F\xFD\xE1\x91\x93\x85\x86\x83" + 0xC5EDFF8D).s);
			AddEntry(/*  Unl Ammo*/XorStr<0xbb, 11, 0x51ecb9ba>("\x9b\x9c\xe8\xd0\xd3\xe0\x80\xaf\xae\xab" + 0x51ecb9ba).s, &cvars.MISC_UnlimitedAmmo, 0, 1, 1);
			AddEntry(/*  Removals*/XorStr<0x63, 11, 0x2bc662c9>("\x43\x44\x37\x3\xa\x7\x1f\xb\x7\x1f" + 0x2bc662c9).s, &cvars.MISC_Removals, 0, 1, 1, /*Norecoil and Nofatigue*/XorStr<0xe3, 23, 0xe07be1ad>("\xad\x8b\x97\x83\x84\x87\x80\x86\xcb\x8d\x83\x8a\xcf\xbe\x9e\x94\x92\x80\x9c\x91\x82\x9d" + 0xe07be1ad).s);
			AddEntry(/*  Swim*/XorStr<0x14, 7, 0x45ab1213>("\x34\x35\x45\x60\x71\x74" + 0x45ab1213).s, &cvars.MISC_Swim, 0, 1, 1, /*ALT key to active*/XorStr<0xf1, 18, 0x53208656>("\xb0\xbe\xa7\xd4\x9e\x93\x8e\xd8\x8d\x95\xdb\x9d\x9e\x8a\x96\x76\x64" + 0x53208656).s);
			//pMenu->AddEntry("  Hover", &cvars.MISC_Hover, 0, 1, 1, "Move into view direction. Key = SPACE");
			AddEntry(/*  Worldtime*/XorStr<0xb8, 12, 0x4ae61c1d>("\x98\x99\xed\xd4\xce\xd1\xda\xcb\xa9\xac\xa7" + 0x4ae61c1d).s, &cvars.MISC_Worldtime, 0, 50, 1);
		}
	}
}