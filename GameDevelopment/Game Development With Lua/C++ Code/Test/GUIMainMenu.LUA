--=======================================
-- (c) copyright 2004, Magic Lantern, Inc
-- All Rights Reserved.  U.S.A.
--=======================================--
--  filename: MainMenu.lua
--  author:   Paul Schuytema
--  created:  May 6, 2004
--  descrip:  the main navigation menu
--=======================================--



SetCoordTrans(800,600)


--background
CreateItem(1, "Sprite", "test.bmp")
SetItemPosition(1, 0, 0, 800, 600)

CreateItem(101,"TextField")
SetItemPosition(101, 320, 230, 250, 26)
SetFont(101, "Arial", 20)
ItemCommand(101, "SetColor", 255,0,255,255)
ItemCommand(101, "SetString", "     Version 1.184")

SetEventHandler("MainMenuEvent")


function MainMenuEvent(id, eventCode)

    if (eventCode == GUI_REENTER_INTERFACE)  or (eventCode == GUI_ENTER_INTERFACE) then

    end

    if eventCode == GUI_KEY_PRESS then

    end

    if eventCode == GUI_EVENT_BUTTON_UP then


    end

end
