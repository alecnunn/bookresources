--=======================================
-- (c) copyright 2005, Magic Lantern, Inc
-- All Rights Reserved.  U.S.A.
--=======================================--
--  filename: Chapter7.lua
--  author:   Paul Schuytema
--  created:  Feb 6, 2005
--  descrip:  Example
--=======================================--

SetEventHandler("Chapter8Event")


function Chapter8Event(id, eventCode)

    if eventCode == GUI_KEY_PRESS then

	print "Hello";
	QuitProgram();
    end

end
