# Day Z Multihack written by Gordon`

Multihack written in C++ from 2014 for DayZ, a multiplayer openworld zombie survival game/simulation. 
Uses D3D9 (implemented as virtual machine table (VMT, VTable) hook but it modifies virtual memory in the data section of the game (only seen HackShield and PunkBuster scanning for D3D VMT hooks yet)), or uses D3D11 (implemented on a invisible/hidden window overlay as this doesn't modify any client/engine code which could be scanned by an anticheat.
DayZ is protected by VAC3 (Valve Anti Cheat) and BattlEye. As long as the multihack doesn't get published on cheat forums, it's pretty hard for those two anti cheats to detect private multihacks.
Strings are encrypted with XorKey, All it needed for the multihack to be undetected.

Includes:
Aimbot, ESP, Wallhack, Chams, Speedhack, Telekill, Menu, Console, swimming in the sky, warning messages, player item stealing and much much more. Enough for DayZ to be easily destroyed by a single multihack, the reason why I only released source code on cheat developing forums, never executable binaries. Was only used for private purposes and it was a lot of fun.
