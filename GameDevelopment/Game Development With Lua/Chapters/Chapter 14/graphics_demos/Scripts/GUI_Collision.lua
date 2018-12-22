--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_Collison.lua
-- author:    Nick Carlson
-- created:   April 15, 2005
-- descrip:   Collsion demonstration
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
tankSpeed = 3
CreateItem(tankID, "Sprite", "tank_body.bmp")
SetItemPosition(tankID, tankX, tankY, 78, 34)


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
ItemCommand(GUI_INGAME + 201, "SetString", "Left-Click On A Hut To Dismantle It")


--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--Functions

function CollisonApproachTarget(curX, curY, tarX, tarY, moveSpeed)

    --first, figure out the sides of the triangle:
    a = curY - tarY
    b = curX - tarX
    c = math.sqrt((a*a)+(b*b))

    --now, find the angle to the target:
    angle = math.acos(b/c)

    --now, determine the prescence of negatives:
    if a > 0 then
        yNeg = -1
        if b > 0 then
            xNeg = -1
        elseif b == 0 then
            xNeg = 0
        else
            xNeg = -1
        end
    elseif a == 0 then
        yNeg = 1
        if b > 0 then
            xNeg = -1
        else
            xNeg = -1
        end
    else
        yNeg = 1
        if b > 0 then
            xNeg = -1
        elseif b == 0 then
            xNeg = 0
        else
            xNeg = -1
        end
    end

    --now, determine the x movement:
    movX = math.floor(moveSpeed * math.cos(angle) * xNeg)
    movY = math.floor(moveSpeed * math.sin(angle) * yNeg)

    --now, update your current position:
    curX = curX + movX
    curY = curY + movY

    tankX = curX
    tankY = curY

    SetItemPosition(tankID, tankX, tankY, 78, 34)
end


function CollisonCheck()
    for indx = 1, hutCount do
        if myHuts[indx].DAMAGE ~= 1 then
            hitTable = GetCollisions(myHuts[indx].ID)
            for index,value in ipairs(hitTable) do
                if value == tankID then
                    myHuts[indx].DAMAGE = 1
                    SetTexture(myHuts[indx].ID, "hut3.bmp")
                end
            end
        end
    end
end


--Initial run sets the event handler
SetEventHandler("CollisionEvent")


--Event handler
function CollisionEvent(id, eventCode)

    --print(eventCode)

    if eventCode == GUI_ENTER_INTERFACE then
        HutInit()
        StartTimer(.1)
    end

    if eventCode == GUI_KEY_PRESS then
        if id == 27 then --Esc
            RunGUI("GUI_MainMenu.lua")
        end
    end

    if eventCode == GUI_MOUSE_BUTTON_UP then
        if id == LEFT then
            tarX, tarY = GetMousePosition()
        end
    end

    if eventCode == GUI_TIMER_EXPIRED then
        if ((tarX ~= nil) and (tarY ~= nil)) then
            CollisonApproachTarget(tankX, tankY, tarX, tarY, tankSpeed)
            hitTable = HitTest(tarX, tarY)
            for index,value in ipairs(hitTable) do
                if value == tankID then
                    tarX = nil
                    tarY = nil
                end
            end
            CollisonCheck()
        end
        StartTimer(.1)
    end
end
