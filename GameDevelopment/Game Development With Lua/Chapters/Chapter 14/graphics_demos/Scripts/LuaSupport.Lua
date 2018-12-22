--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  LuaSupport.lua
-- author:    Paul Schuytema
-- created:   March 17, 2005
-- descrip:   function files for AI pathfinding
--=======================================

--function ClearBox()


--end

function MakeBox(startX, startY, length, width)

    masterLineID = 101

    lineWidth = 2

    --set up the values

    --length = math.abs(length)
    --width = math.abs(width)



    --down

    CreateItem(masterLineID, "Sprite", "box2.bmp")
    SetItemPosition(masterLineID, startX, startY, lineWidth, width)

    --bottom

    masterLineID =  masterLineID + 1

    CreateItem(masterLineID, "Sprite", "box2.bmp")
    SetItemPosition(masterLineID, startX, startY + width, length + (lineWidth * 2), lineWidth)

    --top

    masterLineID =  masterLineID + 1

    CreateItem(masterLineID, "Sprite", "box2.bmp")
    SetItemPosition(masterLineID, startX + lineWidth, startY, length + lineWidth, lineWidth)

    --right

    masterLineID =  masterLineID + 1

    CreateItem(masterLineID, "Sprite", "box2.bmp")
    SetItemPosition(masterLineID, startX + length + lineWidth, startY, lineWidth, width)


end


function ApproachTarget(object, curX, curY, tarX, tarY, moveSpeed)

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

    --then, redraw the object:
    if object == "tank" then
        tankX = curX
        tankY = curY
        DrawObject("tank", tankX, tankY)
    end

    if type(object) == "number" then
        myProjectiles[object].X = curX
        myProjectiles[object].Y = curY
        DrawObject(object, myProjectiles[object].X, myProjectiles[object].Y)
    end
end


function FaceTarget(object, curX, curY, curRot, tarX, tarY)

    --first, figure out the sides of the triangle:
    a = curY - tarY
    b = curX - tarX
    c = math.sqrt((a*a)+(b*b))

    --now, find the angle to the target:
    angle = math.acos(b/c)

    --now, determine the prescence of negatives:
    if a > 0 then
        if b > 0 then
            angle = math.pi + angle   -- Upper-Left
            print("Upper-Left")
        elseif b == 0 then
            angle = 1.5 * math.pi    -- Directly Up
            print("Directly Up")
        else
            angle = math.pi + angle   -- Upper-Right
            print("Upper-Right")
        end
    elseif a == 0 then
        if b > 0 then
            angle = math.pi           -- Directly Left
            print("Directly Left")
        else
            angle = 2 * math.pi       -- Directly Right
            print("Directly Right")
        end
    else
        if b > 0 then
            angle = math.pi - angle   -- Lower-Left
            print("Lower-Left")
        elseif b == 0 then
            angle = .5 * math.pi      -- Directly Down
            print("Directly Down")
        else
            angle = math.pi - angle   -- Lower-Right
            print("Lower-Right")
        end
    end

    if curRot > angle then
        angleDif = curRot - angle
        if angleDif < math.pi then
            angleDif = "ccwise"
        else
            angleDif = "cwise"
        end
    else
        angleDif = angle - curRot
        if angleDif < math.pi then
            angleDif = "cwise"
        else
            angleDif = "ccwise"
        end
    end

    if angleDif == "ccwise" then
        curRot = curRot - .1
        if curRot < .1 then
            curRot = 6.2
        end
    else
        curRot = curRot + .1
        if curRot > 6.2 then
            curRot = 0
        end
    end

    if object == "tank" then
        tankRot = curRot
        ItemCommand(tankID, "SetRotation", tankRot)
        if angleDif == "ccwise" then
            turRot = turRot - .1
            if turRot < 0 then
                turRot = 6.2
            end
        else
            turRot = turRot + .1
            if turRot > 6.2 then
                turRot = 0
            end
        end
        ItemCommand(turID, "SetRotation", turRot)
    end

    if (curRot < (angle + .1)) and (curRot > (angle - .1)) then
        orientation = "done"
    else
        orientation = "processing"
    end
end


--=======================================
-- function:  ProjectileInit()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   creates myProjectiles table
--=======================================
function ProjectileInit()

    pCount = 50 --Number of projectiles in the game
    pIndx = 0 --Index of projectile in the myProjectiles table

    myProjectiles = {} --Creates myProjectiles table

    --Creates table, one entry per potential projectile
    for indx = 1,pCount do
        --Creates a table to hold the data for each projectile
        myProjectiles[indx] = {}
        myProjectiles[indx].ID = nil--GUI identification number (#)
        myProjectiles[indx].X = nil --X coordinate (#)
        myProjectiles[indx].Y = nil --Y coordinate (#)
        myProjectiles[indx].TARX = nil --X coordinate of the target (#)
        myProjectiles[indx].TARY = nil --Y coordinate target (#)
        myProjectiles[indx].SHIP = nil --Shooter ("player" or myEnemies index)
    end
end


--=======================================
-- function:  FireProjectile(ship, xThrust, yThrust)
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   creates a projectile and indexs it in myProjectiles table
--=======================================
function FireProjectile(curX, curY, curRot)

    pIndx = pIndx + 1
    myProjectiles[pIndx].ID = GUI_INGAME + 149 + pIndx --Starts IDs at 150

    --now, draw the projectile:
    myProjectiles[pIndx].X = curX + math.floor(42 * math.cos(curRot))
    myProjectiles[pIndx].Y = curY + math.floor(42 * math.sin(curRot))
    myProjectiles[pIndx].TARX = curX + math.floor(1000 * math.cos(curRot))
    myProjectiles[pIndx].TARY = curY + math.floor(1000 * math.sin(curRot))
    CreateItem(myProjectiles[pIndx].ID, "Sprite", "box1.bmp")
    --Update the display
    SetItemPosition(myProjectiles[pIndx].ID, myProjectiles[pIndx].X, myProjectiles[pIndx].Y, 4, 4)
--    ItemCommand(GUI_INGAME + 204, "SetString", CommaFormatBigInteger(playerProjectiles))
    if pIndx == pCount then
        pIndx = 0
    end
end


function DrawObject(object, x, y)

    if object == "tank" then
        SetItemPosition(tankID, x, y, 78, 34)
        SetItemPosition(turID, x+turXOff, y+turYOff, 83, 28)
    end

    if type(object) == "number" then
        SetItemPosition(myProjectiles[object].ID, x, y, 4, 4)
    end
end


--=======================================
-- function:  HutInit()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   creates myHuts table
--=======================================
function HutInit()

    hutCount = 3 --Number of huts in the game

    myHuts = {} --Creates myHuts table

    --Creates table, one entry per potential hut
    for indx = 1,hutCount do
        --Creates a table to hold the data for each hut
        myHuts[indx] = {}
        myHuts[indx].ID = GUI_INGAME + 100 + indx--GUI identification number (#)
        myHuts[indx].X = math.random(0,760) --X coordinate (#)
        myHuts[indx].Y = math.random(0,520) --Y coordinate (#)
        if (myHuts[indx].X > 325) and (myHuts[indx].X < 500) then
            myHuts[indx].X = 500
        end
        myHuts[indx].DAMAGE = 0
        CreateItem(myHuts[indx].ID, "Sprite", "hut1.bmp")
        SetItemPosition(myHuts[indx].ID, myHuts[indx].X, myHuts[indx].Y, 40, 80)
    end
end


function HutHit()
    for indx = 1, hutCount do
        if myHuts[indx].DAMAGE < 2 then
            hitTable = GetCollisions(myHuts[indx].ID)
            for index,value in ipairs(hitTable) do
                for i = 1, pCount do
                    if value == myProjectiles[i].ID then
                        DeleteItem(myProjectiles[i].ID)
                        myHuts[indx].DAMAGE = myHuts[indx].DAMAGE + 1
                        if myHuts[indx].DAMAGE == 1 then
                            SetTexture(myHuts[indx].ID, "hut2.bmp")
                        elseif myHuts[indx].DAMAGE == 2 then
                            explosionFrame = 1
                            burningHut = indx
                            SetTexture(myHuts[indx].ID, "hut3.bmp")
                        end
                    end
                end
            end
        end
    end
end
