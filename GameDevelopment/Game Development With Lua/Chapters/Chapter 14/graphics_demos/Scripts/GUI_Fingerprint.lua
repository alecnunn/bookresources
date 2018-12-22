--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_Fingerprint.lua
-- author:    Nick Carlson
-- created:   April 15, 2005
-- descrip:   Sprite animation demonstration
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


--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)
local printCounter = 1
local printDir = POSITIVE


--Functions
function AnimatePrint()

    local pic = "ui_tp_01.bmp"
    if printCounter < 10 then
        pic = string.format("%s%d%s","ui_tp_0",printCounter,".bmp")
    else
        pic = string.format("%s%d%s","ui_tp_",printCounter,".bmp")
    end
    CreateItem(GUI_INGAME + 101, "Sprite", pic)
    SetItemPosition(GUI_INGAME + 101, 264, 120, 272, 360)

    StartTimer(.09)
end


--Initial run sets the event handler
SetEventHandler("FingerprintEvent")


--Event handler
function FingerprintEvent(id, eventCode)

    --print(eventCode)

    if eventCode == GUI_ENTER_INTERFACE then
        StartTimer(.09)
    end

    if eventCode == GUI_KEY_PRESS then

        if id == 27 then --Esc
            RunGUI("GUI_MainMenu.lua")
        end
    end

    if eventCode == GUI_TIMER_EXPIRED then

        if printDir == POSITIVE then
           printCounter = printCounter + 1
           if printCounter > 17 then  -- max number of 2d pics, so reverse direction
            printDir = NEGATIVE
            printCounter = 16
           end
        else
           printCounter = printCounter - 1
           if printCounter < 1 then  -- min number of  2d pics, so reverse direction
            printDir = POSITIVE
            printCounter = 2
           end
        end
        AnimatePrint()
    end
end
