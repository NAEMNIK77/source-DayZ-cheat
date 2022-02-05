#ifndef __GPLAYERMGR__
#define __GPLAYERMGR__

struct player_info {
	float health;
	float blood;
	float morale;
	float shock;
	float bleedingsources;
	float bleedingeffects;

	float unconscious;
};

class GPlayerManager {
public:
	GPlayerManager();
	void Init();
	void Release();

	bool IsValidWorld();
	bool IsValid(const CObject *obj);
	bool IsValid(const CItem *obj);

	CBase *GetBase();
	CNetworkManager *GetNetworkMgr();
	CTransformationData *GetTransformation();
	CGameTime *GetTime();

	CObject *GetLocalPlayer();
	CObject *GetPlayerByIndex(int index);
	CEntity *GetPlayerByIndex1(int index);
	CObject *GetPlayerByPlayerId(int playerid);

	CObject *GetTableObjectByIndex(char object_type, int index);
	int GetMaxObjectIndex();
	int GetMaxTableObjectIndex(char object_type);

	GVector GetViewangles();
	GVector GetForwardVector(float factor = 1);
	GVector GetRightVector(float factor = 1);
	GVector GetUpVector(float factor = 1);
	GVector GetViewPosition();

	GVector GetPlayerViewangles(const CObject *obj);
	GVector GetPlayerForwardVector(const CObject *obj, float factor = 1);
	GVector GetPlayerRightVector(const CObject *obj, float factor = 1);

	float CalcDistance(const CObject *from, const CObject *to);
	float CalcDistance(const CObject *to);
	float CalcDistance(const CItem *to);
	float CalcDistance(GVector from, GVector to);
	float CalcDistance(GVector to);

	bool WorldToScreen(const CObject *in, GVector &out);
	bool WorldToScreen(GVector in, GVector &out);

	char *GetPlayerName(const CObject *obj);
	char *GetPlayerName(int playerid);

	bool IsZombie(const CObject *obj);
	bool IsAnimal(const CObject *obj);
	bool IsPlayer(const CObject *obj);
	bool IsFriend(const CObject *obj);

	bool IsConnected();

	bool EnemyAimingAtYou(const CObject* obj);

	bool InCenterArea(GVector screen);

	void DoPointerSearch();

	char *GetPlayerWeaponName(const CObject *obj);
	void GetPlayerHealthBlood(const CObject *obj, player_info &info);

	void AimToPoint(const GVector pos);
	void SetViewangles(const GVector angles);

	void DrawHealthbars();
	void DrawRadar();
	void DrawStatsbox();

	void FlyToView(GVector &pos);
	void GetFixedView(GVector &forward, GVector &up, GVector &right);
	void GetFixedView(GVector &forward, GVector &up, GVector &right, GVector &fov, GVector &translation, GVector &viewport);

	void RefillAmmo();
	void RemoveFiringSpeed();
	int GetMagazineAmmo(CObject *mag, int *maxammo = 0);
	void SetMagazineAmmo(CObject *mag);

	void CalcVelocity(int i);
	float GetVelocity(int i);

	bool IsVisible(CObject *obj, GVector start, GVector end);

	void SetWorldTime(const int no);

	char *GetObjectName(const CObject *obj);
	char *GetObjectClassName(const CObject *obj);
	char *GetObjectTypeName(const CObject *obj);

	void DoTeleport();

	void AddTeleportList(const CItem *item);
	void ClearTeleportList();

	std::list<CItem*> GetItemContainerContent(const CItem *obj);
	bool IsInItemContainer(const CItem *item, const CItem *obj);
	void RemoveFromItemContainer(const CItem *item, const CItem *remove);
	std::list<CItem*> GetWeaponSlots(const CWeapon *obj);

	bool FindItem(const CItem *item);

	void DropItem(CEntity *player, CItem *item);
	void DropItem(CObject *player, CItem *item);
	void MoveItem(CObject *player, CItem *item);
	void ItemMoveIn(CItem *item);
	void ItemMoveOut(CEntity *player, CItem *item);
	void MoveToGround(CItem *item);
	void AskForDropItem(CObject *player, CItem *item);

	void ShowHint(char *text);

	char Intersect(GVector start, GVector end);

	class GESP *ESP;
	class GAimbot *Aimbot;

	CTransformation *transform;

	int local_index;
private:
	
	CBase *base;
	CNetworkManager *networkmgr;
	CGameTime *time;
	const DWORD playerinfo_offset = 0xE8;

	std::list<const CItem*>list_teleport;

	const DWORD diff_time = 1000;

	struct velocity {
		GVector pos;
		GVector old_pos;
		float speed;
		bool nextFrame;
		GTimer timer;
	}; velocity vel_enemy[256];

};

extern GPlayerManager *g_pPlayerMgr;

#endif