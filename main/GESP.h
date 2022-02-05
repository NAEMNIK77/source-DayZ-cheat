#ifndef __GESP__
#define __GESP__

#include "GSDK.h"
#include "GTools.h"

class GESP {
public:
	void Render();

	void DoPlayer(int i);
	void DoCity();
	void DoItem(CObject *obj2);
	void DoTest();
	void DoEffects(CObject *obj);
	void DoAnimals(CObject *obj);
	void DoZombieWarning();
	void DoPlayerInventory(CObject *obj);
	void Reset();

	void CollectWeapon(CObject *obj);
	void DoWeaponSteal();

	char* FormatItemName(CObject *obj, GColor &color);
	void CalcRadarPoint(GVector object_pos, GVector local_pos, float local_yaw, GVector &out);

	int count_zombies;
	int count_items;
	int count_players;
	float cur_speed;

	int inventory_box_x;

	bool zombie_near;
	bool zombie_closer;
	bool zombie_attention;
	bool zombie_moving;

	std::list<CObject*>list_items;
	std::list<CObject*>list_animals;
	std::list<CObject*>list_effects;

	std::list<CObject*>list_bufitems;

	CObject *nearest_obj;
	float obj_dist;
private:
};

struct city_pos_t {
	GVector pos;
	char name[64];
	int seeing_dist;
};

const int CITY_FAR_DIST = 8000;
const int CITY_MID_DIST = 2500;
const int CITY_NEAR_DIST = 200;

//Berezino --> City
//[Berezino] --> Schild/Wegweiser
const city_pos_t city_pos[] = {
	{ GVector(12298.22, 33.98, 9073.13), "Berezino", CITY_FAR_DIST },
	{ GVector(9812.10, 289.62, 8803.69), "Gorka", CITY_FAR_DIST },
	{ GVector(12153.61, 6.16, 3498.92), "Kamyshovo", CITY_FAR_DIST },
	{ GVector(10443.31, 5.99, 2369.12), "Elektrozavodsk", CITY_FAR_DIST },
	{ GVector(13466.44, 5.65, 6291.06), "Solnichniy", CITY_FAR_DIST },
	{ GVector(14127.37, 3.54, 13072.47), "Svetlojarsk", CITY_FAR_DIST },
	{ GVector(11205.41, 195.51, 12178.72), "Krasnostav", CITY_FAR_DIST },
	{ GVector(6253.72, 297.98, 7628.25), "Stary Sobor", CITY_FAR_DIST },
	{ GVector(6930.95, 6.00, 2425.37), "Chernogorsk", CITY_FAR_DIST },
	{ GVector(2553.79, 201.34, 5026.84), "Zelenogorsk", CITY_FAR_DIST },
	{ GVector(3840.24, 310.89, 8850.63), "Vybor", CITY_FAR_DIST },
	{ GVector(4095.27, 337.75, 11079.59), "Military", CITY_FAR_DIST },
	{ GVector(4813.76, 6.00, 2235.44), "Military Balota", CITY_FAR_DIST },
	{ GVector(12351.84, 130.43, 14883.50), "Karmanovka", CITY_FAR_DIST },

	{ GVector(2148.15, 91.45, 3374.93), "Military Pavlovo", CITY_MID_DIST },
	{ GVector(12301.95, 104.52, 10874.33), "Khelm", CITY_MID_DIST },
	{ GVector(12407.31, 73.03, 13841.02), "Cernaya Polana", CITY_MID_DIST },
	{ GVector(13327.20, 96.03, 12949.96), "Olsha", CITY_MID_DIST },
	{ GVector(12920.60, 6.24, 8352.18), "Nizhnoye", CITY_MID_DIST },
	{ GVector(12837.87, 173.97, 4459.08), "Tulga", CITY_MID_DIST },
	{ GVector(11207.40, 76.61, 6549.15), "Dolina", CITY_MID_DIST },
	{ GVector(10665.08, 210.90, 8014.60), "Polana", CITY_MID_DIST },
	{ GVector(10137.69, 245.03, 5451.83), "Staroye", CITY_MID_DIST },
	{ GVector(7138.51, 294.04, 7697.73), "Novy Sobor", CITY_MID_DIST },
	{ GVector(9634.43, 210.82, 6539.17), "Shakhovka", CITY_MID_DIST },
	{ GVector(8514.48, 358.24, 6680.43), "Guglovo", CITY_MID_DIST },
	{ GVector(10322.37, 161.86, 9574.92), "Dubrovka", CITY_MID_DIST },
	{ GVector(1712.82, 149.43, 4021.38), "Pavlovo", CITY_MID_DIST },
	{ GVector(3446.00, 245.89, 4953.20), "Drozhino", CITY_MID_DIST },
	{ GVector(4355.39, 235.71, 4703.60), "Kozlovka", CITY_MID_DIST },
	{ GVector(7968.82, 6.00, 3262.85), "Prigorodki", CITY_MID_DIST },
	{ GVector(6524.40, 61.27, 3579.77), "Dubky", CITY_MID_DIST },
	{ GVector(6203.96, 39.81, 3536.44), "Novoselky", CITY_MID_DIST },
	{ GVector(4471.17, 3.45, 2383.34), "Balota", CITY_MID_DIST },
	{ GVector(5896.99, 271.71, 10225.20), "Grishino", CITY_MID_DIST },
	{ GVector(6583.58, 321.70, 6021.16), "Vyshnoye", CITY_MID_DIST },
	{ GVector(8593.03, 196.91, 11899.16), "Gvozdno", CITY_MID_DIST },
	{ GVector(5368.86, 331.48, 8566.82), "Kabanino", CITY_MID_DIST },
	{ GVector(2697.89, 273.98, 9988.09), "Lopatina", CITY_MID_DIST },
	{ GVector(5615.69, 68.47, 2641.67), "Chapaevsk", CITY_MID_DIST },
	{ GVector(3569.97, 6.48, 2450.10), "Komarovo", CITY_MID_DIST },
	{ GVector(7545.32, 213.66, 5079.47), "Mogilevka", CITY_MID_DIST },
	{ GVector(5864.15, 138.69, 4648.83), "Nadezhdino", CITY_MID_DIST },
	{ GVector(3132.97, 302.89, 8068.98), "Pustoshka", CITY_MID_DIST },
	{ GVector(2011.14, 253.43, 7328.83), "Myshkino", CITY_MID_DIST },
	{ GVector(2524.99, 305.73, 6409.29), "Sosnovka", CITY_MID_DIST },
	{ GVector(1914.64, 6.12, 2245.93), "Kamenka", CITY_MID_DIST },
	{ GVector(12074.06, 190.99, 7272.10), "Orlovets", CITY_MID_DIST },
	{ GVector(4894.84, 322.00, 5638.51), "Pulkovo", CITY_MID_DIST },

	{ GVector(12396.24, 58.29, 9036.73), "Kran", CITY_NEAR_DIST },
	{ GVector(11164.17, 20.99, 2500.34), "Drakon", CITY_NEAR_DIST },
	{ GVector(10409.81, 6.00, 2595.71), "Kraftwerk", CITY_NEAR_DIST },
	{ GVector(10313.11, 49.30, 3622.67), "Wasserdamm", CITY_NEAR_DIST },
};

class GItem {
public:
	typedef char item_type;
	typedef char item_group;

	enum ITEM_TYPE {
		NONE,
		T_CLOTHING,
		T_HOUSE,
		T_INVENTORY,
		T_BOOK,
		T_COMPASS,
		T_MAP,
		T_OPTICS,
		T_TRANSMITTER,
		T_MAGAZINE,
		T_WEAPON
	};

	enum ITEM_TYPE_CLOTHING {
		ATHLETICS_SHOES,
		BAG_CHILDS,
		BAG_HUNTING,
		BAG_IMPROVISED,
		BAG_MOUNTAIN,
		BAG_TALOON,
		BALLISTIC_HELMET,
		BANDANA,
		BASEBALL_CAP,
		BEANIE_HAT,
		BOONIE_HAT,
		BUBBLE_GOOSE,
		CANVAS_PANTS,
		CARGO_PANTS,
		CHEST_HOLSTER,
		COMBAT_BOOTS,
		CONSTRUCTION_HELMET,
		COWBOY_HAT,
		DALLAS_MASK,
		FLAT_CAP,
		GAS_MASK,
		HEAD_TORCH,
		HIKING_BOOTS,
		HOODIE,
		HOXTON_MASK,
		HUNTER_PANTS,
		JEANS,
		JOGGING_SHOES,
		JUNGLE_BOOTS,
		LEATHER_SHOES,
		MILITARY_BERET,
		MILITARY_BOOTS,
		MOTO_HELMET,
		NIOSH_FACE_MASK,
		PILOT_HELMET,
		POLICE_VEST,
		RADAR_CAP,
		RAIN_COAT,
		RIDERS_JACKET,
		SHIRT,
		TACTICAL_SHIRT,
		TSHIRT,
		TTSKO_JACKET,
		TTSKO_PANTS,
		UK_ASSAULT_VEST,
		USHANKA,
		WELLIES,
		WOLF_MASK,
		WOOL_COAT,
		WORKING_BOOTS,
		WORKING_GLOVES,
		ZMIJOVKA_CAP
	};

	enum ITEM_TYPE_HOUSE {
		LAND
	};

	enum ITEM_TYPE_INVENTORY {
		AMMO,
		ARROWS,
		ATTACHMENT,
		BASEBALL_BAT,
		COMBAT_KNIFE,
		CONSUMEABLE,
		CONTAINER,
		COOKER,
		COOKWARE,
		CRAFTING,
		DRINK,
		FARMING_HOE,
		FIREFIGHTER_AXT,
		FOOD,
		FRUIT,
		GLASSES,
		LIGHT,
		MACHETE,
		MEDICAL,
		PEN,
		PIPE_WRENCH,
		PITCH_FORK,
		ROCKET_AVIATORS,
		SPRAY_CAN,
		TOOL,
		WOOD_AXE,
	};

	enum ITEM_TYPE_BOOK {
		BOOK
	};

	enum ITEM_TYPE_COMPASS {
		COMPASS
	};

	enum ITEM_TYPE_MAP {
		MAP
	};

	enum ITEM_TYPE_OPTICS {
		ATTACHMENT_
	};

	enum ITEM_TYPE_TRANSMITTER {
		TRANSMITTER
	};

	enum ITEM_TYPE_MAGAZINE {
		A,
		CLIP,
		M
	};

	enum ITEM_TYPE_WEAPON {
		_1911,
		B95,
		BOW,
		CROSSBOW,
		CZ75,
		FNX45,
		M4A1,
		MAGNUM,
		MKII,
		MOSIN9130,
		BINOCULARS,
		RUGER1022,
		IZH43,
		SKS
	};

	item_group GetTypeClothing() {

		char* item_type_clothing = GetName();

		if (strstr(item_type_clothing, "AthleticShoes"))
			return ITEM_TYPE_CLOTHING::ATHLETICS_SHOES;

		else if (strstr(item_type_clothing, "BagChilds"))
			return ITEM_TYPE_CLOTHING::BAG_CHILDS;

		else if (strstr(item_type_clothing, "BagHunting"))
			return ITEM_TYPE_CLOTHING::BAG_HUNTING;

		else if (strstr(item_type_clothing, "BagImprovised"))
			return ITEM_TYPE_CLOTHING::BAG_IMPROVISED;

		else if (strstr(item_type_clothing, "BagMountain"))
			return ITEM_TYPE_CLOTHING::BAG_MOUNTAIN;

		else if (strstr(item_type_clothing, "BagTaloon"))
			return ITEM_TYPE_CLOTHING::BAG_TALOON;

		else if (strstr(item_type_clothing, "BallisticHelmet"))
			return ITEM_TYPE_CLOTHING::BALLISTIC_HELMET;

		else if (strstr(item_type_clothing, "Bandana"))
			return ITEM_TYPE_CLOTHING::BANDANA;

		else if (strstr(item_type_clothing, "BaseballCap"))
			return ITEM_TYPE_CLOTHING::BASEBALL_CAP;

		else if (strstr(item_type_clothing, "BeanieHat"))
			return ITEM_TYPE_CLOTHING::BEANIE_HAT;

		else if (strstr(item_type_clothing, "BoonieHat"))
			return ITEM_TYPE_CLOTHING::BOONIE_HAT;

		else if (strstr(item_type_clothing, "BubbleGoose"))
			return ITEM_TYPE_CLOTHING::BUBBLE_GOOSE;

		else if (strstr(item_type_clothing, "CanvasPants"))
			return ITEM_TYPE_CLOTHING::CANVAS_PANTS;

		else if (strstr(item_type_clothing, "CargoPants"))
			return ITEM_TYPE_CLOTHING::CARGO_PANTS;

		else if (strstr(item_type_clothing, "ChestHolster"))
			return ITEM_TYPE_CLOTHING::CHEST_HOLSTER;

		else if (strstr(item_type_clothing, "CombatBoots"))
			return ITEM_TYPE_CLOTHING::COMBAT_BOOTS;

		else if (strstr(item_type_clothing, "ConstructionHelmet"))
			return ITEM_TYPE_CLOTHING::CONSTRUCTION_HELMET;

		else if (strstr(item_type_clothing, "CowboyHat"))
			return ITEM_TYPE_CLOTHING::COWBOY_HAT;

		else if (strstr(item_type_clothing, "DallasMask"))
			return ITEM_TYPE_CLOTHING::DALLAS_MASK;

		else if (strstr(item_type_clothing, "FlatCap"))
			return ITEM_TYPE_CLOTHING::FLAT_CAP;

		else if (strstr(item_type_clothing, "GasMask"))
			return ITEM_TYPE_CLOTHING::GAS_MASK;

		else if (strstr(item_type_clothing, "Headtorch"))
			return ITEM_TYPE_CLOTHING::HEAD_TORCH;

		else if (strstr(item_type_clothing, "HikingBoots"))
			return ITEM_TYPE_CLOTHING::HIKING_BOOTS;

		else if (strstr(item_type_clothing, "Hoodie"))
			return ITEM_TYPE_CLOTHING::HOODIE;

		else if (strstr(item_type_clothing, "HoxtonMask"))
			return ITEM_TYPE_CLOTHING::HOXTON_MASK;

		else if (strstr(item_type_clothing, "HunterPants"))
			return ITEM_TYPE_CLOTHING::HUNTER_PANTS;

		else if (strstr(item_type_clothing, "Jeans"))
			return ITEM_TYPE_CLOTHING::JEANS;

		else if (strstr(item_type_clothing, "JoggingShoes"))
			return ITEM_TYPE_CLOTHING::ATHLETICS_SHOES;

		else if (strstr(item_type_clothing, "JungleBoots"))
			return ITEM_TYPE_CLOTHING::JUNGLE_BOOTS;

		else if (strstr(item_type_clothing, "LeatherShoes"))
			return ITEM_TYPE_CLOTHING::LEATHER_SHOES;

		else if (strstr(item_type_clothing, "MilitaryBeret"))
			return ITEM_TYPE_CLOTHING::MILITARY_BERET;

		else if (strstr(item_type_clothing, "MilitaryBoots"))
			return ITEM_TYPE_CLOTHING::MILITARY_BOOTS;

		else if (strstr(item_type_clothing, "MotoHelmet"))
			return ITEM_TYPE_CLOTHING::MOTO_HELMET;

		else if (strstr(item_type_clothing, "NioshFaceMask"))
			return ITEM_TYPE_CLOTHING::NIOSH_FACE_MASK;

		else if (strstr(item_type_clothing, "PilotHelmet"))
			return ITEM_TYPE_CLOTHING::PILOT_HELMET;

		else if (strstr(item_type_clothing, "PoliceVest"))
			return ITEM_TYPE_CLOTHING::POLICE_VEST;

		else if (strstr(item_type_clothing, "RadarCap"))
			return ITEM_TYPE_CLOTHING::RADAR_CAP;

		else if (strstr(item_type_clothing, "RainCoat"))
			return ITEM_TYPE_CLOTHING::RAIN_COAT;

		else if (strstr(item_type_clothing, "RidersJacket"))
			return ITEM_TYPE_CLOTHING::RIDERS_JACKET;

		else if (strstr(item_type_clothing, "Shirt"))
			return ITEM_TYPE_CLOTHING::SHIRT;

		else if (strstr(item_type_clothing, "TacticalShirt"))
			return ITEM_TYPE_CLOTHING::TACTICAL_SHIRT;

		else if (strstr(item_type_clothing, "TShirt"))
			return ITEM_TYPE_CLOTHING::TSHIRT;

		else if (strstr(item_type_clothing, "TTsKO_Jacket"))
			return ITEM_TYPE_CLOTHING::TTSKO_JACKET;

		else if (strstr(item_type_clothing, "ttsko_pants"))
			return ITEM_TYPE_CLOTHING::TTSKO_PANTS;

		else if (strstr(item_type_clothing, "UKAssVest"))
			return ITEM_TYPE_CLOTHING::UK_ASSAULT_VEST;

		else if (strstr(item_type_clothing, "Ushanka"))
			return ITEM_TYPE_CLOTHING::USHANKA;

		else if (strstr(item_type_clothing, "Wellies"))
			return ITEM_TYPE_CLOTHING::WELLIES;

		else if (strstr(item_type_clothing, "WolfMask"))
			return ITEM_TYPE_CLOTHING::WOLF_MASK;

		else if (strstr(item_type_clothing, "WoolCoat"))
			return ITEM_TYPE_CLOTHING::WOOL_COAT;

		else if (strstr(item_type_clothing, "WorkingBoots"))
			return ITEM_TYPE_CLOTHING::WORKING_BOOTS;

		else if (strstr(item_type_clothing, "WorkingGloves"))
			return ITEM_TYPE_CLOTHING::WORKING_GLOVES;

		else if (strstr(item_type_clothing, "ZmijovkaCap"))
			return ITEM_TYPE_CLOTHING::ZMIJOVKA_CAP;

		return ITEM_TYPE::NONE;
	}

	item_group GetTypeHouse() {

		char* item_type_house = GetName();

		if (strstr(item_type_house, "ItemBook"))
			return ITEM_TYPE_HOUSE::LAND;

		return ITEM_TYPE::NONE;
	}

	item_group GetTypeInventory() {

		char* item_type_inventory = GetName();

		if (strstr(item_type_inventory, "Ammo"))
			return ITEM_TYPE_INVENTORY::AMMO;

		else if (strstr(item_type_inventory, "Arrows"))
			return ITEM_TYPE_INVENTORY::ARROWS;

		else if (strstr(item_type_inventory, "Attackment"))
			return ITEM_TYPE_INVENTORY::ATTACHMENT;

		else if (strstr(item_type_inventory, "BaseballBat"))
			return ITEM_TYPE_INVENTORY::BASEBALL_BAT;

		else if (strstr(item_type_inventory, "CombatKnife"))
			return ITEM_TYPE_INVENTORY::COMBAT_KNIFE;

		else if (strstr(item_type_inventory, "Consumable"))
			return ITEM_TYPE_INVENTORY::CONSUMEABLE;

		else if (strstr(item_type_inventory, "Container"))
			return ITEM_TYPE_INVENTORY::CONTAINER;

		else if (strstr(item_type_inventory, "Cooker"))
			return ITEM_TYPE_INVENTORY::COOKER;

		else if (strstr(item_type_inventory, "Cookware"))
			return ITEM_TYPE_INVENTORY::COOKWARE;

		else if (strstr(item_type_inventory, "Crafting"))
			return ITEM_TYPE_INVENTORY::CRAFTING;

		else if (strstr(item_type_inventory, "Drink"))
			return ITEM_TYPE_INVENTORY::DRINK;

		else if (strstr(item_type_inventory, "FarmingHoe"))
			return ITEM_TYPE_INVENTORY::FARMING_HOE;

		else if (strstr(item_type_inventory, "FirefighterAxe"))
			return ITEM_TYPE_INVENTORY::FIREFIGHTER_AXT;

		else if (strstr(item_type_inventory, "Food"))
			return ITEM_TYPE_INVENTORY::FOOD;

		else if (strstr(item_type_inventory, "Fruit"))
			return ITEM_TYPE_INVENTORY::FRUIT;

		else if (strstr(item_type_inventory, "Glasses"))
			return ITEM_TYPE_INVENTORY::GLASSES;

		else if (strstr(item_type_inventory, "Light"))
			return ITEM_TYPE_INVENTORY::LIGHT;

		else if (strstr(item_type_inventory, "Machete"))
			return ITEM_TYPE_INVENTORY::MACHETE;

		else if (strstr(item_type_inventory, "Medical"))
			return ITEM_TYPE_INVENTORY::MEDICAL;

		else if (strstr(item_type_inventory, "Pen"))
			return ITEM_TYPE_INVENTORY::PEN;

		else if (strstr(item_type_inventory, "PipeWrench"))
			return ITEM_TYPE_INVENTORY::PIPE_WRENCH;

		else if (strstr(item_type_inventory, "Pitchfork"))
			return ITEM_TYPE_INVENTORY::PITCH_FORK;

		else if (strstr(item_type_inventory, "RocketAviators"))
			return ITEM_TYPE_INVENTORY::ROCKET_AVIATORS;

		else if (strstr(item_type_inventory, "Spraycan"))
			return ITEM_TYPE_INVENTORY::SPRAY_CAN;

		else if (strstr(item_type_inventory, "Tool"))
			return ITEM_TYPE_INVENTORY::TOOL;

		else if (strstr(item_type_inventory, "WoodAxe"))
			return ITEM_TYPE_INVENTORY::WOOD_AXE;

		return ITEM_TYPE::NONE;
	}

	item_group GetTypeBook() {

		char* item_type_book = GetName();

		if (strstr(item_type_book, "ItemBook"))
			return ITEM_TYPE_BOOK::BOOK;

		return ITEM_TYPE::NONE;
	}

	item_group GetTypeCompass() {

		char* item_type_compass = GetName();

		if (strstr(item_type_compass, "Compass"))
			return ITEM_TYPE_COMPASS::COMPASS;

		return ITEM_TYPE::NONE;
	}

	item_group GetTypeMap() {

		char* item_type_map = GetName();

		if (strstr(item_type_map, "Map"))
			return ITEM_TYPE_MAP::MAP;

		return ITEM_TYPE::NONE;
	}

	item_group GetTypeOptics() {

		char* item_type_optics = GetName();

		if (strstr(item_type_optics, "Attachment"))
			return ITEM_TYPE_OPTICS::ATTACHMENT_;

		return ITEM_TYPE::NONE;
	}

	item_group GetTypeTransmitter() {

		char* item_type_transmitter = GetName();

		if (strstr(item_type_transmitter, "ItemTransmitter"))
			return ITEM_TYPE_TRANSMITTER::TRANSMITTER;

		return ITEM_TYPE::NONE;
	}

	item_group GetTypeMagazine() {

		char* item_type_magazine = GetName();

		if (strstr(item_type_magazine, "A_"))
			return ITEM_TYPE_MAGAZINE::A;

		else if (strstr(item_type_magazine, "CLIP_"))
			return ITEM_TYPE_MAGAZINE::CLIP;

		else if (strstr(item_type_magazine, "M_"))
			return ITEM_TYPE_MAGAZINE::M;

		return ITEM_TYPE::NONE;
	}

	item_group GetTypeWeapon() {

		char* item_type_weapon = GetName();

		if (strstr(item_type_weapon, "1911"))
			return ITEM_TYPE_WEAPON::_1911;

		else if (strstr(item_type_weapon, "b95"))
			return ITEM_TYPE_WEAPON::B95;

		else if (strstr(item_type_weapon, "Bow_"))
			return ITEM_TYPE_WEAPON::BOW;

		else if (strstr(item_type_weapon, "Crossbow"))
			return ITEM_TYPE_WEAPON::CROSSBOW;

		else if (strstr(item_type_weapon, "CZ75"))
			return ITEM_TYPE_WEAPON::CZ75;

		else if (strstr(item_type_weapon, "FNX45"))
			return ITEM_TYPE_WEAPON::FNX45;

		else if (strstr(item_type_weapon, "M4A1"))
			return ITEM_TYPE_WEAPON::M4A1;

		else if (strstr(item_type_weapon, "magnum"))
			return ITEM_TYPE_WEAPON::MAGNUM;

		else if (strstr(item_type_weapon, "mkii"))
			return ITEM_TYPE_WEAPON::MKII;

		else if (strstr(item_type_weapon, "Mosin9130"))
			return ITEM_TYPE_WEAPON::MOSIN9130;

		else if (strstr(item_type_weapon, "Optics_Binoculars"))
			return ITEM_TYPE_WEAPON::BINOCULARS;

		else if (strstr(item_type_weapon, "Ruger1022"))
			return ITEM_TYPE_WEAPON::RUGER1022;

		else if (strstr(item_type_weapon, "ShotgunIzh43"))
			return ITEM_TYPE_WEAPON::IZH43;

		else if (strstr(item_type_weapon, "SKS"))
			return ITEM_TYPE_WEAPON::SKS;

		return ITEM_TYPE::NONE;
	}


	GItem(CItem *item_obj) {
		obj = item_obj;
	}

	GItem(CObject *item_obj) {
		obj = (CItem*)item_obj;
	}

	GItem(CWeapon *item_obj) {
		obj = (CItem*)item_obj;
	}

	void SetObject(CItem *item_obj) {
		obj = item_obj;
	}

	bool Valid();

	bool IsRuined() {
		return (bool)obj->ruined;
	}

	char *GetTypeName();
	char *GetName();
	char *GetClearName();

	item_type GetType() {

		if (!Valid())
			return ITEM_TYPE::NONE;

		char* item_type_name = GetTypeName();

		if (strcmp(item_type_name, "clothing") == 0)
			return ITEM_TYPE::T_CLOTHING;

		else if (strcmp(item_type_name, "house") == 0)
			return ITEM_TYPE::T_HOUSE;

		else if (strcmp(item_type_name, "inventoryItem") == 0)
			return ITEM_TYPE::T_INVENTORY;

		else if (strcmp(item_type_name, "ItemBook") == 0)
			return ITEM_TYPE::T_BOOK;

		else if (strcmp(item_type_name, "ItemCompass") == 0)
			return ITEM_TYPE::T_COMPASS;

		else if (strcmp(item_type_name, "itemoptics") == 0)
			return ITEM_TYPE::T_OPTICS;

		else if (strcmp(item_type_name, "ItemTransmitter") == 0)
			return ITEM_TYPE::T_TRANSMITTER;

		else if (strcmp(item_type_name, "ProxyMagazines") == 0)
			return ITEM_TYPE::T_MAGAZINE;

		else if (strcmp(item_type_name, "Weapon") == 0)
			return ITEM_TYPE::T_WEAPON;

		return ITEM_TYPE::NONE;
	}

private:
	CItem *obj;
};

#endif