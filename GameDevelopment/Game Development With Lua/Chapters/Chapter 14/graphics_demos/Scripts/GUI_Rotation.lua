--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_Rotation.lua
-- author:    Nick Carlson
-- created:   April 15, 2005
-- descrip:   Sprite rotation demonstration
--=======================================

-- first steps


--dofile calls for sub-GUIs


--100s Sprites
--background
CreateItem(100, "Sprite", "map2.bmp")
SetItemPosition(100, 0, 0, 800, 600)

--tank
tankID = GUI_INGAME + 110
tankX = 400
tankY = 300
tankRot = 0
CreateItem(tankID, "Sprite", "tank_body.bmp")
SetItemPosition(tankID, tankX, tankY, 78, 34)


--200s Text items

CreateItem(GUI_INGAME + 200,"TextField")
SetItemPosition(GUI_INGAME + 200, 10, 10, 200, 20)
SetFont(GUI_INGAME + 200, "Arial", 24)
ItemCommand(GUI_INGAME + 200, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 200, "SetString", string.format("%s%s%s","{", gScenarioScript, "}"))

CreateItem(GUI_INGAME + 201,"TextField")
SetItemPosition(GUI_INGAME + 201, 110, 500, 400, 20)
SetFont(GUI_INGAME + 201, "Arial", 30)
ItemCommand(GUI_INGAME + 201, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 201, "SetString", "Use The '[' and ']' Keys To Rotate The Tank Frame")


--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--Initial run sets the event handler
SetEventHandler("RotationEvent")


--Event handler
function RotationEvent(id, eventCode)

    --print(eventCode)

    if eventCode == GUI_ENTER_INTERFACE then

    end

    if eventCode == GUI_KEY_PRESS then

        if id == 27 then --Esc
            RunGUI("GUI_MainMenu.lua")
        end

        if id == 91 then --[
            tankRot = tankRot + .1
            if tankRot > 6.2 then
                tankRot = 0
            end
            ItemCommand(tankID, "SetRotation", tankRot)
        end

        if id == 93 then --]
            tankRot = tankRot - .1
            if tankRot < 0 then
                tankRot = 6.2
            end
            ItemCommand(tankID, "SetRotation", tankRot)
        end
    end
end
