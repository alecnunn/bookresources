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
SetFont(GUI_INGAME + 200, "Arial", 16)
ItemCommand(GUI_INGAME + 200, "SetColor", 200,200,200,255)
ItemCommand(GUI_INGAME + 200, "SetString", "{path test}")

CreateItem(GUI_INGAME + 205,"TextField")
SetItemPosition(GUI_INGAME + 205, 10, 25, 200, 20)
SetFont(GUI_INGAME + 205, "Arial", 16)
ItemCommand(GUI_INGAME + 205, "SetColor", 200,200,200,255)
ItemCommand(GUI_INGAME + 205, "SetString", "mode::place start")


--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--functions related to this GUI (not used anywhere else)

function ClearWorld()
    for indxID = startCellID, masterCellID do
        DeleteItem(indxID)
    end
end

function RefreshWorld()
    ClearWorld()
    DefineWorld()
    InitWorld()
    --DefineStart(2,2)
    --DrawStart(startX,startY)
end

function PlaceTarget(myX, myY)
    RefreshWorld()
    DrawStart(startX,startY)
    DefineStart(startX, startY)
    myX = math.floor(myX/20)
    myY = math.floor(myY/20)
    if (myX < maxX-1) and (myX > 1) and (myY < maxY-1) and (myY > 1) then
        if world[myY][myX] ~= 1 then
            DefineTarget(myY, myX)
            DrawTarget(targetX,targetY)
            --is the line below needed?
            --open[1].H = 10*(math.abs(startX-targetX) + math.abs(startY-targetY))
            repeat
                targetFound = FindPath()
            until targetFound
            BuildPath()
            DrawPath(path)
        end
    end
end

function PlaceStart(myX, myY)
    RefreshWorld()
    myX = math.floor(myX/20)
    myY = math.floor(myY/20)
    if (myX < maxX-1) and (myX > 1) and (myY < maxY-1) and (myY > 1) then
        if world[myY][myX] ~= 1 then
            DefineStart(myY, myX)
            DrawStart(startX,startY)
        end
    end
end

modeValue = "start"


--Initial run sets the event handler
SetEventHandler("InGameEvent")


--Event handler
function InGameEvent(id, eventCode)

    if eventCode == GUI_ENTER_INTERFACE then
        --PathTest()
        DefineWorld()
        InitWorld()
        --DefineStart(2,2)
        --DrawStart(startX,startY)
    end

    if eventCode == GUI_KEY_PRESS then
        if id == 32 then --space
            if modeValue == "target" then
                modeValue = "start"
                ItemCommand(GUI_INGAME + 205, "SetString", "mode::place start")
            else
                modeValue = "target"
                ItemCommand(GUI_INGAME + 205, "SetString", "mode::place target")
            end
        end

        if id == 13 then --return
            RefreshWorld()
            modeValue = "start"
            ItemCommand(GUI_INGAME + 205, "SetString", "mode::place start")
        end
    end

    if GUI_MOUSE_BUTTON_UP then
        if id == 2 then
            windowX, windowY = GetMousePosition()
            if modeValue == "target" then
                PlaceTarget(windowX, windowY)
            else
                PlaceStart(windowX, windowY)
            end
        end
    end


end
