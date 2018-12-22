--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_EndGame.lua
-- author:    Paul Schuytema
-- created:   February 12, 2005
-- descrip:   End Game screen that shows final score
--=======================================

-- first steps


--dofile calls for sub-GUIs


--100s Sprites
CreateItem(GUI_END_GAME + 100, "Sprite", "ui_bg_endgame.bmp")
SetItemPosition(GUI_END_GAME + 100, 0, 0, 800, 600)

--200s Text items
--Game over display
CreateItem(GUI_END_GAME + 204,"TextField")
SetItemPosition(GUI_END_GAME + 204, 274, 338, 294, 24)
SetFont(GUI_END_GAME + 204, "Arial", 25)
ItemCommand(GUI_END_GAME + 204, "SetColor", 218,216,253,255)
ItemCommand(GUI_END_GAME + 204, "SetString", GetText(GUI_END_GAME + 204))

--Score amount
CreateItem(GUI_END_GAME + 205,"TextField")
SetItemPosition(GUI_END_GAME + 205, 465, 338, 294, 24)
SetFont(GUI_END_GAME + 205, "Arial", 25)
ItemCommand(GUI_END_GAME + 205, "SetColor", 218,216,253,255)
ItemCommand(GUI_END_GAME + 205, "SetString", "")

--Top ten notice
CreateItem(GUI_END_GAME + 206,"TextField")
SetItemPosition(GUI_END_GAME + 206, 215, 375, 325, 20)
SetFont(GUI_END_GAME + 206, "Arial", 22)
ItemCommand(GUI_END_GAME + 206, "SetColor", 218,216,253,255)
ItemCommand(GUI_END_GAME + 206, "SetString", GetText(GUI_END_GAME + 206))


--300s Buttons

--to go back to main menu
CreateItem(GUI_END_GAME + 300, "Button", "uib_tomain_up.bmp", "uib_tomain_hv.bmp", "uib_tomain_dn.bmp");
SetItemPosition(GUI_END_GAME + 300, 452, 493, 293, 67)

--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--functions related to this GUI (not used anywhere else)


--Initial run sets the event handler
SetEventHandler("EndGameEvent")


--Event handler
function EndGameEvent(id, eventCode)

    if (eventCode == GUI_REENTER_INTERFACE)  or (eventCode == GUI_ENTER_INTERFACE) then
        ItemCommand(GUI_END_GAME + 205, "SetString", CommaFormatBigInteger(score))
        if SendHighScore(score) == "yes" then
            EnableObject(GUI_END_GAME + 206, 1, 1)
        else
            EnableObject(GUI_END_GAME + 206, 0, 0)
        end
    end

    if eventCode == GUI_EVENT_BUTTON_UP then

        if id == GUI_END_GAME + 300 then
            PlaySound(1, "button.wav")
            gEscapeOn = 0
            RunGUI("GUI_MainMenu.lua")
            --ShowCursor()
        end
    end
end
