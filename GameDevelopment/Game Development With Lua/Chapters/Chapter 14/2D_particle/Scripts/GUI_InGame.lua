--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_InGame.lua
-- author:    Paul Schuytema
-- created:   February 10, 2005
-- descrip:   In game play screen and interfaces
--=======================================

-- first steps


--dofile calls for sub-GUIs


--100s Sprites







--200s Text items

CreateItem(GUI_INGAME + 200,"TextField")
SetItemPosition(GUI_INGAME + 200, 10, 10, 200, 20)
SetFont(GUI_INGAME + 200, "Arial", 24)
ItemCommand(GUI_INGAME + 200, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 200, "SetString", "{particle demo}")

CreateItem(GUI_INGAME + 202,"TextField")
SetItemPosition(GUI_INGAME + 202, 10, 45, 200, 20)
SetFont(GUI_INGAME + 202, "Arial", 24)
ItemCommand(GUI_INGAME + 202, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 202, "SetString", "left click to place particle emitter")






--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)



--Initial run sets the event handler
SetEventHandler("InGameEvent")


--Event handler
function InGameEvent(id, eventCode)

    if eventCode == GUI_ENTER_INTERFACE then
    end

    if eventCode == GUI_KEY_PRESS then
        if id == 32 then --space bar
        end

        if id == 13 then --enter key
        end

        if id == 104 then -- h key
        end

        if id == 115 then -- s key
        end

    end

    if eventCode == GUI_MOUSE_BUTTON_DOWN then
    end

    if eventCode == GUI_MOUSE_BUTTON_UP then
        partSystem.X,partSystem.Y = GetMousePosition()
        InitSystem()
        StartTimer(timeSlice)
    end

    if eventCode == GUI_TIMER_EXPIRED then
        ProcessParticles()
    end


end
