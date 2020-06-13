// menu_definitions_sheet.h

// TODO
// Set num positions on Enter. Gamedir?


// 								cursor solid count 	col1, col2, 	nwidth
CMD_DEF ( Main,							0,			0,		0,		0			)
CMD_DEF (	SinglePlayer,				0,			0,		0,		0			) 
//				New Game
CMD_DEF (		Load,					0,			16,		0,		35 * 8		)
CMD_DEF (		Save, 					0,			16,		0,		35 * 8		)
CMD_DEF (		Levels, 				0,			0,		0,		0			)
CMD_DEF (	MultiPlayer, 				0,			0,		0,		0			)
CMD_DEF (		JoinGame, 				0,  		0,		0,		29 * 8		)
CMD_DEF (			Search, 			0,			0,		0,		0			)
CMD_DEF (				ServerList, 	0,			0,		0,		0			)
CMD_DEF (		HostGame, 				0,  		0,		0,		29 * 8		)		
CMD_DEF (			GameOptions, 		0, 		6 * 8, 20 * 8,		32 * 8		)
CMD_DEF (		PlayerSetup, 			0,		6 * 8,		0,		32 * 8		)
CMD_DEF (			NameMaker, 			0,			0,		0,		0			)
CMD_DEF (		Demos, 					0,			0,		0,		0			)
CMD_DEF (	Options, 					0,		6 * 8,	  220,		34 * 8		)
CMD_DEF (		CustomizeControls, 		0,		2 * 8,	    0,		38 * 8		)
CMD_DEF (		Preferences, 			0,		7 * 8, 22 * 8,		34 * 8		)
CMD_DEF (		Video, 					0,		6 * 8,		0,		34 * 8		)
CMD_DEF (	Help, 						0,			0,		0,		0			)
CMD_DEF (	Quit, 						0,			0,		0,		0			)

//==================================
CMD_DEF ( Dialog, 						0,			0,		0,		0			)
CMD_DEF ( OnScreenKeyboard, 			0,			0,		0,		0			)
#undef CMD_DEF

