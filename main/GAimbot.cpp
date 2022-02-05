#include "GAimbot.h"
#include "GPlayerManager.h"
#include "GDraw.h"
#include "GOverlay.h"

GAimbot::GAimbot()
{
	distance = 0;
	buf_distance = 0;
	nearest_object = 0;
	locked = false;
}

void GAimbot::Reset()
{
	if(!cvars.AIM_Lock) 
		locked = false;

	if (!locked) {

		buf_distance = 0;
		nearest_object = 0;
	}
}

void GAimbot::Collect(CObject *obj)
{
	if (!cvars.AIM_On)
		return;

	if (locked)
		return;

	if (!g_pPlayerMgr->IsValid(obj))
		return;

	CObject *pLocalPlayer = g_pPlayerMgr->GetLocalPlayer();
	if (!pLocalPlayer)
		return;

	if (!g_pPlayerMgr->IsConnected())
		return;

	if (pLocalPlayer == obj)
		return;

	if (obj->waterContact)
		return;

	if (cvars.AIM_Telekill) {
		distance = g_pPlayerMgr->CalcDistance(old_pos, obj->visualstate->pos);
	}
	else {
		distance = g_pPlayerMgr->CalcDistance(obj->visualstate->pos);
	}

	//CEntity *ent = g_pPlayerMgr->GetPlayerByIndex1(i);
	//g_pPlayerMgr->IsVisible(ent);

	//bool bVisible = true;
	bool bCanAim = true;
	bool bZombie = g_pPlayerMgr->IsZombie(obj);
	bool bAnimal = g_pPlayerMgr->IsAnimal(obj);

	if (cvars.AIM_Mode == 1) {

		GVector out;
		if (!g_pPlayerMgr->WorldToScreen(obj, out))
			return;

		int w, h;
		g_pOverlay->DirectX->Draw->GetViewport(w, h);
		w /= 2;
		h /= 2;

		float x = out.x > w ? out.x - w : w - out.x;
		float y = out.y > h ? out.y - h : h - out.y;

		distance = sqrtf(x*x + y*y);
	}
	else if (cvars.AIM_Mode == 2 && (!bZombie || !bAnimal)) {

		player_info info;
		g_pPlayerMgr->GetPlayerHealthBlood(obj, info);
		distance = info.health;
	}

	if (obj->isDead)
		return;

	if (bZombie && !cvars.AIM_Zombie)
		return;

	if (!bZombie && !cvars.AIM_Player)
		return;

	if (g_pPlayerMgr->IsFriend(obj) && !cvars.AIM_Friends)
		return;

	if ((distance < buf_distance || !buf_distance) && bCanAim) {

		buf_distance = distance;
		nearest_object = obj;
	}
}

void GAimbot::Do()
{	
	if (!cvars.AIM_On)
		return;

	if (nearest_object == 0 || !g_pPlayerMgr->IsValid(nearest_object)) {

		locked = false;
		return;
	}

	CObject *obj = nearest_object;
	if (!obj || obj->isDead) {

		locked = false;
		return;
	}

	if (cvars.AIM_Pos == 0) 
		target_pos = obj->visualstate->head_pos;
	else if (cvars.AIM_Pos == 1)
		target_pos = obj->visualstate->body_pos;
	else if (cvars.AIM_Pos == 2)
		target_pos = obj->visualstate->pos;

	if (g_pPlayerMgr->IsAnimal(obj))
		target_pos = obj->visualstate->body_pos;


	GVector out;
	if (g_pPlayerMgr->WorldToScreen(target_pos, out))
		g_pOverlay->DirectX->Draw->FilledQuad(out.x, out.y, 2, 2, color_white);


	static bool reset_pos = false;
	if (cvars.AIM_Key == 0 || (cvars.AIM_Key && g_pInput->IsKeyDown(cvars.AIM_Key))) {

		if (cvars.AIM_Lock)
			locked = true;

		if (cvars.AIM_Telekill) {

			GVector view = g_pPlayerMgr->GetPlayerViewangles(obj);
			GVector pos_view = obj->visualstate->pos;

			float Speed = 1;
			pos_view.x -= sin(view.y) * Speed;
			pos_view.z -= cos(view.y) * Speed;

			g_pPlayerMgr->GetLocalPlayer()->visualstate->pos = pos_view;

			reset_pos = true;
		}

		g_pPlayerMgr->AimToPoint(target_pos);
	}
	else {

		if (cvars.AIM_Telekill) {

			if (reset_pos) {

				g_pPlayerMgr->GetLocalPlayer()->visualstate->pos = old_pos;
				g_pPlayerMgr->SetViewangles(old_viewangles);

				reset_pos = false;
			}
			else {

				old_pos = g_pPlayerMgr->GetLocalPlayer()->visualstate->pos;
				old_viewangles = g_pPlayerMgr->GetViewangles();
			}
		}

		locked = false;
	}
}