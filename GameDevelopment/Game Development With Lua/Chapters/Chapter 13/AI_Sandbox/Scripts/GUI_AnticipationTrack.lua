--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_AnticipationTrack.lua
-- author:    Nick Carlson
-- created:   March 7, 2005
-- descrip:   AI anticipates a player's position
--=======================================

-- first steps


--dofile calls for sub-GUIs


--100s Sprites


--200s Text items
--Script name
CreateItem(GUI_INGAME + 201,"TextField")
SetItemPosition(GUI_INGAME + 201, 26, 26, 0, 0)
SetFont(GUI_INGAME + 201, "Arial", 18)
ItemCommand(GUI_INGAME + 201, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 201, "SetString", gScenarioScript)

--Anticipation box
CreateItem(GUI_RUNTIME_SPRITES + 150, "Sprite", "box2.jpg")

--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--functions related to this GUI (not used anywhere else)
--=======================================
-- function:  AnticipationTrackSetUp()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   sets constants for the game
--=======================================
function AnticipationTrackSetUp()
    --Initial player constants
    myRotation = 1 --Player's rotation (#)
    myX = 390      --Player's x coordinate (#)
    myY = 290      --Player's y coordinate (#)
    myXThrust = 0  --Player's thrust along the x-axis (#)
    myYThrust = 0  --Player's thrust along the y-axis (#)
    alive = "yes"  --Player's life status ("yes" or "no")

    --Initial limits
    respawnInterval = 15  --Number respawnCounter must reach to respawn player (#)

    --Initial setting of counters
    respawnCounter = 0    --Player's death period (#)
    timeCounter = 0       --Passage of time (#)

    --Preferred game speed
    refreshRate = .1 --Seconds between timer expirations (#)
end


--=======================================
-- function:  AnticipationTrackEnemyInit()
-- author:    Nick Carlson
-- created:   March 3, 2005
-- returns:   nothing (process)
-- descrip:   sets up initial enemies
--=======================================
function AnticipationTrackEnemyInit()

    enemyCount = 1 --Number of enemies in the game
    myEnemies = {} --Creates myEnemies table

    --Creates table, one entry per potential enemy
    for indx = 1,enemyCount do
        --Creates a table to hold the data for each enemy
        myEnemies[indx] = {}
        --Initial values
        myEnemies[indx].EX = math.random(20,760)
        myEnemies[indx].EY = math.random(20,560)
        myEnemies[indx].XTHRUST = 0 --Thrust along the x-axis (#)
        myEnemies[indx].YTHRUST = 0 --Thrust along the y-axis (#)
        myEnemies[indx].ROT = math.random(1,8) --Rotation of enemy ship (#)
        myEnemies[indx].ID = GUI_RUNTIME_SPRITES + indx + 100 --Starts GUI identification at 101 (#)
        myEnemies[indx].REACT = 1 --Reaction time interval (#)
        myEnemies[indx].MAX = 7 --Maximum thrust (#)
        myEnemies[indx].TYPE = 1
        enemyFireInterval = 9
        CreateItem(myEnemies[indx].ID, "Sprite", "e1_ship1.bmp")
        SetItemPosition(myEnemies[indx].ID, myEnemies[indx].EX, myEnemies[indx].EY, 20, 20)
    end
end


--=======================================
-- function:  AnticipationTrackSetTravelDirection(indx, tX, tY)
-- author:    Paul Schuytema
-- created:   February 1, 2004
-- returns:   nothing (process)
-- descrip:   sets the enemy's rotation based upon the coordinates of its goal
--=======================================
function AnticipationTrackSetTravelDirection(indx, myX, myY, myXThrust, myYThrust)
--indx is the myEnemies table index assigned to the enemy
--tX is the X coordinate of the goal (target or player's ship)
--tY is the Y coordinate of the goal (target or player's ship)
    tX = myX + 10
    tY = myY + 10

    local enemyDistance = math.sqrt(((myX + 10) - (myEnemies[indx].EX + 10))^2 + ((myY + 10) - (myEnemies[indx].EY + 10))^2)
    local enemyRate = math.sqrt(((myEnemies[indx].XTHRUST)^2) + ((myEnemies[indx].YTHRUST)^2))
    local time = enemyDistance / enemyRate

    local playerRate = math.sqrt(((myXThrust)^2) + ((myYThrust)^2))
    local playerDistance = playerRate * time

    if playerDistance > 25 then
        playerDistance = 25
    end

    local playerXVector = math.sqrt(((playerDistance)^2) - ((myYThrust)^2))
    local playerYVector = math.sqrt(((playerDistance)^2) - ((myXThrust)^2))

    local dir = GetTravelDirection(myXThrust, myYThrust)

    if dir == 1 then --Up
        tY = tY - (playerDistance)
    elseif dir == 2 then --Up/right
        tX = tX + (playerXVector)
        tY = tY - (playerYVector)
    elseif dir == 3 then --Right
        tX = tX + (playerDistance)
    elseif dir == 4 then --Down/right
        tX = tX + (playerXVector)
        tY = tY + (playerYVector)
    elseif dir == 5 then --Down
        tY = tY + (playerDistance)
    elseif dir == 6 then --Down/left
        tX = tX - (playerXVector)
        tY = tY + (playerYVector)
    elseif dir == 7 then --Left
        tX = tX - (playerDistance)
    elseif dir == 8 then --Up/left
        tX = tX - (playerXVector)
        tY = tY - (playerYVector)
    end

    SetItemPosition(GUI_RUNTIME_SPRITES + 150, tX, tY, 4, 4)

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
-- function:  AnticipationTrackCasualtyCheck()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   checks for collisions with the player
--=======================================
function AnticipationTrackCasualtyCheck()

    --Collisions with enemies
    for i = 1,enemyCount do
        if myEnemies[i].ID ~= nil then
            if ((myEnemies[i].EX + 20) < myX) or (myEnemies[i].EX > (myX + 20)) or ((myEnemies[i].EY + 20) < myY) or (myEnemies[i].EY > (myY + 20)) then
                --No collision
            else
                --Collision
                --Delete the player's ship
                DeleteItem(GUI_RUNTIME_SPRITES + 100)
                alive = "no"

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
                AnticipationTrackEnemyInit()
            end
        end
    end
end


--=======================================
-- function:  AnticipationTrackDrawShip(myRot, x, y)
-- author:    Paul Schuytema
-- created:   February 1, 2004
-- returns:   nothing (process)
-- descrip:   refreshes the image of the player's ship
--=======================================
function AnticipationTrackDrawShip(myRot, x, y)
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
        AnticipationTrackCasualtyCheck()
    end
end


--Initial run sets the event handler
SetEventHandler("AnticipationTrackEvent")


--Event handler
function AnticipationTrackEvent(id, eventCode)

	if (eventCode == GUI_REENTER_INTERFACE)  or (eventCode == GUI_ENTER_INTERFACE) then
        MakeWorld()
        AnticipationTrackSetUp()
        AnticipationTrackEnemyInit()
        StartTimer(refreshRate)
    end

    if eventCode == GUI_KEY_PRESS then
    	if id == 27 then --Esc
            RunGUI("GUI_MainMenu.lua")
    	end

        if id == 91 then -- [
            --Turns player's ship counterclockwise
            myRotation = myRotation - 1
            if myRotation < 1 then
                myRotation = 8
            end
        end

        if id == 93 then -- ]
            --Turns player's ship clockwise
            myRotation = myRotation + 1
            if myRotation > 8 then
                myRotation = 1
            end
        end

        if id == 32 then --Space bar
            --Increases the player's thrust
            if myRotation == 1 then
                myYThrust = myYThrust - 1
            end
            if myRotation == 2 then
                myYThrust = myYThrust - 1
                myXThrust = myXThrust + 1
            end
            if myRotation == 3 then
                myXThrust = myXThrust + 1
            end
            if myRotation == 4 then
                myYThrust = myYThrust + 1
                myXThrust = myXThrust + 1
            end
            if myRotation == 5 then
                myYThrust = myYThrust + 1
            end
            if myRotation == 6 then
                myYThrust = myYThrust + 1
                myXThrust = myXThrust - 1
            end
            if myRotation == 7 then
                myXThrust = myXThrust - 1
            end
            if myRotation == 8 then
                myYThrust = myYThrust - 1
                myXThrust = myXThrust - 1
            end
            if myYThrust > 5 then
                myYThrust = 5
            end
            if myYThrust < -5 then
                myYThrust = -5
            end
            if myXThrust > 5 then
                myXThrust = 5
            end
            if myXThrust < -5 then
                myXThrust = -5
            end
        end
    end


    if eventCode == GUI_TIMER_EXPIRED then
        if alive == "yes" then
            --Refreshes the player's ship image
            AnticipationTrackDrawShip(myRotation, myXThrust, myYThrust)
        else
            --Respawns the player after a certain period
            respawnCounter = respawnCounter + 1
            if respawnCounter == respawnInterval then
                myRotation = 1
                myXThrust = 0
                myYThrust = 0
                myX = 390
                myY = 290
                respawnCounter = 0
                alive = "yes"
            end
        end

        --Directs each enemy based on its own REACT setting
        timeCounter = timeCounter + 1
        for indx = 1,enemyCount do
            if (timeCounter/myEnemies[indx].REACT) == (math.floor(timeCounter/myEnemies[indx].REACT)) then
                AnticipationTrackSetTravelDirection(indx, myX, myY, myXThrust, myYThrust)
            end
        end

        --Updates entities
        DrawEnemyShip()
        --Checks for various collisions
        --Resets the timer
        StartTimer(refreshRate)
    end
end
