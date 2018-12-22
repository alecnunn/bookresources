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
-- function:  CommaFormatBigInteger(aNumber)
-- author:    Paul Schuytema
-- created:   February 7, 2005
-- returns:   newString: string that has numbers and commas
-- descrip:   takes a number and returns a formatted string
--=======================================
function CommaFormatBigInteger(aNumber)
	isNegative = 0
	if aNumber < 0 then
		isNegative = 1
		aNumber = -1*aNumber
	end
	baseString = string.format("%d", aNumber)
	baseLength = string.len(baseString)
	numBlocks = math.floor(baseLength/3)

	if numBlocks == 0 then
		if isNegative ~= 0 then
			newString = string.format("%d", -1*aNumber)
		else
			newString = baseString
		end
	else
		if isNegative ~= 0 then
			newString = "-"
		else
			newString = ""
		end
		numLeaders = baseLength - 3*numBlocks;
		if numLeaders > 0 then
			newString = newString .. string.sub (baseString, 1 , numLeaders) .. ","
		end
		for i = 1,numBlocks do
			newString = newString .. string.sub (baseString, numLeaders+1+(i-1)*3 , numLeaders+i*3) .. ","
		end
		newLength = string.len(newString)
		newString = string.sub(newString, 1, newLength-1); -- strip off final comma
	end
	return newString
end


--=======================================
-- function:  ASCIIToString(id)
-- author:    Nick Carlson
-- created:   March 1, 2005
-- returns:   myString: string representation of a keyboard ID
-- descrip:   turns ASCII reprentations into strings naming the key
--=======================================
function ASCIIToString(id)
    if id == 8 then
        myString = "Backspace"
    elseif id == 9 then
        myString = "Tab"
    elseif id == 13 then
        myString = "Enter"
    elseif id == 32 then --Space Bar
        myString = "Space Bar"
    else
        myString = string.char(id)
    end
    return myString
end

--=======================================
-- function:  HighlightText(id)
-- author:    Paul Schuytema
-- created:   March 2, 2005
-- returns:   nothing (process)
-- descrip:   in the Control interface, this will turn the indicated text field red
--=======================================
function HighlightText(id)
    --first, set everything to default color
    for indx = GUI_KEY_SELECT + 200, GUI_KEY_SELECT + 200 + 3 do
        ItemCommand(indx, "SetColor", 176,173,254,255)
    end
    --now set the indicated text field to red
    --do nothing is the id range doesn't match
    if (id > GUI_KEY_SELECT + 199) and (id < GUI_KEY_SELECT + 204) then
        ItemCommand(id, "SetColor", 255,0,0,255)
    end
end


--=======================================
-- function: ClearGUI(id)
-- author:   Paul Schuytema
-- created:  February 1, 2004
-- returns:  nothing (process)
-- descrip:  disables all GUI of a specific interface
--=======================================
function ClearGUI(id)
    for indx = 1,1000 do
        EnableObject(id + indx, 0, 0);
    end
end


--=======================================
-- function: RestoreGUI(id)
-- author:   Paul Schuytema
-- created:  February 1, 2004
-- returns:  nothing (process)
-- descrip:  enables all GUI of a specific interface
--=======================================
function RestoreGUI(id)
    for indx = 1,1000 do
        EnableObject(id + indx, 1, 1);
    end
end


--=======================================
-- function:  EnemyInit()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   sets up initial enemies
--=======================================
function EnemyInit()
    enemyCount = 5 --Number of enemies in the game
    myEnemies = {} --Creates myEnemies table

    --Creates table, one entry per potential enemy
    for indx = 1,enemyCount do
        --Creates a table to hold the data for each enemy
        myEnemies[indx] = {}
        --Now initialize the enemy
        EnemyRespawn(indx)
    end
end


--=======================================
-- function:  EnemyRespawn(indx)
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   fills in enemy data given an index to the myEnemies table
--=======================================
function EnemyRespawn(indx)
--Fills/refills myEnemies table according to indx
--indx is the myEnemies table index assigned to the enemy

    --Initial values
    myEnemies[indx].XTHRUST = 0 --Thrust along the x-axis (#)
    myEnemies[indx].YTHRUST = 0 --Thrust along the y-axis (#)
    myEnemies[indx].ROT = math.random(1,8) --Rotation of enemy ship (#)
    myEnemies[indx].ID = GUI_RUNTIME_SPRITES + indx + 100 --Starts GUI identification at 101 (#)
    myEnemies[indx].E_TOW = "no" --Towing flag (target ID # or "no")
    myEnemies[indx].FIRE = 0 --Projectile firing time interval (#)

    --Randomly selects side of screen to enter from
    entrySide = math.random(1,4)
    if entrySide == 1 then       --Left
        myEnemies[indx].EX = math.random(-40,-20) --X coordinate (#)
        myEnemies[indx].EY = math.random(-40,620) --Y coordinate (#)
    elseif entrySide == 2 then   --Right
        myEnemies[indx].EX = math.random(800,820) --X coordinate (#)
        myEnemies[indx].EY = math.random(-40,620) --Y coordinate (#)
    elseif entrySide == 3 then   --Top
        myEnemies[indx].EX = math.random(-40,820) --X coordinate (#)
        myEnemies[indx].EY = math.random(-40,-20) --Y coordinate (#)
    else                        --Bottom
        myEnemies[indx].EX = math.random(-40,820) --X coordinate (#)
        myEnemies[indx].EY = math.random(600,620) --Y coordinate (#)
    end

    --Determines enemy's thrust, reaction, and firing abilities based on time
    --Reaction time decreases as REACT decreases (must be at least 1)
    --Maximum thrust increases as MAX increases
    --enemyFireInterval (#) compared to FIRE to determine enemy shooting (see EnemyFacing(indx) function)
    if (timeCounter >= 0) and (timeCounter < 100) then
        myEnemies[indx].REACT = 5 --Reaction time interval (#)
        myEnemies[indx].MAX = 5 --Maximum thrust (#)
        enemyFireInterval = 9
    elseif (timeCounter >= 100) and (timeCounter < 200) then
        myEnemies[indx].REACT = 4 --Reaction time interval (#)
        myEnemies[indx].MAX = 6 --Maximum thrust (#)
        enemyFireInterval = 8
    elseif (timeCounter >= 200) and (timeCounter < 300) then
        myEnemies[indx].REACT = 3 --Reaction time interval (#)
        myEnemies[indx].MAX = 7 --Maximum thrust (#)
        enemyFireInterval = 7
    elseif (timeCounter >= 300) and (timeCounter < 400) then
        myEnemies[indx].REACT = 2 --Reaction time interval (#)
        myEnemies[indx].MAX = 8 --Maximum thrust (#)
        enemyFireInterval = 6
    elseif (timeCounter >= 400) then
        myEnemies[indx].REACT = 1 --Reaction time interval (#)
        myEnemies[indx].MAX = 9 --Maximum thrust (#)
        enemyFireInterval = 5
    end

    --Randomly selects the AI type and diplays the proper ship image
    myEnemies[indx].TYPE = math.random(1,4) --AI type (1,2,3, or 4)
    if myEnemies[indx].TYPE == 1 then
        CreateItem(myEnemies[indx].ID, "Sprite", "e1_ship1.bmp")
    elseif myEnemies[indx].TYPE == 2 then
        CreateItem(myEnemies[indx].ID, "Sprite", "e2_ship1.bmp")
    elseif myEnemies[indx].TYPE == 3 then
        myEnemies[indx].MAX = 10
        CreateItem(myEnemies[indx].ID, "Sprite", "e3_ship1.bmp")
    elseif myEnemies[indx].TYPE == 4 then
        CreateItem(myEnemies[indx].ID, "Sprite", "e4_ship1.bmp")
    end
    SetItemPosition(myEnemies[indx].ID, myEnemies[indx].EX, myEnemies[indx].EY, 20, 20)
end


--=======================================
-- function:  TargetInit()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   This initializes the target crates
--=======================================
function TargetInit()

    targetCount = 8 --Number of targets in the game
    myTargets = {} --Creates myTargets table

    startID = GUI_RUNTIME_SPRITES + 200 --Starts GUI identification at 200 (#)
    startX = 360
    startY = 260

    --Creates/fills table with initial values
    for indx = 1, targetCount + 1 do
        myTargets[indx] = {}
        myTargets[indx].T_ID = startID
        startID = startID + 1
        myTargets[indx].T_TOW = "no" --Towing flag (enemy ID #, "no", or "done")
        myTargets[indx].T_X = startX --X coordinate (#)
        startX = startX + 30
        if startX > 420 then
            startX = 360
        end
        myTargets[indx].T_Y = startY --Y coordinate (#)
        if indx > 2 then
            if indx < 6 then
                startY = 290
            else
                startY = 320
            end
        end
    end

    --now delete the middle target, so the player can spawn in
    table.remove(myTargets, 5)

    --Creates targets (as sprites) and places them
    for indx = 1,targetCount do
        CreateItem(myTargets[indx].T_ID, "Sprite", "box2.jpg")
        SetItemPosition(myTargets[indx].T_ID, myTargets[indx].T_X, myTargets[indx].T_Y, 20, 20)
    end
end


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
-- function:  DrawShip(myRot, x, y)
-- author:    Paul Schuytema
-- created:   February 1, 2004
-- returns:   nothing (process)
-- descrip:   refreshes the image of the player's ship
--=======================================
function DrawShip(myRot, x, y)
--myRot is the rotation of the player's ship
--x is the X coordinate of the player's ship
--y is the Y coordinate of the player's ship

    DeleteItem(GUI_RUNTIME_SPRITES + 100)
    shipName = string.format("%s%d%s", "ship", myRot, ".bmp")
    myX = myX + x
    myY = myY + y
    --Checks for collision and displays new image
    CollisionCheck("player", myX, myY)
    CreateItem(GUI_RUNTIME_SPRITES + 100, "Sprite", shipName);
    SetItemPosition(GUI_RUNTIME_SPRITES + 100, myX, myY, 20, 20)
    --Checks for player's contact with enemies or enemy projectiles
    if alive == "yes" then
        CasualtyCheck()
    end
end


--=======================================
-- function:  EnemyFacing(indx)
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   calculates the actions of a specific ship's AI
--=======================================
function EnemyFacing(indx)
--indx is the myEnemies table index assigned to the enemy

    if myEnemies[indx].ID ~= nil then
        if myEnemies[indx].TYPE == 1 then --Box grabbers
            for i = 1,targetCount do
                if myEnemies[indx].E_TOW == "no" then --Enemy not towing
                    if myTargets[i].T_TOW == "no" then --Target free
                        SetTravelDirection(indx, myTargets[i].T_X + 10, myTargets[i].T_Y + 10)
                    end
                end
            end
        elseif myEnemies[indx].TYPE == 2 then --Shooters
            SetTravelDirection(indx, myX + 10, myY + 10)
            --Enemy shooting script (see EnemyRespawn(indx) function)
            myEnemies[indx].FIRE = myEnemies[indx].FIRE + 1
            if myEnemies[indx].FIRE == enemyFireInterval then
                FireProjectile(indx, myEnemies[indx].XTHRUST, myEnemies[indx].YTHRUST)
                myEnemies[indx].FIRE = 0
            end
        elseif myEnemies[indx].TYPE == 3 then --Rammers
            SetTravelDirection(indx, myX + 10, myY + 10)
        elseif myEnemies[indx].TYPE == 4 then --Hybrids
            if myEnemies[indx].E_TOW == "no" then --Enemy not towing
                --Determine distance from player
                playerDistance = math.sqrt(((myEnemies[indx].EX + 10) - (myX + 10))^2 + ((myEnemies[indx].EY + 10) - (myY + 10))^2)
                targetDistance = 10000
                for i = 1,targetCount do
                    if myTargets[i].T_TOW == "no" then --Target free
                        --Determine distance from closest target
                        tempTargetDistance = math.sqrt(((myEnemies[indx].EX + 10) - (myTargets[i].T_X + 10))^2 + ((myEnemies[indx].EY + 10) - (myTargets[i].T_Y + 10))^2)
                        if tempTargetDistance < targetDistance then
                            targetDistance = tempTargetDistance
                            --Evaluate for the closest option
                            if playerDistance < targetDistance then
                                tX = myX + 10
                                tY = myY + 10
                                --Enemy shooting script
                                myEnemies[indx].FIRE = myEnemies[indx].FIRE + 1
                                if myEnemies[indx].FIRE == enemyFireInterval then
                                    FireProjectile(indx, myEnemies[indx].XTHRUST, myEnemies[indx].YTHRUST)
                                    myEnemies[indx].FIRE = 0
                                end
                            else
                                tX = myTargets[i].T_X + 10
                                tY = myTargets[i].T_Y + 10
                            end
                        end
                    else
                        tX = myX + 10
                        tY = myY + 10
                    end
                end
                SetTravelDirection(indx, tX, tY)
            end
        end
    end
end


--=======================================
-- function:  SetTravelDirection(indx, tX, tY)
-- author:    Paul Schuytema
-- created:   February 1, 2004
-- returns:   nothing (process)
-- descrip:   sets the enemy's rotation based upon the coordinates of its goal
--=======================================
function SetTravelDirection(indx, tX, tY)
--indx is the myEnemies table index assigned to the enemy
--tX is the X coordinate of the goal (target or player's ship)
--tY is the Y coordinate of the goal (target or player's ship)

    if tX < myEnemies[indx].EX then
        if tY < myEnemies[indx].EY then
            if (myEnemies[indx].EY - tY) > 150 then
                if (myEnemies[indx].EX - tX) > 150 then
                    myEnemies[indx].ROT = 8 --Up/left
                else
                    myEnemies[indx].ROT = 1 --Up
                end
            elseif (myEnemies[indx].EY - tY) > 30 then
                myEnemies[indx].ROT = 8 --Up/left
            else
                myEnemies[indx].ROT = 7 --Left
            end
        elseif tY == myEnemies[indx].EY then
            myEnemies[indx].ROT = 7 --Left
        elseif tY > myEnemies[indx].EY then
            if (tY - myEnemies[indx].EY) > 150 then
                if (myEnemies[indx].EX - tX) > 150 then
                    myEnemies[indx].ROT = 6 --Down/left
                else
                    myEnemies[indx].ROT = 5 --Down
                end
            elseif (tY - myEnemies[indx].EY) > 30 then
                myEnemies[indx].ROT = 6 --Down/left
            else
                myEnemies[indx].ROT = 7 --Left
            end
        end
    elseif tX == myEnemies[indx].EX then
        if tY <= myEnemies[indx].EY then
            myEnemies[indx].ROT = 1 --Up
        else
            myEnemies[indx].ROT = 5 --Down
        end
    elseif tX > myEnemies[indx].EX then
        if tY < myEnemies[indx].EY then
            if (myEnemies[indx].EY - tY) > 150 then
                if (tX - myEnemies[indx].EX) > 150 then
                    myEnemies[indx].ROT = 2 --Up/right
                else
                    myEnemies[indx].ROT = 1 --Up
                end
            elseif (myEnemies[indx].EY - tY) > 30 then
                myEnemies[indx].ROT = 2 --Up/right
            else
                myEnemies[indx].ROT = 3 --Right
            end
        elseif tY == myEnemies[indx].EY then
            myEnemies[indx].ROT = 3 --Right
        elseif tY > myEnemies[indx].EY then
            if (tY - myEnemies[indx].EY) > 150 then
                if (tX - myEnemies[indx].EX) > 150 then
                    myEnemies[indx].ROT = 4 --Down/right
                else
                    myEnemies[indx].ROT = 5 --Down
                end
            elseif (tY - myEnemies[indx].EY) > 30 then
                myEnemies[indx].ROT = 4 --Down/right
            else
                myEnemies[indx].ROT = 3 --Right
            end
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
-- function:  DrawTargets()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   refreshes the target images
--=======================================
function DrawTargets()

    for indx = 1,targetCount do
        if myTargets[indx].T_ID ~= nil then
            DeleteItem(myTargets[indx].T_ID)
            if (myTargets[indx].T_TOW ~= "no") then --Target being towed
                for i = 1, enemyCount do
                    if myTargets[indx].T_TOW == myEnemies[i].ID then --Towing pair
                        --Postition target behind enemy
                        if myEnemies[i].ROT == 1 then --Up
                            myTargets[indx].T_X = myEnemies[i].EX
                            myTargets[indx].T_Y = myEnemies[i].EY + 20
                        end
                        if myEnemies[i].ROT == 2 then --Up/right
                            myTargets[indx].T_X = myEnemies[i].EX - 20
                            myTargets[indx].T_Y = myEnemies[i].EY + 20
                        end
                        if myEnemies[i].ROT == 3 then --Right
                            myTargets[indx].T_X = myEnemies[i].EX - 20
                            myTargets[indx].T_Y = myEnemies[i].EY
                        end
                        if myEnemies[i].ROT == 4 then --Down/right
                            myTargets[indx].T_X = myEnemies[i].EX - 20
                            myTargets[indx].T_Y = myEnemies[i].EY - 20
                        end
                        if myEnemies[i].ROT == 5 then --Down
                            myTargets[indx].T_X = myEnemies[i].EX
                            myTargets[indx].T_Y = myEnemies[i].EY - 20
                        end
                        if myEnemies[i].ROT == 6 then --Down/left
                            myTargets[indx].T_X = myEnemies[i].EX + 20
                            myTargets[indx].T_Y = myEnemies[i].EY - 20
                        end
                        if myEnemies[i].ROT == 7 then --Left
                            myTargets[indx].T_X = myEnemies[i].EX + 20
                            myTargets[indx].T_Y = myEnemies[i].EY
                        end
                        if myEnemies[i].ROT == 8 then --Up/left
                            myTargets[indx].T_X = myEnemies[i].EX + 20
                            myTargets[indx].T_Y = myEnemies[i].EY + 20
                        end

                        CreateItem(myTargets[indx].T_ID, "Sprite", "box2.jpg")
                        SetItemPosition(myTargets[indx].T_ID, myTargets[indx].T_X, myTargets[indx].T_Y, 20, 20)

                        --Deal with stolen targets
                        if (myTargets[indx].T_X > 800) or (myTargets[indx].T_X < -20) or (myTargets[indx].T_Y > 600) or (myTargets[indx].T_Y < -20) then
                            DeleteItem(myTargets[indx].T_ID)
                            myTargets[indx].T_X = nil
                            myTargets[indx].T_Y = nil
                            myTargets[indx].T_ID = nil
                            myTargets[indx].T_TOW = "done"

                            myEnemies[i].E_TOW = "no"

                            --Update stolen target display and/or display game over string
                            targetDoneCounter = targetDoneCounter + 1
                            ItemCommand(GUI_INGAME + 205, "SetString", CommaFormatBigInteger(targetDoneCounter))
                            if targetDoneCounter == targetCount then
                                for indx = 1,pCount do
                                    if myProjectiles[indx].PROJ_ID ~= nil then
                                        DeleteItem(myProjectiles[indx].PROJ_ID)
                                    end
                                end
                                DeleteItem(GUI_RUNTIME_SPRITES + 100)
                                RunGUI("GUI_EndGame.lua")
                            end
                        end
                    end
                end
            else
            CreateItem(myTargets[indx].T_ID, "Sprite", "box2.jpg")
            SetItemPosition(myTargets[indx].T_ID, myTargets[indx].T_X, myTargets[indx].T_Y, 20, 20)
            end
        end
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
    playerProjectiles = 0 --Counter of projectiles fired by the player
    pIndx = 0 --Index of projectile in the myProjectiles table

    myProjectiles = {} --Creates myProjectiles table

    --Creates table, one entry per potential projectile
    for indx = 1,pCount do
        --Creates a table to hold the data for each projectile
        myProjectiles[indx] = {}
        myProjectiles[indx].PROJ_ID = 0--GUI identification number (#)
        myProjectiles[indx].PROJ_XTH = 0 --Thrust along the x-axis (#)
        myProjectiles[indx].PROJ_YTH = 0 --Thrust along the y-axis (#)
        myProjectiles[indx].PROJ_X = 0 --X coordinate (#)
        myProjectiles[indx].PROJ_Y = 0 --Y coordinate (#)
        myProjectiles[indx].PROJ_SHIP = 0 --Shooter ("player" or myEnemies index)
    end
end


--=======================================
-- function:  FireProjectile(ship, xThrust, yThrust)
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   creates a projectile and indexs it in myProjectiles table
--=======================================
function FireProjectile(ship, xThrust, yThrust)
--ship can either be "player" or the myEnemies index of the enemy
--xThrust is the projectile's thrust along the x-axis
--yThrust is the projectile's thrust along the y-axis

    pIndx = pIndx + 1
    myProjectiles[pIndx].PROJ_ID = GUI_RUNTIME_SPRITES + pIndx + 299 --Starts IDs at 300 (not including offset)
    if ship == "player" then
        --Player's projectile
        playerProjectiles = playerProjectiles + 1
        myProjectiles[pIndx].PROJ_SHIP = "player"
        myProjectiles[pIndx].PROJ_X = myX
        myProjectiles[pIndx].PROJ_Y = myY
        Rot = myRotation
        CreateItem(myProjectiles[pIndx].PROJ_ID, "Sprite", "box2.jpg")
    else
        --Enemy's projectile
        myProjectiles[pIndx].PROJ_SHIP = ship
        myProjectiles[pIndx].PROJ_X = myEnemies[ship].EX + myEnemies[ship].XTHRUST
        myProjectiles[pIndx].PROJ_Y = myEnemies[ship].EY + myEnemies[ship].YTHRUST
        Rot = myEnemies[ship].ROT
        CreateItem(myProjectiles[pIndx].PROJ_ID, "Sprite", "box3.jpg")
    end

    --Set the projectile's position and thrust
    if Rot == 1 then --Up
        myProjectiles[pIndx].PROJ_X = myProjectiles[pIndx].PROJ_X + 8
        myProjectiles[pIndx].PROJ_Y = myProjectiles[pIndx].PROJ_Y
        myProjectiles[pIndx].PROJ_XTH = xThrust
        myProjectiles[pIndx].PROJ_YTH = yThrust - 10
    end
    if Rot == 2 then --Up/right
        myProjectiles[pIndx].PROJ_X = myProjectiles[pIndx].PROJ_X + 18
        myProjectiles[pIndx].PROJ_Y = myProjectiles[pIndx].PROJ_Y
        myProjectiles[pIndx].PROJ_XTH = xThrust + 10
        myProjectiles[pIndx].PROJ_YTH = yThrust - 10
    end
    if Rot == 3 then --Right
        myProjectiles[pIndx].PROJ_X = myProjectiles[pIndx].PROJ_X + 18
        myProjectiles[pIndx].PROJ_Y = myProjectiles[pIndx].PROJ_Y + 8
        myProjectiles[pIndx].PROJ_XTH = xThrust + 10
        myProjectiles[pIndx].PROJ_YTH = yThrust
    end
    if Rot == 4 then --Down/right
        myProjectiles[pIndx].PROJ_X = myProjectiles[pIndx].PROJ_X + 18
        myProjectiles[pIndx].PROJ_Y = myProjectiles[pIndx].PROJ_Y + 18
        myProjectiles[pIndx].PROJ_XTH = xThrust + 10
        myProjectiles[pIndx].PROJ_YTH = yThrust + 10
    end
    if Rot == 5 then --Down
        myProjectiles[pIndx].PROJ_X = myProjectiles[pIndx].PROJ_X + 8
        myProjectiles[pIndx].PROJ_Y = myProjectiles[pIndx].PROJ_Y + 18
        myProjectiles[pIndx].PROJ_XTH = xThrust
        myProjectiles[pIndx].PROJ_YTH = yThrust + 10
    end
    if Rot == 6 then --Down/left
        myProjectiles[pIndx].PROJ_X = myProjectiles[pIndx].PROJ_X - 2
        myProjectiles[pIndx].PROJ_Y = myProjectiles[pIndx].PROJ_Y + 18
        myProjectiles[pIndx].PROJ_XTH = xThrust - 10
        myProjectiles[pIndx].PROJ_YTH = yThrust + 10
    end
    if Rot == 7 then --Left
        myProjectiles[pIndx].PROJ_X = myProjectiles[pIndx].PROJ_X - 2
        myProjectiles[pIndx].PROJ_Y = myProjectiles[pIndx].PROJ_Y + 8
        myProjectiles[pIndx].PROJ_XTH = xThrust - 10
        myProjectiles[pIndx].PROJ_YTH = yThrust
    end
    if Rot == 8 then --Up/left
        myProjectiles[pIndx].PROJ_X = myProjectiles[pIndx].PROJ_X - 2
        myProjectiles[pIndx].PROJ_Y = myProjectiles[pIndx].PROJ_Y
        myProjectiles[pIndx].PROJ_XTH = xThrust - 10
        myProjectiles[pIndx].PROJ_YTH = yThrust - 10
    end

    --Update the display
    SetItemPosition(myProjectiles[pIndx].PROJ_ID, myProjectiles[pIndx].PROJ_XTH, myProjectiles[pIndx].PROJ_YTH, 4, 4)
    if targetDoneCounter ~= targetCount then
        ItemCommand(GUI_INGAME + 204, "SetString", CommaFormatBigInteger(playerProjectiles))
    end
    if pIndx == pCount then
        pIndx = 0
    end
end


--=======================================
-- function:  DrawProjectile()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   refreshes the image of the projectiles
--=======================================
function DrawProjectile()

    for indx = 1, pCount do
        if myProjectiles[indx].PROJ_ID ~= nil then
            DeleteItem(myProjectiles[indx].PROJ_ID)
            if myProjectiles[indx].PROJ_SHIP == "player" then
                --Player's projectile
                CreateItem(myProjectiles[indx].PROJ_ID, "Sprite", "box2.jpg")
            else
                --Enemy's projectile
                CreateItem(myProjectiles[indx].PROJ_ID, "Sprite", "box3.jpg")
            end
            myProjectiles[indx].PROJ_X = myProjectiles[indx].PROJ_X + myProjectiles[indx].PROJ_XTH
            myProjectiles[indx].PROJ_Y = myProjectiles[indx].PROJ_Y + myProjectiles[indx].PROJ_YTH
            SetItemPosition(myProjectiles[indx].PROJ_ID, myProjectiles[indx].PROJ_X, myProjectiles[indx].PROJ_Y, 4, 4)

            --Deletes projectiles when they pass the world's boundaries
            if (myProjectiles[indx].PROJ_X > 780) or (myProjectiles[indx].PROJ_X < 20) or (myProjectiles[indx].PROJ_Y > 580) or (myProjectiles[indx].PROJ_Y < 20) then
                DeleteItem(myProjectiles[indx].PROJ_ID)
                myProjectiles[indx].PROJ_X = nil
                myProjectiles[indx].PROJ_Y = nil
                myProjectiles[indx].PROJ_XTH = nil
                myProjectiles[indx].PROJ_YTH = nil
                myProjectiles[indx].PROJ_ID = nil
                myProjectiles[indx].PROJ_SHIP = nil
            end
        end
    end
end


--=======================================
-- function:  EnemyHitCheck()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   checks for collisions between the player's projectiles and the enemies
--=======================================
function EnemyHitCheck()

    for i = 1,enemyCount do
        if myEnemies[i].ID ~= nil then
            j = 1
            while ((myEnemies[i].ID ~= nil) and (j < (pCount + 1))) do
                if myProjectiles[j].PROJ_ID ~= nil then
                    if myProjectiles[j].PROJ_SHIP == "player" then  --Projectile shot by player
                        if ((myEnemies[i].EX + 20) < myProjectiles[j].PROJ_X) or (myEnemies[i].EX > (myProjectiles[j].PROJ_X + 4)) or ((myEnemies[i].EY + 20) < myProjectiles[j].PROJ_Y) or (myEnemies[i].EY > (myProjectiles[j].PROJ_Y + 4)) then
                            --No collision
                        else
                            --Collision
                            if myEnemies[i].E_TOW ~= "no" then --Enemy was towing
                                for k = 1,targetCount do
                                    if myTargets[k].T_TOW == myEnemies[i].ID then
                                        --Makes the target available for towing again
                                        myTargets[k].T_TOW = "no"
                                    end
                                end
                            end

                            --Deletes the enemy and clears its table indexes
                            DeleteItem(myEnemies[i].ID)
                            myEnemies[i].XTHRUST = nil
                            myEnemies[i].YTHRUST = nil
                            myEnemies[i].ROT = nil
                            myEnemies[i].EX = nil
                            myEnemies[i].EY = nil
                            myEnemies[i].ID = nil
                            myEnemies[i].E_TOW = "no"
                            myEnemies[i].FIRE = nil

                            --Respawns the enemy if the game is not over
                            if targetDoneCounter ~= targetCount then
                                UpdateScore(myEnemies[i].TYPE)
                                EnemyRespawn(i)
                            end

                            --Deletes the projectile and clears its table indexes
                            DeleteItem(myProjectiles[j].PROJ_ID)
                            myProjectiles[j].PROJ_X = nil
                            myProjectiles[j].PROJ_Y = nil
                            myProjectiles[j].PROJ_XTH = nil
                            myProjectiles[j].PROJ_YTH = nil
                            myProjectiles[j].PROJ_ID = nil
                            myProjectiles[j].PROJ_SHIP = nil
                        end
                    end
                end
                j = j + 1
            end
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

    for i = 1,enemyCount do
        if myEnemies[i].ID ~= nil then
            j = 1
            while ((myEnemies[i].E_TOW == "no") and (j < (targetCount + 1))) do
                if (myTargets[j].T_TOW == "no") and ((myEnemies[i].TYPE == 1) or (myEnemies[i].TYPE == 4)) then --Enemies are box grabbers or hybrids & the target is free for towing
                    if ((myEnemies[i].EX + 20) < myTargets[j].T_X) or (myEnemies[i].EX > (myTargets[j].T_X + 20)) or ((myEnemies[i].EY + 20) < myTargets[j].T_Y) or (myEnemies[i].EY > (myTargets[j].T_Y + 20)) then
                        --No collision
                    else
                        --Collision
                        --Indexes the ID of each entity into the appropriate portion of the other's table
                        myTargets[j].T_TOW = myEnemies[i].ID
                        myEnemies[i].E_TOW = myTargets[j].T_ID
                    end
                end
                j = j + 1
            end
        end
    end
end


--=======================================
-- function:  CasualtyCheck()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   checks for collisions with the player
--=======================================
function CasualtyCheck()

    --Collisions with enemies
    i = 1
    while ((alive == "yes") and (i < (enemyCount + 1))) do
        if myEnemies[i].ID ~= nil then
            if ((myEnemies[i].EX + 20) < myX) or (myEnemies[i].EX > (myX + 20)) or ((myEnemies[i].EY + 20) < myY) or (myEnemies[i].EY > (myY + 20)) then
                --No collision
            else
                --Collision
                --Delete the player's ship
                DeleteItem(GUI_RUNTIME_SPRITES + 100)
                alive = "no"
                --Check for the enemy's target in tow (if any)
                if myEnemies[i].E_TOW ~= "no" then
                    for j = 1,targetCount do
                        if myEnemies[i].E_TOW == myTargets[j].T_ID then
                            --Makes the target available for towing again
                            myTargets[j].T_TOW = "no"
                        end
                    end
                    --Resets the enemy's towing status
                    myEnemies[i].E_TOW = "no"
                end
                --Deletes the enemy and clears its table indexes
                DeleteItem(myEnemies[i].ID)
                myEnemies[i].XTHRUST = nil
                myEnemies[i].YTHRUST = nil
                myEnemies[i].ROT = nil
                myEnemies[i].EX = nil
                myEnemies[i].EY = nil
                myEnemies[i].ID = nil
                myEnemies[i].FIRE = nil

                --Respawns the enemy if the game is not over
                if targetDoneCounter ~= targetCount then
                    UpdateScore(myEnemies[i].TYPE)
                    EnemyRespawn(i)
                end
            end
        end
        i = i + 1
    end

    --Collisions with projectiles
    i = 1
    while ((alive == "yes") and (i < (pCount + 1))) do
        if myProjectiles[i].PROJ_ID ~= nil then
            if myProjectiles[i].PROJ_SHIP ~= "player" then --Projectile shot by enemy
                if ((myX + 20) < myProjectiles[i].PROJ_X) or (myX > (myProjectiles[i].PROJ_X + 4)) or ((myY + 20) < myProjectiles[i].PROJ_Y) or (myY > (myProjectiles[i].PROJ_Y + 4)) then
                    --No collision
                else
                    --Collision
                    --Delete the player's ship
                    DeleteItem(GUI_RUNTIME_SPRITES + 100)
                    alive = "no"

                    --Deletes the projectile and clears its table indexes
                    DeleteItem(myProjectiles[i].PROJ_ID)
                    myProjectiles[i].PROJ_X = nil
                    myProjectiles[i].PROJ_Y = nil
                    myProjectiles[i].PROJ_XTH = nil
                    myProjectiles[i].PROJ_YTH = nil
                    myProjectiles[i].PROJ_ID = nil
                    myProjectiles[i].PROJ_SHIP = nil
                end
            end
        end
        i = i + 1
    end
end


--=======================================
-- function:  SaveGame()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   saves the current game
--=======================================
function SaveGame()

    gSavedGameDate = os.date("%m/%d/%Y %I:%M%p")

    --Creates the pathway for the file
    local fileName = "Take_Away_Saved_Game"
    --Writes the current game to the specified location
    myFile = io.open(string.format("%s%s%s", "SaveGames\\", fileName, ".lua"), "w")
    if myFile ~= nil then --File exists
        myFile:write("-- Take Away save game file");
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%s", "-- File created on: ", os.date()));
        myFile:write(string.char (10))
        myFile:write(string.char (10))

        myFile:write("--Initial player constants")
        myFile:write(string.char (10))
        myValue = myRotation
        myFile:write(string.format("%s%d", "myRotation = ",myValue))
        myFile:write(string.char (10))
        myValue = myX
        myFile:write(string.format("%s%d", "myX = ",myValue))
        myFile:write(string.char (10))
        myValue = myY
        myFile:write(string.format("%s%d", "myY = ",myValue))
        myFile:write(string.char (10))
        myValue = myXThrust
        myFile:write(string.format("%s%d", "myXThrust = ",myValue))
        myFile:write(string.char (10))
        myValue = myYThrust
        myFile:write(string.format("%s%d", "myYThrust = ",myValue))
        myFile:write(string.char (10))
        myValue = alive
        myFile:write(string.format("%s%s%s%s", "alive = ", string.char (34), myValue, string.char (34)))
        myFile:write(string.char (10))
        myFile:write(string.char (10))

        myFile:write("--Initial limits")
        myFile:write(string.char (10))
        myValue = respawnInterval
        myFile:write(string.format("%s%d", "respawnInterval = ",myValue))
        myFile:write(string.char (10))
        myFile:write(string.char (10))

        myFile:write("--Initial setting of counters")
        myFile:write(string.char (10))
        myValue = respawnCounter
        myFile:write(string.format("%s%d", "respawnCounter = ",myValue))
        myFile:write(string.char (10))
        myValue = score
        myFile:write(string.format("%s%d", "score = ",myValue))
        myFile:write(string.char (10))
        myValue = timeCounter
        myFile:write(string.format("%s%d", "timeCounter = ",myValue))
        myFile:write(string.char (10))
        myValue = targetDoneCounter
        myFile:write(string.format("%s%d", "targetDoneCounter = ",myValue))
        myFile:write(string.char (10))
        myFile:write(string.char (10))

        myFile:write("--Preferred game speed")
        myFile:write(string.char (10))
        myValue = refreshRate
        myFile:write(string.format("%s%.2f", "refreshRate = ",myValue))
        myFile:write(string.char (10))
        myFile:write(string.char (10))

        myFile:write("--Enemy information")
        myFile:write(string.char (10))
        myValue = enemyCount
        myFile:write(string.format("%s%d", "enemyCount = ",myValue))
        myFile:write(string.char (10))
        myValue = enemyFireInterval
        myFile:write(string.format("%s%d", "enemyFireInterval = ",myValue))
        myFile:write(string.char (10))
        for indx = 1,enemyCount do
            myValue = myEnemies[indx].ID
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myEnemies[", indx, "].ID = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myEnemies[", indx, "].ID = nil"))
            end
            myFile:write(string.char (10))
            myValue = myEnemies[indx].XTHRUST
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myEnemies[", indx, "].XTHRUST = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myEnemies[", indx, "].XTHRUST = nil"))
            end
            myFile:write(string.char (10))
            myValue = myEnemies[indx].YTHRUST
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myEnemies[", indx, "].YTHRUST = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myEnemies[", indx, "].YTHRUST = nil"))
            end
            myFile:write(string.char (10))
            myValue = myEnemies[indx].EX
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myEnemies[", indx, "].EX = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myEnemies[", indx, "].EX = nil"))
            end
            myFile:write(string.char (10))
            myValue = myEnemies[indx].EY
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myEnemies[", indx, "].EY = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myEnemies[", indx, "].EY = nil"))
            end
            myFile:write(string.char (10))
            myValue = myEnemies[indx].ROT
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myEnemies[", indx, "].ROT = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myEnemies[", indx, "].ROT = nil"))
            end
            myFile:write(string.char (10))
            myValue = myEnemies[indx].FIRE
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myEnemies[", indx, "].FIRE = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myEnemies[", indx, "].FIRE = nil"))
            end
            myFile:write(string.char (10))
            myValue = myEnemies[indx].E_TOW
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myEnemies[", indx, "].E_TOW = ", myValue))
            elseif type(myValue) == "string" then
                myFile:write(string.format("%s%d%s%s%s%s", "myEnemies[", indx, "].E_TOW = ", string.char (34), myValue, string.char (34)))
            end
            myFile:write(string.char (10))
            myValue = myEnemies[indx].MAX
            myFile:write(string.format("%s%d%s%d", "myEnemies[", indx, "].MAX = ", myValue))
            myFile:write(string.char (10))
            myValue = myEnemies[indx].REACT
            myFile:write(string.format("%s%d%s%d", "myEnemies[", indx, "].REACT = ", myValue))
            myFile:write(string.char (10))
            myValue = myEnemies[indx].TYPE
            myFile:write(string.format("%s%d%s%d", "myEnemies[", indx, "].TYPE = ", myValue))
            myFile:write(string.char (10))
        end
        myFile:write(string.char (10))
        myFile:write(string.char (10))

        myFile:write("--Target information")
        myFile:write(string.char (10))
        myValue = targetCount
        myFile:write(string.format("%s%d", "targetCount = ",myValue))
        myFile:write(string.char (10))
        for indx = 1,targetCount do
            myValue = myTargets[indx].T_ID
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myTargets[", indx, "].T_ID = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myTargets[", indx, "].T_ID = nil"))
            end
            myFile:write(string.char (10))
            myValue = myTargets[indx].T_X
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myTargets[", indx, "].T_X = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myTargets[", indx, "].T_X = nil"))
            end
            myFile:write(string.char (10))
            myValue = myTargets[indx].T_Y
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myTargets[", indx, "].T_Y = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myTargets[", indx, "].T_Y = nil"))
            end
            myFile:write(string.char (10))
            myValue = myTargets[indx].T_TOW
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myTargets[", indx, "].T_TOW = ", myValue))
            elseif type(myValue) == "string" then
                myFile:write(string.format("%s%d%s%s%s%s", "myTargets[", indx, "].T_TOW = ", string.char (34), myValue, string.char (34)))
            end
            myFile:write(string.char (10))
        end
        myFile:write(string.char (10))
        myFile:write(string.char (10))

        myFile:write("--Projectile information")
        myFile:write(string.char (10))
        myValue = pCount
        myFile:write(string.format("%s%d", "pCount = ",myValue))
        myFile:write(string.char (10))
        myValue = pIndx
        myFile:write(string.format("%s%d", "pIndx = ",myValue))
        myFile:write(string.char (10))
        myValue = playerProjectiles
        myFile:write(string.format("%s%d", "playerProjectiles = ",myValue))
        myFile:write(string.char (10))
        for indx = 1,pCount do
            myValue = myProjectiles[indx].PROJ_ID
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myProjectiles[", indx, "].PROJ_ID = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myProjectiles[", indx, "].PROJ_ID = nil"))
            end
            myFile:write(string.char (10))
            myValue = myProjectiles[indx].PROJ_XTH
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myProjectiles[", indx, "].PROJ_XTH = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myProjectiles[", indx, "].PROJ_XTH = nil"))
            end
            myFile:write(string.char (10))
            myValue = myProjectiles[indx].PROJ_YTH
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myProjectiles[", indx, "].PROJ_YTH = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myProjectiles[", indx, "].PROJ_YTH = nil"))
            end
            myFile:write(string.char (10))
            myValue = myProjectiles[indx].PROJ_X
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myProjectiles[", indx, "].PROJ_X = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myProjectiles[", indx, "].PROJ_X = nil"))
            end
            myFile:write(string.char (10))
            myValue = myProjectiles[indx].PROJ_Y
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myProjectiles[", indx, "].PROJ_Y = ", myValue))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myProjectiles[", indx, "].PROJ_Y = nil"))
            end
            myFile:write(string.char (10))
            myValue = myProjectiles[indx].PROJ_SHIP
            if type(myValue) == "number" then
                myFile:write(string.format("%s%d%s%d", "myProjectiles[", indx, "].PROJ_SHIP = ", myValue))
            elseif type(myValue) == "string" then
                myFile:write(string.format("%s%d%s%s%s%s", "myProjectiles[", indx, "].PROJ_SHIP = ", string.char (34), myValue, string.char (34)))
            elseif type(myValue) == "nil" then
                myFile:write(string.format("%s%d%s", "myProjectiles[", indx, "].PROJ_SHIP = nil"))
            end
            myFile:write(string.char (10))
        end
        myFile:write(string.char (10))
        myFile:write(string.char (10))

        myFile:write("--Initial GUI setup")
        myFile:write(string.char (10))
        myFile:write(string.format("%s%s%s%s%s%s%s", "ItemCommand(GUI_INGAME + 204, ", string.char (34), "SetString", string.char (34), ",", CommaFormatBigInteger(playerProjectiles), ")"))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%s%s%s%s%s%s", "ItemCommand(GUI_INGAME + 205, ", string.char (34), "SetString", string.char (34), ",", CommaFormatBigInteger(targetDoneCounter), ")"))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%s%s%s%s%s%s", "ItemCommand(GUI_INGAME + 206, ", string.char (34), "SetString", string.char (34), ",", CommaFormatBigInteger(score), ")"))
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        io.close(myFile)
    end
end


--=======================================
-- function:  LoadGame()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   loads a saved game
--=======================================
function LoadGame()

    --Creates the pathway for the file
    local fileName = io.open(("SaveGames\\Take_Away_Saved_Game.lua"), "r")
    --Checks for the file's existence
    if fileName ~= nil then
        --Clears all projectiles
        for indx = 1,pCount do
            if myProjectiles[indx].PROJ_ID ~= nil then
                DeleteItem(myProjectiles[indx].PROJ_ID)
            end
        end
        --Clears all targets
        for indx = 1,targetCount do
            if myTargets[indx].T_ID ~= nil then
                DeleteItem(myTargets[indx].T_ID)
            end
        end
        DeleteItem(GUI_RUNTIME_SPRITES + 100)
        --Reads the file, closes it, and begins the action
        dofile("SaveGames\\Take_Away_Saved_Game.lua")
        fileName:close()
        StartTimer(refreshRate)
    end
end


--=======================================
-- function:  SaveSettings()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   saves the current setting and high scores
--=======================================
function SaveSettings()

    --Creates the pathway for the file
    local fileName = "Take_Away_Saved_Settings"
    --Writes the current game to the specified location
    myFile = io.open(string.format("%s%s%s", "SaveGames\\", fileName, ".lua"), "w")
    if myFile ~= nil then --File exists
        myFile:write("-- Take Away save settings file");
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%s", "-- File created on: ", os.date()));
        myFile:write(string.char (10))
        myFile:write(string.char (10))

        myFile:write("--Key select settings")
        myFile:write(string.char (10))
        myValue = gThrustKey
        myFile:write(string.format("%s%d", "gThrustKey = ",myValue))
        myFile:write(string.char (10))
        myValue = gShootKey
        myFile:write(string.format("%s%d", "gShootKey = ",myValue))
        myFile:write(string.char (10))
        myValue = gTurnLeftKey
        myFile:write(string.format("%s%d", "gTurnLeftKey = ",myValue))
        myFile:write(string.char (10))
        myValue = gTurnRightKey
        myFile:write(string.format("%s%d", "gTurnRightKey = ",myValue))
        myFile:write(string.char (10))
        myFile:write(string.char (10))

        myFile:write("--High scores")
        myFile:write(string.char (10))
        myValue = gHighScoreNum
        myFile:write(string.format("%s%d", "gHighScoreNum = ", myValue));
        myFile:write(string.char (10))
        myFile:write(string.format("%s", "myHighScoresDate = {}"));
        myFile:write(string.char (10))
        myFile:write(string.format("%s", "myHighScoresAmount = {}"));
        myFile:write(string.char (10))
        if gHighScoreNum > 0 then
            for indx = 1, gHighScoreNum do
                myValue = myHighScoresDate[indx]
                myFile:write(string.format("%s%d%s%s%s%s", "myHighScoresDate[", indx, "] = ", string.char (34), myValue, string.char (34)));
                myFile:write(string.char (10))
                myValue = myHighScoresAmount[indx]
                myFile:write(string.format("%s%d%s%d", "myHighScoresAmount[", indx, "] = ", myValue));
                myFile:write(string.char (10))
            end
        end
        myFile:write(string.char (10))

        myFile:write("--Saved game date")
        myFile:write(string.char (10))
        myValue = gSavedGameDate
        myFile:write(string.format("%s%s%s%s", "gSavedGameDate = ", string.char (34), myValue, string.char (34)))
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        io.close(myFile)
    end
end


--=======================================
-- function:  LoadSettings()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   loads the current setting and high scores
--=======================================
function LoadSettings()

    --Creates the pathway for the file
    local fileName = io.open(("SaveGames\\Take_Away_Saved_Settings.lua"), "r")
    --Checks for the file's existence
    if fileName ~= nil then
        --Reads the file, closes it, and begins the action
        dofile("SaveGames\\Take_Away_Saved_Settings.lua")
        fileName:close()
    else
        gSavedGameDate = ""
        gThrustKey = 32
        gShootKey = 112
        gTurnLeftKey = 91
        gTurnRightKey = 93
        gHighScoreNum = 0
        myHighScoresDate = {}
        myHighScoresAmount = {}
        EnableObject(GUI_MAIN_MENU + 301, 0, 0)
    end
end


--=======================================
-- function:  UpdateMainMenu()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   updates the Date and Score portions of the Main Menu
--=======================================
function UpdateMainMenu()

    for indx = 1,10 do
        if myHighScoresDate[indx] == nil then
            ItemCommand(GUI_MAIN_MENU + 200 + indx, "SetString", "")
            ItemCommand(GUI_MAIN_MENU + 210 + indx, "SetString", "")
        else
            ItemCommand(GUI_MAIN_MENU + 200 + indx, "SetString", myHighScoresDate[indx])
            ItemCommand(GUI_MAIN_MENU + 210 + indx, "SetString", CommaFormatBigInteger(myHighScoresAmount[indx]))
        end

        ItemCommand(GUI_MAIN_MENU + 200, "SetString", gSavedGameDate)
        if gSavedGameDate == "" then
            EnableObject(GUI_MAIN_MENU + 301, 0, 0)
        else
            EnableObject(GUI_MAIN_MENU + 301, 1, 1)
        end
    end
end


--=======================================
-- function:  SortScoreLists()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   sorts the myHighScoresAmount table into descending order
--=======================================
function SortScoreLists()
    if gHighScoreNum > 0 then
        for indx = 1, gHighScoreNum do
            for i = gHighScoreNum, indx+1, -1 do
                if myHighScoresAmount[i] > myHighScoresAmount[i-1] then
                    --swapy numbers
                    t =  myHighScoresAmount[i]
                    myHighScoresAmount[i] = myHighScoresAmount[i-1]
                    myHighScoresAmount[i-1] = t
                    --swap dates
                    t =  myHighScoresDate[i]
                    myHighScoresDate[i] = myHighScoresDate[i-1]
                    myHighScoresDate[i-1] = t
                end
            end
        end
    end
end


--=======================================
-- function:  SendHighScore(score)
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   "yes" or "no"
-- descrip:   checks to see if the score deserves top ten status & adds to tables (sorted later)
--=======================================
function SendHighScore(score)

    if gHighScoreNum < 10 then
        gHighScoreNum = gHighScoreNum + 1
        myHighScoresDate[gHighScoreNum] = os.date("%m/%d/%Y %I:%M%p")
        myHighScoresAmount[gHighScoreNum] = score
        return "yes"
    else
        if score >= myHighScoresAmount[gHighScoreNum] then
            myHighScoresDate[gHighScoreNum] = os.date("%m/%d/%Y %I:%M%p")
            myHighScoresAmount[gHighScoreNum] = score
            return "yes"
        else
            return "no"
        end
    end
end
