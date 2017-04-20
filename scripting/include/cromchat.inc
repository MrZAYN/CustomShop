#if defined _cromchat_included
    #endinput
#endif

#define _cromchat_included

#if !defined ColorChat
	#define ColorChat CC_SendMessage
#endif

#if !defined client_print_color
	#define client_print_color CC_SendMessage
#endif

#define CromChat CC_SendMessage

#define CC_VERSION 1.1
#define CC_MAX_TEAM_SIZE 10
#define CC_MAX_PREFIX_SIZE 64
#define CC_MAX_MESSAGE_SIZE 188

new CC_PREFIX[CC_MAX_PREFIX_SIZE], bool:CC_FIRST_TIME = true, CC_MSG_SAYTEXT, CC_MSG_TEAMINFO
new const CC_REPLACE_COLORS[][] = { "&x04", "^x04", "&x03", "^x03", "&x01", "^x01" }
new const CC_PLUS_COLORS[][] = { "&x07", "TERRORIST", "&x06", "CT", "&x05", "SPECTATOR" }
new const CC_COLORS_LIST[][] = { "&x07", "&x06", "&x05", "&x04", "&x03", "&x01", "&x00" }
new const CC_MENU_COLORS[][] = { "\y", "\r", "\w", "\d", "\R" }
new const CC_NO_PREFIX[] = "&x00"

stock CC_SendMessage(const id, const szInput[], any:...)
{
	static iPlayers[32], iPnum
	
	if(id)
	{
		if(!is_user_connected(id))
			return 0
	}
	else
	{
		get_players(iPlayers, iPnum, "ch")
		
		if(!iPnum)
			return 0
	}
	
	static szMessage[CC_MAX_MESSAGE_SIZE], szTeam[CC_MAX_TEAM_SIZE], bool:bNoPrefix, i
	vformat(szMessage[1], charsmax(szMessage), szInput, 3)
	szMessage[0] = 0x01
	szTeam[0] = EOS
	
	bNoPrefix = bool:equal(szMessage[1], CC_NO_PREFIX, charsmax(CC_NO_PREFIX))
	
	if(bNoPrefix)
		replace(szMessage, charsmax(szMessage), CC_NO_PREFIX, "")
	else if(CC_PREFIX[0])
		format(szMessage, charsmax(szMessage), "^x01%s ^x01%s", CC_PREFIX, szMessage)
	
	for(i = 0; i < sizeof(CC_REPLACE_COLORS) - 1; i += 2)
		replace_all(szMessage, charsmax(szMessage), CC_REPLACE_COLORS[i], CC_REPLACE_COLORS[i + 1])
		
	for(i = 0; i < sizeof(CC_PLUS_COLORS) - 1; i += 2)
	{
		if(contain(szMessage, CC_PLUS_COLORS[i]) != -1)
		{
			copy(szTeam, charsmax(szTeam), CC_PLUS_COLORS[i + 1])
			break
		}
	}
	
	if(szTeam[0])
	{
		for(i = 0; i < 3; i++)
			replace_all(szMessage, charsmax(szMessage), CC_COLORS_LIST[i], "^x03")
	}
	
	if(id)
		CC_WriteMessage(id, szMessage, szTeam)
	else
	{
		for(i = 0; i < iPnum; i++)
			CC_WriteMessage(iPlayers[i], szMessage, szTeam)
	}
	
	return strlen(szMessage)
}

stock CC_LogMessage(const id, const szFile[] = "", const szInput[], any:...)
{
	static szMessage[CC_MAX_MESSAGE_SIZE]
	vformat(szMessage, charsmax(szMessage), szInput, 4)
	
	if(!CC_SendMessage(id, szMessage))
		return 0
		
	CC_RemoveColors(szMessage, charsmax(szMessage))
	szFile[0] ? log_to_file(szFile, szMessage) : log_amx(szMessage)
	return strlen(szMessage)
}
	
stock CC_WriteMessage(const id, const szMessage[], szTeam[CC_MAX_TEAM_SIZE])
{
	static bool:bRefresh
	bRefresh = true
	
	if(!szTeam[0])
	{
		bRefresh = false
		get_user_team(id, szTeam, charsmax(szTeam))
	}
	
	if(CC_FIRST_TIME)
	{
		CC_FIRST_TIME = false
		CC_MSG_SAYTEXT = get_user_msgid("SayText")
		CC_MSG_TEAMINFO = get_user_msgid("TeamInfo")
	}
		
	CC_SendTeamInfo(id, szTeam)

	message_begin(MSG_ONE, CC_MSG_SAYTEXT, _, id)
	write_byte(id)
	write_string("%s")
	write_string(szMessage)
	message_end()
	
	if(bRefresh)
	{
		get_user_team(id, szTeam, charsmax(szTeam))
		CC_SendTeamInfo(id, szTeam)
	}
}

stock CC_RemoveColors(szMessage[], iLen, bool:bChat = true, bool:bMenu = false)
{
	static i
	
	if(bChat)
	{
		for(i = 0; i < sizeof(CC_COLORS_LIST); i++)
			replace_all(szMessage, iLen, CC_COLORS_LIST[i], "")
	}
	
	if(bMenu)
	{
		for(i = 0; i < sizeof(CC_MENU_COLORS); i++)
			replace_all(szMessage, iLen, CC_MENU_COLORS[i], "")
	}
}

stock CC_SendTeamInfo(const id, const szTeam[CC_MAX_TEAM_SIZE])
{
	message_begin(MSG_ONE, CC_MSG_TEAMINFO, _, id)
	write_byte(id)
	write_string(szTeam)
	message_end()
}

stock CC_SetPrefix(const szPrefix[])
	copy(CC_PREFIX, charsmax(CC_PREFIX), szPrefix)