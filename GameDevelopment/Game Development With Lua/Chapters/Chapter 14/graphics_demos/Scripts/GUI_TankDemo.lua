--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_TankDemo.lua
-- author:    Nick Carlson
-- created:   April 15, 2005
-- descrip:   Culmination of graphics chapter
--=======================================

-- first steps


--dofile calls for sub-GUIs


--100s Sprites
--background
CreateItem(100, "Sprite", "map1.bmp")
SetItemPosition(100, 0, 0, 800, 600)

--tank
tankID = GUI_INGAME + 110
tankX = 400
tankY = 300
tankRot = 0
tankSpeed = 3
CreateItem(tankID, "Sprite", "tank_body.bmp")
SetItemPosition(tankID, tankX, tankY, 78, 34)

--tank turret
turID = GUI_INGAME + 120
turXOff = -10
turYOff = 3
turRot = 0
CreateItem(turID, "Sprite", "tank_turret.tga")
SetItemPosition(turID, tankX+turXOff, tankY+turYOff, 83, 28)

--huts
--GUI IDs 101-103


--projectiles
--GUI IDs 150-199
proSpeed = 20

--explosion
explosionFrame = 0



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
ItemCommand(GUI_INGAME + 201, "SetString", "Use The 'p' Key To Incinerate The Huts")


--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--myX = 0
--myY = 0
--myLength = 1
--myWidth = 1


--Initial run sets the event handler
SetEventHandler("TankDemoEvent")


--Event handler
function TankDemoEvent(id, eventCode)

    --print(eventCode)

    if eventCode == GUI_ENTER_INTERFACE then
        HutInit()
        ProjectileInit()
        StartTimer(.1)
        --MakeBox(50, 50, 100, 50)

    end

    if eventCode == GUI_KEY_PRESS then


            --ItemCommand(GUI_INGAME + 205, "SetString", " ")
            --ItemCommand(GUI_INGAME + 206, "SetString", " ")

        if id == 27 then --Esc
            RunGUI("GUI_MainMenu.lua")
        end

        if id == 112 then --p
            FireProjectile(tankX+turXOff+41.5, tankY+turYOff+14, turRot)
        end

        if id == 91 then --[
            turRot = turRot + .1
            if turRot > 6.2 then
                turRot = 0
            end
            ItemCommand(turID, "SetRotation", turRot)
        end

        if id == 93 then --]
            turRot = turRot - .1
            if turRot < 0 then
                turRot = 6.2
            end
            ItemCommand(turID, "SetRotation", turRot)
        end
    end

    if eventCode == GUI_MOUSE_BUTTON_DOWN then
        if id == LEFT then
            --ItemCommand(GUI_INGAME + 205, "SetString", "action::down")
            --ItemCommand(GUI_INGAME + 206, "SetString", "boooo")
            --master = "drawing"
            --myX, myY = GetMousePosition()
            --StartTimer(.1)
        end
    end

    if eventCode == GUI_MOUSE_BUTTON_UP then
        --x = x + 1
        if id == LEFT then
            --ItemCommand(GUI_INGAME + 205, "SetString", "action::up")
            --tempX, tempY = GetMousePosition()
            --myLength = tempX - myX
           -- myWidth = tempY - myY
            --MakeBox(myX, myY, myLength, myWidth)
            --master = "done"
            newTarX, newTarY = GetMousePosition()
            if (newTarX ~= tarX) or (newTarY ~= tarY) then
                tarX = newTarX
                tarY = newTarY
                orientation = "processing"
            end
        end
        --ItemCommand(GUI_INGAME + 205, "SetString", string.format("%d", x))
    end

    if eventCode == GUI_TIMER_EXPIRED then
        if ((tarX ~= nil) and (tarY ~= nil)) then
            if orientation == "done" then
                ApproachTarget("tank", tankX, tankY, tarX, tarY, tankSpeed)
                hitTable = HitTest(tarX, tarY)
                for index,value in ipairs(hitTable) do
                    if value == tankID then
                        tarX = nil
                        tarY = nil
                    end
                end
            else
                FaceTarget("tank", tankX, tankY, tankRot, tarX, tarY)
            end
        end

        for indx = 1,pCount do
            if myProjectiles[indx].ID ~= nil then
                ApproachTarget(indx, myProjectiles[indx].X, myProjectiles[indx].Y, myProjectiles[indx].TARX, myProjectiles[indx].TARY, proSpeed)
            end
        end

        HutHit()

        if explosionFrame > 0 then
            if explosionFrame < 10 then
                pic = string.format("%s%d%s","kaboom_0",explosionFrame,".tga")
            else
                pic = string.format("%s%d%s","kaboom_",explosionFrame,".tga")
            end
            CreateItem(GUI_INGAME + 105, "Sprite", pic)
            SetItemPosition(GUI_INGAME + 105, myHuts[burningHut].X+1, myHuts[burningHut].Y+21, 38, 38)
            explosionFrame = explosionFrame + 1
            if explosionFrame > 13 then
                DeleteItem(GUI_INGAME + 105)
                explosionFrame = 0
            end
        end

        StartTimer(.1)
    end

end
