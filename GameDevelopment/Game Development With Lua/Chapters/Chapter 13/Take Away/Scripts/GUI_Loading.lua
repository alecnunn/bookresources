--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_Loading.lua
-- author:    Nick Carlson
-- created:   February 10, 2005
-- descrip:   Loading interface screen
--=======================================

-- first steps


--dofile calls for sub-GUIs


--100s Sprites
CreateItem(GUI_LOADING + 100, "Sprite", "ui_bg_loading.bmp")
SetItemPosition(GUI_LOADING + 100, 0, 0, 800, 600)

--200s Text items
CreateItem(GUI_LOADING + 200,"TextField")
SetItemPosition(GUI_LOADING + 200, 350, 20, 0, 0)
SetFont(GUI_LOADING + 200, "Arial", 22)
ItemCommand(GUI_LOADING + 200, "SetColor", 169,20,231,255)
ItemCommand(GUI_LOADING + 200, "SetString", GetText(GUI_LOADING + 200))

CreateItem(GUI_LOADING + 201,"TextField")
SetItemPosition(GUI_LOADING + 201, 245, 555, 0, 0)
SetFont(GUI_LOADING + 201, "Arial", 22)
ItemCommand(GUI_LOADING + 201, "SetColor", 169,20,231,255)
ItemCommand(GUI_LOADING + 201, "SetString", GetText(GUI_LOADING + 201))

loadString = "."

CreateItem(GUI_LOADING + 202,"TextField")
SetItemPosition(GUI_LOADING + 202, 275, 500, 0, 0)
SetFont(GUI_LOADING + 202, "Arial", 48)
ItemCommand(GUI_LOADING + 202, "SetColor", 177,174,255,255)
ItemCommand(GUI_LOADING + 202, "SetString", loadString)



--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--functions related to this GUI (not used anywhere else)


--first run calls
StartTimer(0.25)

SetEventHandler("LoadingMenuEvent")

--Event handler
function LoadingMenuEvent(id, eventCode)

    if eventCode == GUI_TIMER_EXPIRED then

        if string.len(loadString) < 21 then
            loadString = string.format("%s%s", loadString, ".")
            ItemCommand(GUI_LOADING + 202, "SetString", loadString)
            StartTimer(.15)
        else
            RunGUI("GUI_MainMenu.lua")
        end
    end
end  -- event handler
