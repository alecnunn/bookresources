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

    --GUI sprite set up will go here later

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
        --GUI set up of targets goes here later
    end
end



--=======================================
-- function:  ProjectileInit()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   This initializes the projectile table
--=======================================
function ProjectileInit()
--Creates myProjectiles table

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
-- function:  SaveGame()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   This saves the game data into a Lua file
--=======================================
function SaveGame()
--Saves the current game

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
        myFile:write(string.format("%s%s%s%s%s%s%s%s%s%s", "ItemCommand(GUI_INGAME + 201, ", string.char (34), "SetString", string.char (34), ",", string.char (34), "Projectiles Fired: ", CommaFormatBigInteger(playerProjectiles), string.char (34), ")"))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%s%s%s%s%s%s%s%s%s", "ItemCommand(GUI_INGAME + 202, ", string.char (34), "SetString", string.char (34), ",", string.char (34), "Targets Lost: ", CommaFormatBigInteger(targetDoneCounter), string.char (34), ")"))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%s%s%s%s%s%s%s%s%s", "ItemCommand(GUI_INGAME + 203, ", string.char (34), "SetString", string.char (34), ",", string.char (34), "Score: ", CommaFormatBigInteger(score), string.char (34), ")"))
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        io.close(myFile)
    end
end


--=======================================
-- function:  LoadGame()
-- author:    Nick Carlson
-- created:   February 14, 2005
-- returns:   nothing (process)
-- descrip:   This clears out the game data and then loads a save game file
--=======================================
function LoadGame()
--Loads a saved game

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
        --Reads the file, closes it, and begins the action
        dofile("SaveGames\\Take_Away_Saved_Game.lua")
        fileName:close()
        StartTimer(refreshRate)
    end
end

