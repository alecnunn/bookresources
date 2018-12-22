--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_StationaryTrack.lua
-- author:    Nick Carlson
-- created:   March 2, 2005
-- descrip:   AI pursues a stationary target
--=======================================

-- first steps


--dofile calls for sub-GUIs


--100s Sprites


--200s Text items
--Projectiles fired text
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
-- function:  StationaryTrackSetUp()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   sets constants for the game
--=======================================
function StationaryTrackSetUp()

    timeCounter = 0 --Passage of time (#)
    refreshRate = .1 --Seconds between timer expirations (#)
end


--=======================================
-- function:  StationaryTrackTargetInit()
-- author:    Nick Carlson
-- created:   March 3, 2005
-- returns:   nothing (process)
-- descrip:   This initializes the target crates
--=======================================
function StationaryTrackTargetInit()

    targetCount = 1 --Number of targets in the game
    myTargets = {} --Creates myTargets table

    --Creates/fills table with initial values
    for indx = 1, targetCount do
        myTargets[indx] = {}
        myTargets[indx].T_ID = GUI_RUNTIME_SPRITES + 200 --Starts GUI identification at 200 (#)
        myTargets[indx].T_TOW = "no" --Towing flag (enemy ID #, "no", or "done")
        myTargets[indx].T_X = math.random(20,760) --X coordinate (#)
        myTargets[indx].T_Y = math.random(20,560) --Y coordinate (#)
        CreateItem(myTargets[indx].T_ID, "Sprite", "box2.jpg")
        SetItemPosition(myTargets[indx].T_ID, myTargets[indx].T_X, myTargets[indx].T_Y, 20, 20)
    end
end


--=======================================
-- function:  StationaryTrackPutTarget()
-- author:    Paul Schuytema
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   randomly places the target
--=======================================
function StationaryTrackPutTarget()

    for indx = 1, targetCount do
        myTargets[indx].T_X = math.random(20,760)
        myTargets[indx].T_Y = math.random(20,560)
        SetItemPosition(myTargets[indx].T_ID, myTargets[indx].T_X, myTargets[indx].T_Y, 20, 20)
    end
end


--=======================================
-- function:  StationaryTrackDrawTargets()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   refreshes the target images
--=======================================
function StationaryTrackDrawTargets()

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
-- function:  StationaryTrackEnemyInit()
-- author:    Nick Carlson
-- created:   March 3, 2005
-- returns:   nothing (process)
-- descrip:   sets up initial enemies
--=======================================
function StationaryTrackEnemyInit()

    enemyCount = 1 --Number of enemies in the game
    myEnemies = {} --Creates myEnemies table

    --Creates table, one entry per potential enemy
    for indx = 1,enemyCount do
        --Creates a table to hold the data for each enemy
        myEnemies[indx] = {}
        --Initial values
        myEnemies[indx].EX = 390
        myEnemies[indx].EY = 290
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
-- function:  StationaryTrackEnemyFacing(indx)
-- author:    Nick Carlson
-- created:   March 3, 2005
-- returns:   nothing (process)
-- descrip:   calculates the actions of a specific ship's AI
--=======================================
function StationaryTrackEnemyFacing(indx)
--indx is the myEnemies table index assigned to the enemy

    for i = 1,targetCount do
        if myEnemies[indx].E_TOW == "no" then --Enemy not towing
            if myTargets[i].T_TOW == "no" then --Target free
                StationaryTrackSetTravelDirection(indx, myTargets[i].T_X + 10, myTargets[i].T_Y + 10)
            end
        end
    end
end


--=======================================
-- function:  StationaryTrackSetTravelDirection(indx, tX, tY)
-- author:    Paul Schuytema
-- created:   February 1, 2004
-- returns:   nothing (process)
-- descrip:   sets the enemy's rotation based upon the coordinates of its goal
--=======================================
function StationaryTrackSetTravelDirection(indx, tX, tY)
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
SetEventHandler("StationaryTrackEvent")


--Event handler
function StationaryTrackEvent(id, eventCode)

	if (eventCode == GUI_REENTER_INTERFACE)  or (eventCode == GUI_ENTER_INTERFACE) then
        MakeWorld()
        StationaryTrackSetUp()
        StationaryTrackEnemyInit()
        StationaryTrackTargetInit()
        StartTimer(refreshRate)
    end

    if eventCode == GUI_KEY_PRESS then
    	if id == 27 then --Esc
            RunGUI("GUI_MainMenu.lua")
    	end

        if id == 116 then --t
            StationaryTrackPutTarget()
        end

        if id == 110 then --n
            RunGUI("GUI_StationaryTrack.lua")
        end
    end


    if eventCode == GUI_TIMER_EXPIRED then
        --Directs each enemy based on its own REACT setting
        timeCounter = timeCounter + 1
        for indx = 1,enemyCount do
            if (timeCounter/myEnemies[indx].REACT) == (math.floor(timeCounter/myEnemies[indx].REACT)) then
                StationaryTrackEnemyFacing(indx)
            end
        end

        --Updates entities
        DrawEnemyShip()
        StationaryTrackDrawTargets()
        --Checks for various collisions
        EnemyTowCheck()
        --Resets the timer
        StartTimer(refreshRate)
    end
end
