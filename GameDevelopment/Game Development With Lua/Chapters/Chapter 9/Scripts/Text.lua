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


--=======================================
-- function:  GetText
-- author:    Paul Schuytema
-- created:   February 15, 2005
-- returns:   string in textTable indexed by id
--=======================================
function GetText(localID)
    return textTable[localID]
end
