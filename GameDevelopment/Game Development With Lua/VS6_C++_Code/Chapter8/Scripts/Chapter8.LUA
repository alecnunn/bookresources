--=======================================
-- (c) copyright 2005, Magic Lantern, Inc
-- All Rights Reserved.  U.S.A.
--=======================================--
--  filename: Chapter7.lua
--  author:   Paul Schuytema
--  created:  Feb 6, 2005
--  descrip:  Example
--=======================================--



--SetCoordTrans(800,600)
CreateItem(1, "Sprite", "test.bmp")
SetItemPosition(1, 100, 100, 128, 64)
rot = 3.14
ItemCommand(1, "SetRotation", rot);

--CreateItem(55, "Button", "button1.bmp", "button1_h.bmp", "button1_d.bmp");
--SetItemPosition(55, 100, 100, 64, 32);

SetEventHandler("Chapter7Event")




CreateItem(11, "Sprite", "test.bmp")
SetItemPosition(11, 110, 110, 128, 64)

CreateItem(12, "Sprite", "test.bmp")
SetItemPosition(12, 90, 90, 128, 64)



t = GetCollisions(1)
print(t);


function Chapter7Event(id, eventCode)

    if eventCode == GUI_KEY_PRESS then

		rot = rot + 0.1;
		ItemCommand(1, "SetRotation", rot);


		print "Hello";

--		QuitProgram();
    end

end
