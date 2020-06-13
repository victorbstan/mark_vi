/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2009-2018 Baker and others

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// mnu_multiplayer_hostgame_gameoptions.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_GameOptions
#define LOCAL_EVENT(NAME)		void Mnu_GameOptions_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

// Define menu items
typedef enum { ENUM_FORCE_INT_GCC_ (opt)
	opt_begin_game_0,
	opt_maxplayers_1,
	opt_server_is_public_2,
	opt_deathmatch_coop_3,
	opt_teamplay_4,
	opt_skill_5,
	opt_frag_limit_6,
	opt_time_limit_7,
	opt_episode_8,
	opt_level_9,
	opt_COUNT // Ender
} opt_e;

//=============================================================================
/* GAME OPTIONS MENU */


static int local_cursor_table[opt_COUNT] = {40, 56, 64, 72, 80, 88, 96, 104, 120, 128}; // Cursor positions

static int m_maxplayers;
static int m_start_episode;
static int m_start_level;



level_t		levels[] =
{
	{"start", "Entrance"},	// 0

	{"e1m1", "Slipgate Complex"},				// 1
	{"e1m2", "Castle of the Damned"},
	{"e1m3", "The Necropolis"},
	{"e1m4", "The Grisly Grotto"},
	{"e1m5", "Gloom Keep"},
	{"e1m6", "The Door To Chthon"},
	{"e1m7", "The House of Chthon"},
	{"e1m8", "Ziggurat Vertigo"},

	{"e2m1", "The Installation"},				// 9
	{"e2m2", "Ogre Citadel"},
	{"e2m3", "Crypt of Decay"},
	{"e2m4", "The Ebon Fortress"},
	{"e2m5", "The Wizard's Manse"},
	{"e2m6", "The Dismal Oubliette"},
	{"e2m7", "Underearth"},

	{"e3m1", "Termination Central"},			// 16
	{"e3m2", "The Vaults of Zin"},
	{"e3m3", "The Tomb of Terror"},
	{"e3m4", "Satan's Dark Delight"},
	{"e3m5", "Wind Tunnels"},
	{"e3m6", "Chambers of Torment"},
	{"e3m7", "The Haunted Halls"},

	{"e4m1", "The Sewage System"},				// 23
	{"e4m2", "The Tower of Despair"},
	{"e4m3", "The Elder God Shrine"},
	{"e4m4", "The Palace of Hate"},
	{"e4m5", "Hell's Atrium"},
	{"e4m6", "The Pain Maze"},
	{"e4m7", "Azure Agony"},
	{"e4m8", "The Nameless City"},

	{"end", "Shub-Niggurath's Pit"},			// 31

	{"dm1", "Place of Two Deaths"},				// 32
	{"dm2", "Claustrophobopolis"},
	{"dm3", "The Abandoned Base"},
	{"dm4", "The Bad Place"},
	{"dm5", "The Cistern"},
	{"dm6", "The Dark Zone"}
};

//MED 01/06/97 added hipnotic levels
level_t     hipnoticlevels[] =
{
   {"start", "Command HQ"},  // 0

   {"hip1m1", "The Pumping Station"},          // 1
   {"hip1m2", "Storage Facility"},
   {"hip1m3", "The Lost Mine"},
   {"hip1m4", "Research Facility"},
   {"hip1m5", "Military Complex"},

   {"hip2m1", "Ancient Realms"},          // 6
   {"hip2m2", "The Black Cathedral"},
   {"hip2m3", "The Catacombs"},
   {"hip2m4", "The Crypt"},
   {"hip2m5", "Mortum's Keep"},
   {"hip2m6", "The Gremlin's Domain"},

   {"hip3m1", "Tur Torment"},       // 12
   {"hip3m2", "Pandemonium"},
   {"hip3m3", "Limbo"},
   {"hip3m4", "The Gauntlet"},

   {"hipend", "Armagon's Lair"},       // 16

   {"hipdm1", "The Edge of Oblivion"}           // 17
};

//PGM 01/07/97 added rogue levels
//PGM 03/02/97 added dmatch level
level_t		roguelevels[] =
{
	{"start",	"Split Decision"},
	{"r1m1",	"Deviant's Domain"},
	{"r1m2",	"Dread Portal"},
	{"r1m3",	"Judgement Call"},
	{"r1m4",	"Cave of Death"},
	{"r1m5",	"Towers of Wrath"},
	{"r1m6",	"Temple of Pain"},
	{"r1m7",	"Tomb of the Overlord"},
	{"r2m1",	"Tempus Fugit"},
	{"r2m2",	"Elemental Fury I"},
	{"r2m3",	"Elemental Fury II"},
	{"r2m4",	"Curse of Osiris"},
	{"r2m5",	"Wizard's Keep"},
	{"r2m6",	"Blood Sacrifice"},
	{"r2m7",	"Last Bastion"},
	{"r2m8",	"Source of Evil"},
	{"ctf1",    "Division of Change"}
};

const int num_quake_original_levels = ARRAY_COUNT(levels);

typedef struct
{
	const char	*description;
	int			firstLevel;
	int			levels;
} episode_t;

episode_t	episodes[] =
{
	{"Welcome to Quake", 0, 1},
	{"Doomed Dimension", 1, 8},
	{"Realm of Black Magic", 9, 7},
	{"Netherworld", 16, 7},
	{"The Elder World", 23, 8},
	{"Final Level", 31, 1},
	{"Deathmatch Arena", 32, 6}
};

//MED 01/06/97  added hipnotic episodes
episode_t   hipnoticepisodes[] =
{
   {"Scourge of Armagon", 0, 1},
   {"Fortress of the Dead", 1, 5},
   {"Dominion of Darkness", 6, 6},
   {"The Rift", 12, 4},
   {"Final Level", 16, 1},
   {"Deathmatch Arena", 17, 1}
};

//PGM 01/07/97 added rogue episodes
//PGM 03/02/97 added dmatch episode
episode_t	rogueepisodes[] =
{
	{"Introduction", 0, 1},
	{"Hell's Fortress", 1, 7},
	{"Corridors of Time", 8, 8},
	{"Deathmatch Arena", 16, 1}
};


//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	qpic_t	*p;
	int y = 40;

	M_DrawTransPic (16, 4, Draw_CachePic ("gfx/qplaque.lmp"), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);
	p = Draw_CachePic ("gfx/p_multi.lmp");
	M_DrawPic ( (320-p->width)/2, 4, p);

	M_DrawTextBox (152, y-8, 10, 1, 6 /* hotspot border */);
	M_Print (160, y, "begin game");
	y += 16;

	Hotspots_Add (local_menu->column1, y, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (0, y, "      Max players");
	M_Print (160, y, va("%d", m_maxplayers) );
	y += 8;

	Hotspots_Add (local_menu->column1, y, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (0, y, "           Public");
	if (sv_public.value)
		M_Print (local_menu->column2, y, "Yes");
	else
		M_Print (local_menu->column2, y, "No");
	y += 8;

	Hotspots_Add (local_menu->column1, y, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (0, y, "        Game Type");
	if (pr_coop.value)
		M_Print (local_menu->column2, y, "Cooperative");
	else
		M_Print (local_menu->column2, y, "Deathmatch");
	y += 8;

	Hotspots_Add (local_menu->column1, y, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (0, y, "        Teamplay");
	if (com_gametype == gametype_rogue) {
		const char *msg;

		switch((int)pr_teamplay.value) {
			default: 		msg = "Off";
			case_break 1: 	msg = "No Friendly Fire";
			case_break 2: 	msg = "Friendly Fire";
			case_break 3: 	msg = "Tag";
			case_break 4: 	msg = "Capture the Flag";
			case_break 5: 	msg = "One Flag CTF";
			case_break 6: 	msg = "Three Team CTF";
		}
		M_Print (local_menu->column2, y, msg);
	}
	else
	{
		const char *msg;

		switch((int)pr_teamplay.value) {
		default:		msg = "Off"; break;
		case_break 1:	msg = "No Friendly Fire";
		case_break 2:	msg = "Friendly Fire";
		}
		M_Print (local_menu->column2, y, msg);
	}
	y+=8;

	Hotspots_Add (local_menu->column1, y, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (0, y, "            Skill");
	     if (pr_skill.value == 0)				M_Print (local_menu->column2, y, "Easy difficulty");
	else if (pr_skill.value == 1)				M_Print (local_menu->column2, y, "Normal difficulty");
	else if (pr_skill.value == 2)				M_Print (local_menu->column2, y, "Hard difficulty");
	else										M_Print (local_menu->column2, y, "Nightmare difficulty");
	y+=8;

	Hotspots_Add (local_menu->column1, y, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (0, y, "       Frag Limit");
	      if (pr_fraglimit.value == 0)			M_Print (local_menu->column2, y, "none");
	else										M_Print (local_menu->column2, y, va("%d frags", (int)pr_fraglimit.value));
	y+=8;

	Hotspots_Add (local_menu->column1, y, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (0, y, "       Time Limit");
	if (pr_timelimit.value == 0)				M_Print (local_menu->column2, y, "none");
	else										M_Print (local_menu->column2, y, va("%d minutes", (int)pr_timelimit.value));

	y+=8;
	y+=8;

	Hotspots_Add (local_menu->column1, y, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (0, y, "         Episode");
	//MED 01/06/97 added hipnotic episodes
        if (com_gametype == gametype_hipnotic)	M_Print (local_menu->column2, y, hipnoticepisodes[m_start_episode].description); //PGM 01/07/97 added rogue episodes
   
	else if (com_gametype == gametype_rogue)		M_Print (local_menu->column2, y, rogueepisodes[m_start_episode].description);
	else											M_Print (local_menu->column2, y, episodes[m_start_episode].description);
	y+=8;

	Hotspots_Add (local_menu->column1, y, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (0, y, "           Level");
   
	if (com_gametype == gametype_hipnotic) { //MED 01/06/97 added hipnotic episodes
		M_Print (local_menu->column2, y,		hipnoticlevels[hipnoticepisodes[m_start_episode].firstLevel + m_start_level].description);
		M_Print (local_menu->column2, y + 8,	hipnoticlevels[hipnoticepisodes[m_start_episode].firstLevel + m_start_level].name);
	}
   
	else if (com_gametype == gametype_rogue) {//PGM 01/07/97 added rogue episodes
		M_Print (local_menu->column2, y,		roguelevels[rogueepisodes[m_start_episode].firstLevel + m_start_level].description);
		M_Print (local_menu->column2, y + 8,	roguelevels[rogueepisodes[m_start_episode].firstLevel + m_start_level].name);
	}

   else {
		M_Print (local_menu->column2, y,		levels[episodes[m_start_episode].firstLevel + m_start_level].description);
		M_Print (local_menu->column2, y + 8,	levels[episodes[m_start_episode].firstLevel + m_start_level].name);
   }
	y+=8;

// line cursor
	M_DrawCharacter (144, local_cursor_table[local_menu->cursor], 12 + ((int)(realtime*4) & 1));
}


void M_NetStart_Change (int dir)
{
	
	float	f;

	switch (local_menu->cursor) {
	default:										// 0 - begin game which shouldn't happen because Keys doesn't send us here on cursor == 0
	case_break opt_maxplayers_1:					cursor_wrap_set_min_max (m_maxplayers, m_maxplayers + dir, 2, svs.maxclientslimit); // 2 to svs max players
		
	case_break opt_server_is_public_2:				Cvar_SetValueQuick (&sv_public, sv_public.value ? 0 : 1); // Public
	case_break opt_deathmatch_coop_3:				Cvar_SetValueQuick (&pr_coop, pr_coop.value ? 0 : 1); // Coop  vs. Deathmatch
		
	case_break opt_teamplay_4: {					int maxval = (com_gametype == gametype_rogue) ? 6 : 2; // Teamplay
													f = pr_teamplay.value; // March 21 2018 - Team play 2 is friendly fire on but still teamplay.
													cursor_wrap_set_min_max (f, f + dir, 0, maxval); // 0 to 2 teamplay or if rogue then 0 to 6
													Cvar_SetValueQuick (&pr_teamplay, f);
	}
	case_break opt_skill_5:							f = pr_skill.value; // Skill
													cursor_wrap_set_min_max (f, f + dir, 0, 3);
													Cvar_SetValueQuick (&pr_skill, f);

	case_break opt_frag_limit_6:					f = pr_fraglimit.value; // Frag limit
													cursor_wrap_set_min_max (f, f + dir * 10, 0, 100); // 0 to 100 frag limit
													Cvar_SetValueQuick (&pr_fraglimit, f);
		
	case_break opt_time_limit_7:					f = pr_timelimit.value; // Time limit
													cursor_wrap_set_min_max (f, f + dir * 5, 0, 60); // 0 to 60 time limit
													Cvar_SetValueQuick (&pr_timelimit, f);

	case_break opt_episode_8: { // Episode like "Welcome To Quake" or "Dimension of the Doomed"
		int episodes_count;
		
		     if (com_gametype == gametype_hipnotic)	episodes_count = 6; //MED 01/06/97 added hipnotic count
		else if (com_gametype == gametype_rogue)	episodes_count = 4; //PGM 01/07/97 added rogue count //PGM 03/02/97 added 1 for dmatch episode
		else if (registered.value)					episodes_count = 7;
		else										episodes_count = 2;

		cursor_wrap_set_min_max (m_start_episode, m_start_episode + dir, 0, episodes_count - 1);

		m_start_level = 0; // On change set it to the first level
	}
	case_break opt_level_9: { // Level - like Slipgate complex
		int levels_count;
    
		     if (com_gametype == gametype_hipnotic)		levels_count = hipnoticepisodes[m_start_episode].levels; //MED 01/06/97 added hipnotic episodes
		else if (com_gametype == gametype_rogue)		levels_count = rogueepisodes[m_start_episode].levels; //PGM 01/06/97 added hipnotic episodes			
		else											levels_count = episodes[m_start_episode].levels;

		cursor_wrap_set_min_max (m_start_level, m_start_level + dir, 0, levels_count - 1);
	}} // end case 9, end select
}


//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	if (key == K_ENTER && local_menu->cursor != opt_begin_game_0) key = K_RIGHTARROW;

	switch (key) {
	default:						// Do nothing.
	case_break K_ESCAPE:			Mnu_MultiPlayer_Enter_f (NULL);
	case_break K_UPARROW:			MENU_ROW_CHANGE_HARD_SOUND (); cursor_dec (LOCAL_MENU_STATE);
	case_break K_DOWNARROW:			MENU_ROW_CHANGE_HARD_SOUND (); cursor_inc (LOCAL_MENU_STATE);
	case_break K_LEFTARROW:			if (local_menu->cursor == opt_begin_game_0)
										break; // Ignore it
									MENU_TOGGLE_SOFT_SOUND();
									M_NetStart_Change (-1);

	case_break K_RIGHTARROW:		if (local_menu->cursor == opt_begin_game_0)
										break; // Ignore it
									MENU_TOGGLE_SOFT_SOUND();
									M_NetStart_Change (1);

	case_break K_ENTER:				
									if (local_menu->cursor == opt_begin_game_0) { // "Begin Game"
										MENU_ENTER_LOUD_SOUND();
										if (sv.active)
											Cbuf_AddTextLine ("disconnect");
										Cbuf_AddTextLine  ("listen 0");	// so host_netport will be re-examined
										Cbuf_AddTextLinef ("maxplayers %d", m_maxplayers);
										SCR_BeginLoadingPlaque ();

										// Depending on game type, we use approprate table to get the map name.
										if (com_gametype == gametype_hipnotic)		Cbuf_AddTextLinef ("map %s", hipnoticlevels[hipnoticepisodes[m_start_episode].firstLevel + m_start_level].name );
										else if (com_gametype == gametype_rogue)	Cbuf_AddTextLinef ("map %s", roguelevels[rogueepisodes[m_start_episode].firstLevel + m_start_level].name );
										else										Cbuf_AddTextLinef ("map %s", levels[episodes[m_start_episode].firstLevel + m_start_level].name );

										Key_SetDest (key_game);
										return;
									}

									M_NetStart_Change (1);
	} // End of switch (key)
}


//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused)
{
	Key_SetDest (key_menu); Menu_SetDest (LOCAL_MENU_STATE);
	
	if (m_maxplayers == 0) m_maxplayers = svs.maxclients_public; // Use the public fake # here because this is the cap  (TODO See what this does?)
	if (m_maxplayers < 2)
		m_maxplayers = svs.maxclientslimit;
}


//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	DEBUG_COMPILE_TIME_ASSERT (/*hint word*/ opt_COUNT, opt_COUNT == 10);
	self->cursor_solid_count = opt_COUNT; // Immutable
}

