--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  LuaSupport.lua
-- author:    Nick Carlson
-- created:   February 10, 2005
-- descrip:   ny functions that are required are defined here.
--=======================================


--=======================================
-- function:  GetTravelDirection(xThrust, yThrust)
-- author:    Paul Schuytema
-- created:   February 1, 2005
-- returns:   direction: ship's orientation (1-8)
-- descrip:   finds the ship's travel direction
--=======================================
function GetTravelDirection(xThrust, yThrust)
--xThrust is a ship's thrust along the x-axis
--yThrust is a ship's thrust along the y-axis

    direction = 1
    if math.abs(xThrust) >= math.abs(yThrust) then
        --Left/right
        lrPercent =  (math.abs(yThrust)/math.abs(xThrust)) * 100
        if lrPercent < 30 then
            if xThrust < 0 then
                direction = 7 --Left
            else
                direction = 3 --Right
            end
        else
            if xThrust < 0 then
                if yThrust < 0 then
                    direction = 8 --Up/left
                else
                    direction = 6 --Down/left
                end
            else
                if yThrust < 0 then
                    direction = 2 --Up/right
                else
                    direction = 4 --Down/right
                end
            end
        end

    else
        --up/down
        udPercent =  (math.abs(xThrust)/math.abs(yThrust)) * 100
        if udPercent < 30 then
            if yThrust < 0 then
                direction = 1 --Up
            else
                direction = 5 --Down
            end
        else
            if yThrust < 0 then
                if xThrust < 0 then
                    direction = 8 --Up/left
                else
                    direction = 2 --Up/right
                end
            else
                if xThrust < 0 then
                    direction = 6 --Down/left
                else
                    direction = 4 --Down/right
                end
            end
        end
    end
    return direction
end


--=======================================
-- function:  CollisionCheck(ship, x, y)
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   checks for collisions with player and enemy world boundaries
--=======================================
function CollisionCheck(ship, x, y)
--ship can either be "player" or the myEnemies index of the enemy
--x is the ship's X coordinate
--y is the ship's Y coordinate

    collision = NO
    if ship == "player" then
        --Player
        if (y <= 20) then
            myY = 25
            collision = HORIZONTAL
        elseif (y >= 560) then
            myY = 555
            collision = HORIZONTAL
        elseif (x <= 20) then
            myX = 25
            collision = VERTICAL
        elseif (x >= 760) then
            myX = 755
            collision = VERTICAL
        end
        travelDir = GetTravelDirection (myXThrust, myYThrust)
        --Set local variables to global values
        XThrust = myXThrust
        YThrust = myYThrust
    else
        --Enemy
        if (y <= -60) then
            myEnemies[ship].EY = -55
            collision = HORIZONTAL
        elseif (y >= 640) then
            myEnemies[ship].EY = 635
            collision = HORIZONTAL
        elseif (x <= -60) then
            myEnemies[ship].EX = -55
            collision = VERTICAL
        elseif (x >= 840) then
            myEnemies[ship].EX = 835
            collision = VERTICAL
        end
        travelDir = GetTravelDirection (myEnemies[ship].XTHRUST, myEnemies[ship].YTHRUST)
        --Set local variables equal to global values
        XThrust = myEnemies[ship].XTHRUST
        YThrust = myEnemies[ship].YTHRUST
    end

    if collision > NO then
    --Set thrusts based on travel direction and collision
        if collision == HORIZONTAL then
            if travelDir == 1 then
                YThrust = YThrust * -1
            end
            if travelDir == 2 then
                YThrust = YThrust * -1
            end
            if travelDir == 3 then
                XThrust = XThrust * -1
            end
            if travelDir == 4 then
                YThrust = YThrust * -1
            end
            if travelDir == 5 then
                YThrust = YThrust * -1
            end
            if travelDir == 6 then
                YThrust = YThrust * -1
            end
            if travelDir == 7 then
                XThrust = XThrust * -1
            end
            if travelDir == 8 then
                YThrust = YThrust * -1
            end
        elseif collision == VERTICAL then
            if travelDir == 1 then
                YThrust = YThrust * -1
            end
            if travelDir == 2 then
                XThrust = XThrust * -1
            end
            if travelDir == 3 then
                XThrust = XThrust * -1
            end
            if travelDir == 4 then
                XThrust = XThrust * -1
            end
            if travelDir == 5 then
                YThrust = YThrust * -1
            end
            if travelDir == 6 then
                XThrust = XThrust * -1
            end
            if travelDir == 7 then
                XThrust = XThrust * -1
            end
            if travelDir == 8 then
                XThrust = XThrust * -1
            end
        end

        --Set global variables equal to local values
        if ship == "player" then
            myXThrust = XThrust
            myYThrust = YThrust
        else
            myEnemies[ship].XTHRUST = XThrust
            myEnemies[ship].YTHRUST = YThrust
        end
    end
end


--=======================================
-- function:  DrawEnemyShip()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   refreshes the image of the enemy ship
--=======================================
function DrawEnemyShip()

    for indx = 1,enemyCount do
        if myEnemies[indx].ID ~= nil then
            DeleteItem(myEnemies[indx].ID)
            --Determine enemy's new thrust
            newThrust = math.random(0,2)
            if myEnemies[indx].ROT == 1 then --Up
                myEnemies[indx].YTHRUST = myEnemies[indx].YTHRUST - newThrust
            end
            if myEnemies[indx].ROT == 2 then --Up/right
                myEnemies[indx].YTHRUST = myEnemies[indx].YTHRUST - newThrust
                myEnemies[indx].XTHRUST = myEnemies[indx].XTHRUST + newThrust
            end
            if myEnemies[indx].ROT == 3 then --Right
                myEnemies[indx].XTHRUST = myEnemies[indx].XTHRUST + newThrust
            end
            if myEnemies[indx].ROT == 4 then --Down/right
                myEnemies[indx].YTHRUST = myEnemies[indx].YTHRUST + newThrust
                myEnemies[indx].XTHRUST = myEnemies[indx].XTHRUST + newThrust
            end
            if myEnemies[indx].ROT == 5 then --Down
                myEnemies[indx].YTHRUST = myEnemies[indx].YTHRUST + newThrust
            end
            if myEnemies[indx].ROT == 6 then --Down/left
                myEnemies[indx].YTHRUST = myEnemies[indx].YTHRUST + newThrust
                myEnemies[indx].XTHRUST = myEnemies[indx].XTHRUST - newThrust
            end
            if myEnemies[indx].ROT == 7 then --Left
                myEnemies[indx].XTHRUST = myEnemies[indx].XTHRUST - newThrust
            end
            if myEnemies[indx].ROT == 8 then --Up/left
                myEnemies[indx].YTHRUST = myEnemies[indx].YTHRUST - newThrust
                myEnemies[indx].XTHRUST = myEnemies[indx].XTHRUST - newThrust
            end
            --Check new thrust against enemy's maximum thrust
            if myEnemies[indx].YTHRUST > myEnemies[indx].MAX then
                myEnemies[indx].YTHRUST = myEnemies[indx].MAX
            end
            if myEnemies[indx].YTHRUST < (-1 * myEnemies[indx].MAX) then
                myEnemies[indx].YTHRUST = -1 * myEnemies[indx].MAX
            end
            if myEnemies[indx].XTHRUST > myEnemies[indx].MAX then
                myEnemies[indx].XTHRUST = myEnemies[indx].MAX
            end
            if myEnemies[indx].XTHRUST < (-1 * myEnemies[indx].MAX) then
                myEnemies[indx].XTHRUST = -1 * myEnemies[indx].MAX
            end
            --Checks for collisions and displays the new enemy ship
            shipName = string.format("%s%d%s%d%s", "e", myEnemies[indx].TYPE, "_ship", myEnemies[indx].ROT, ".bmp")
            myEnemies[indx].EX = myEnemies[indx].EX + myEnemies[indx].XTHRUST
            myEnemies[indx].EY = myEnemies[indx].EY + myEnemies[indx].YTHRUST
            CollisionCheck(indx, myEnemies[indx].EX, myEnemies[indx].EY)
            CreateItem(myEnemies[indx].ID, "Sprite", shipName)
            SetItemPosition(myEnemies[indx].ID, myEnemies[indx].EX, myEnemies[indx].EY, 20, 20)
        end
    end
end


--=======================================
-- function:  EnemyTowCheck()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   checks for collisions between the enemies and the targets
--=======================================
function EnemyTowCheck()

    towCounter = 0 --Counts collisions and acts as an index in towTable table
    towTable = {}

    --Creates table, one entry per potential collision
    for indx = 1,enemyCount do
        --Creates a table to hold the data for each collision
        towTable[indx] = {}
        towTable[indx].TE_INDX = 0 --The myEnemies table index of the Towing Enemy (#)
        towTable[indx].TT_INDX = 0 --The myTargets table index of the Towed Projectile (#)
    end

    for i = 1,enemyCount do
        if myEnemies[i].ID ~= nil then
            for j = 1, targetCount do
                if (myTargets[j].T_TOW == "no") and (myEnemies[i].E_TOW == "no") and ((myEnemies[i].TYPE == 1) or (myEnemies[i].TYPE == 4)) then --Enemies are box grabbers or hybrids & the enemy and target are free for towing
                    if ((myEnemies[i].EX + 20) < myTargets[j].T_X) or (myEnemies[i].EX > (myTargets[j].T_X + 20)) or ((myEnemies[i].EY + 20) < myTargets[j].T_Y) or (myEnemies[i].EY > (myTargets[j].T_Y + 20)) then
                        --No collision
                    else
                        --Collision
                        towCounter = towCounter + 1
                        towTable[towCounter].TE_INDX = i --Indexes the myEnemies index of the towing enemy
                        towTable[towCounter].TT_INDX = j --Indexes the myTargets index of the towed projectile
                    end
                end
            end
        end
    end
    --Sorts through the towTable table
    for indx = 1,towCounter do
        myTowTargetIndx = towTable[indx].TT_INDX
        myTowEnemyIndx = towTable[indx].TE_INDX
        if myEnemies[myTowEnemyIndx].E_TOW == "no" then
            --Indexes the ID of each entity into the appropriate portion of the other's table
            myTargets[myTowTargetIndx].T_TOW = myEnemies[myTowEnemyIndx].ID
            myEnemies[myTowEnemyIndx].E_TOW = myTargets[myTowTargetIndx].T_ID
        end
    end
end


masterCellID = 100000 --Constant used for the IDs of the world's boundaries

--=======================================
-- function:  DrawCell(x,y)
-- author:    Paul Schuytema
-- created:   February 7, 2005
-- returns:   nothing (process)
-- descrip:   draws a cell in an 40x30 world
--=======================================
function DrawCell(x,y)
--x is the X coordinate
--y is the Y coordinate
    CreateItem(masterCellID, "Sprite", "box1.bmp")
    SetItemPosition(masterCellID, (x-1) * 20, (y-1) * 20, 20, 20)
    masterCellID = masterCellID + 1
end


--=======================================
-- function:  MakeWorld()
-- author:    Paul Schuytema
-- created:   February 7, 2005
-- returns:   nothing (process)
-- descrip:   fills the world with cells according to the world table
--=======================================
function MakeWorld()
    for x = 1,40 do
        for y = 1,30 do
            if world[y][x] == 1 then
                DrawCell(x,y)
            end
        end
    end
end


--Creates the world table

world = {}

--Data filling the world table
--1 results in a cell created in the 20x20 space
--0 results in no cell created in the 20x20 space

world[1]=   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
world[2]=   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[3]=   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[4]=   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[5]=   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[6]=   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[7]=   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[8]=   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[9]=   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[10]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[11]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[12]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[13]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[14]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[15]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[16]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[17]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[18]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[19]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[20]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[21]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[22]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[23]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[24]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[25]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[26]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[27]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[28]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[29]=  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
world[30]=  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
