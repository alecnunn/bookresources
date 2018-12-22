--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  Text.lua
-- author:    Paul Schuytema
-- created:   February 15, 2005
-- descrip:   Text file for Take Away
--=======================================

--define empty table
textTable = {}

--Loading text
textTable[GUI_LOADING + 200] = "Version 1.0"
textTable[GUI_LOADING + 201] = "Copyright 2005, Charles River Media"

--Ingame text
textTable[GUI_INGAME + 201] = "Projectiles Fired:"
textTable[GUI_INGAME + 202] = "Targets Lost:"
textTable[GUI_INGAME + 203] = "Score:"

--End game text
textTable[GUI_END_GAME + 204] = "Your Final Score Is"
textTable[GUI_END_GAME + 206] = "Congratulations! You placed in the Top Ten!"

--=======================================
-- function:  GetText
-- author:    Paul Schuytema
-- created:   February 15, 2005
-- returns:   string in textTable indexed by id
-- descrip:   retrieves text from the textTable
--=======================================
function GetText(localID)
    return textTable[localID]
end
