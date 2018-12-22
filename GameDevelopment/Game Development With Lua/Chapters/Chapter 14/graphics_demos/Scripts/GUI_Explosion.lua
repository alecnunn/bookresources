--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_Explosion.lua
-- author:    Nick Carlson
-- created:   April 15, 2005
-- descrip:   Interactive sprite animation
--=======================================

-- first steps


--dofile calls for sub-GUIs


--100s Sprites
--background
CreateItem(100, "Sprite", "map1.bmp")
SetItemPosition(100, 0, 0, 800, 600)


--200s Text items

CreateItem(GUI_INGAME + 200,"TextField")
SetItemPosition(GUI_INGAME + 200, 10, 10, 200, 20)
SetFont(GUI_INGAME + 200, "Arial", 24)
ItemCommand(GUI_INGAME + 200, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 200, "SetString", string.format("%s%s%s","{", gScenarioScript, "}"))

CreateItem(GUI_INGAME + 201,"TextField")
SetItemPosition(GUI_INGAME + 201, 200, 500, 400, 20)
SetFont(GUI_INGAME + 201, "Arial", 30)
ItemCommand(GUI_INGAME + 201, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 201, "SetString", "Left-Click To View An Explosion")


--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)
local explosionFrame = 0


--Initial run sets the event handler
SetEventHandler("ExplosionEvent")


--Event handler
function ExplosionEvent(id, eventCode)

    --print(eventCode)

    if eventCode == GUI_ENTER_INTERFACE then
        StartTimer(.04)
    end

    if eventCode == GUI_KEY_PRESS then

        if id == 27 then --Esc
            RunGUI("GUI_MainMenu.lua")
        end
    end

    if eventCode == GUI_MOUSE_BUTTON_UP then
        if id == LEFT then
            if explosionFrame == 0 then
                tarX, tarY = GetMousePosition()
                explosionFrame = 1
            end
        end
    end

    if eventCode == GUI_TIMER_EXPIRED then
        if explosionFrame > 0 then
            if explosionFrame < 10 then
                pic = string.format("%s%d%s","kaboom_0",explosionFrame,".tga")
            else
                pic = string.format("%s%d%s","kaboom_",explosionFrame,".tga")
            end
            CreateItem(GUI_INGAME + 105, "Sprite", pic)
            SetItemPosition(GUI_INGAME + 105, tarX-19, tarY-19, 38, 38)
            explosionFrame = explosionFrame + 1
            if explosionFrame > 13 then
                DeleteItem(GUI_INGAME + 105)
                explosionFrame = 0
            end
        end
        StartTimer(.04)
    end
end
