--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_ProximityTrack.lua
-- author:    Nick Carlson
-- created:   March 3, 2005
-- descrip:   AI pursues targets based on relative distance
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

--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--functions related to this GUI (not used anywhere else)
--=======================================
-- function:  ProximityTrackSetUp()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   sets constants for the game
--=======================================
function ProximityTrackSetUp()
    desiredTarget = "none" --Sought target (string)
    timeCounter = 0 --Passage of time (#)
    refreshRate = .1 --Seconds between timer expirations (#)
end


--=======================================
-- function:  GUI_ProximityTrackTargetInit()
-- author:    Nick Carlson
-- created:   March 3, 2005
-- returns:   nothing (process)
-- descrip:   This initializes the target crates
--=======================================
function ProximityTrackTargetInit()

    targetCount = 2 --Number of targets in the game
    myTargets = {} --Creates myTargets table

    --Creates/fills table with initial values
    for indx = 1, targetCount do
        myTargets[indx] = {}
        myTargets[indx].T_ID = GUI_RUNTIME_SPRITES + indx + 199 --Starts GUI identification at 200 (#)
        myTargets[indx].T_TOW = "no" --Towing flag (enemy ID #, "no", or "done")
        myTargets[indx].T_X = math.random(20,760) --X coordinate (#)
        myTargets[indx].T_Y = math.random(20,560) --Y coordinate (#)
        CreateItem(myTargets[indx].T_ID, "Sprite", "box2.jpg")
        SetItemPosition(myTargets[indx].T_ID, myTargets[indx].T_X, myTargets[indx].T_Y, 20, 20)
    end
end


--=======================================
-- function:  ProximityTrackPutTarget()
-- author:    Paul Schuytema
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   randomly places the target
--=======================================
function ProximityTrackPutTarget()

    for indx = 1, targetCount do
        myTargets[indx].T_X = math.random(20,760)
        myTargets[indx].T_Y = math.random(20,560)
        CreateItem(myTargets[indx].T_ID, "Sprite", "box2.jpg")
        SetItemPosition(myTargets[indx].T_ID, myTargets[indx].T_X, myTargets[indx].T_Y, 20, 20)
    end
end


--=======================================
-- function:  ProximityTrackDrawTargets()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   refreshes the target images
--=======================================
function ProximityTrackDrawTargets()

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
-- function:  ProximityTrackEnemyInit()
-- author:    Nick Carlson
-- created:   March 3, 2005
-- returns:   nothing (process)
-- descrip:   sets up initial enemies
--=======================================
function ProximityTrackEnemyInit()

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
        myEnemies[indx].E_TOW = "no" --Towing flag (target ID # or "no")
        myEnemies[indx].FIRE = 0 --Projectile firing time interval (#)
        myEnemies[indx].REACT = 1 --Reaction time interval (#)
        myEnemies[indx].MAX = 7 --Maximum thrust (#)
        myEnemies[indx].TYPE = 1
        enemyFireInterval = 9
        CreateItem(myEnemies[indx].ID, "Sprite", "e1_ship1.bmp")
        SetItemPosition(myEnemies[indx].ID, myEnemies[indx].EX, myEnemies[indx].EY, 20, 20)
    end
end


--=======================================
-- function:  ProximityTrackEnemyFacing(indx)
-- author:    Nick Carlson
-- created:   March 3, 2005
-- returns:   nothing (process)
-- descrip:   calculates the actions of a specific ship's AI
--=======================================
function ProximityTrackEnemyFacing(indx)
--indx is the myEnemies table index assigned to the enemy

    if (myEnemies[indx].E_TOW == "no") and (desiredTarget ~= "none") then --Enemy not towing and seeking target
        if desiredTarget ~= "set" then --Initial run
            --Determine distance from targets
            targetDistance = {}
            for i = 1,targetCount do
                if myTargets[i].T_TOW == "no" then --Target free
                    targetDistance[i] = math.sqrt(((myEnemies[indx].EX + 10) - (myTargets[i].T_X + 10))^2 + ((myEnemies[indx].EY + 10) - (myTargets[i].T_Y + 10))^2)
                end
            end

            if (targetDistance[1] ~= nil) and (targetDistance[2] ~= nil) then
            --Evaluate for the distance options
                if targetDistance[1] < targetDistance[2] then
                    nearIndx = 1
                    farIndx = 2
                else
                    nearIndx = 2
                    farIndx = 1
                end

                if desiredTarget == "near" then
                    tX = myTargets[nearIndx].T_X + 10
                    tY = myTargets[nearIndx].T_Y + 10
                    desiredTarget = "set"
                elseif desiredTarget == "far" then
                    tX = myTargets[farIndx].T_X + 10
                    tY = myTargets[farIndx].T_Y + 10
                    desiredTarget = "set"
                end
                ProximityTrackSetTravelDirection(indx, tX, tY)
            end
        else
            ProximityTrackSetTravelDirection(indx, tX, tY)
        end
    end
end


--=======================================
-- function:  ProximityTrackSetTravelDirection(indx, tX, tY)
-- author:    Paul Schuytema
-- created:   February 1, 2004
-- returns:   nothing (process)
-- descrip:   sets the enemy's rotation based upon the coordinates of its goal
--=======================================
function ProximityTrackSetTravelDirection(indx, tX, tY)
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


--Initial run sets the event handler
SetEventHandler("ProximityTrackEvent")


--Event handler
function ProximityTrackEvent(id, eventCode)

	if (eventCode == GUI_REENTER_INTERFACE)  or (eventCode == GUI_ENTER_INTERFACE) then
        MakeWorld()
        ProximityTrackSetUp()
        ProximityTrackEnemyInit()
        ProximityTrackTargetInit()
        StartTimer(refreshRate)
    end

    if eventCode == GUI_KEY_PRESS then
    	if id == 27 then --Esc
            RunGUI("GUI_MainMenu.lua")
    	end

        if id == 116 then --t
            ProximityTrackPutTarget()
        end

        if id == 110 then --n
            RunGUI("GUI_ProximityTrack.lua")
        end

        if id == 49 then --1
            desiredTarget = "near"
        end

        if id == 50 then --2
            desiredTarget = "far"
        end
    end


    if eventCode == GUI_TIMER_EXPIRED then
        --Directs each enemy based on its own REACT setting
        timeCounter = timeCounter + 1
        for indx = 1,enemyCount do
            if (timeCounter/myEnemies[indx].REACT) == (math.floor(timeCounter/myEnemies[indx].REACT)) then
                ProximityTrackEnemyFacing(indx)
            end
        end

        --Updates entities
        DrawEnemyShip()
        ProximityTrackDrawTargets()
        --Checks for various collisions
        EnemyTowCheck()
        --Resets the timer
        StartTimer(refreshRate)
    end
end
