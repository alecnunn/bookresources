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
dofile("Scripts\\GUI_Escape.lua")


--100s Sprites
CreateItem(GUI_INGAME + 100, "Sprite", "ui_bg_scoreboard.bmp")
SetItemPosition(GUI_INGAME + 100, 22, 22, 178, 89)


--200s Text items
--Projectiles fired text
CreateItem(GUI_INGAME + 201,"TextField")
SetItemPosition(GUI_INGAME + 201, 26, 48, 0, 0)
SetFont(GUI_INGAME + 201, "Arial", 18)
ItemCommand(GUI_INGAME + 201, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 201, "SetString", GetText(GUI_INGAME + 201))

--Stolen targets text
CreateItem(GUI_INGAME + 202,"TextField")
SetItemPosition(GUI_INGAME + 202, 26, 68, 0, 0)
SetFont(GUI_INGAME + 202, "Arial", 18)
ItemCommand(GUI_INGAME + 202, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 202, "SetString", GetText(GUI_INGAME + 202))

--Score text
CreateItem(GUI_INGAME + 203,"TextField")
SetItemPosition(GUI_INGAME + 203, 26, 88, 0, 0)
SetFont(GUI_INGAME + 203, "Arial", 18)
ItemCommand(GUI_INGAME + 203, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 203, "SetString", GetText(GUI_INGAME + 203))

--Number of projectiles fired by the player
CreateItem(GUI_INGAME + 204,"TextField")
SetItemPosition(GUI_INGAME + 204, 150, 48, 0, 0)
SetFont(GUI_INGAME + 204, "Arial", 18)
ItemCommand(GUI_INGAME + 204, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 204, "SetString", "0")

--Number of targets stolen by enemies
CreateItem(GUI_INGAME + 205,"TextField")
SetItemPosition(GUI_INGAME + 205, 120, 68, 0, 0)
SetFont(GUI_INGAME + 205, "Arial", 18)
ItemCommand(GUI_INGAME + 205, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 205, "SetString", "0")

--Player's score
CreateItem(GUI_INGAME + 206,"TextField")
SetItemPosition(GUI_INGAME + 206, 75, 88, 0, 0)
SetFont(GUI_INGAME + 206, "Arial", 18)
ItemCommand(GUI_INGAME + 206, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 206, "SetString", "0")


--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--functions related to this GUI (not used anywhere else)


--=======================================
-- function:  InitialSetUp()
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   sets constants for the game
--=======================================
function InitialSetUp()

    --Initial player constants
    myRotation = 1 --Player's rotation (#)
    myX = 390      --Player's x coordinate (#)
    myY = 290      --Player's y coordinate (#)
    myXThrust = 0  --Player's thrust along the x-axis (#)
    myYThrust = 0  --Player's thrust along the y-axis (#)
    alive = "yes"  --Player's life status ("yes" or "no")

    --Initial limits
    respawnInterval = 20  --Number respawnCounter must reach to respawn player (#)

    --Initial setting of counters
    respawnCounter = 0    --Player's death period (#)
    score = 0             --Player's score (#)
    timeCounter = 0       --Passage of time (#)
    targetDoneCounter = 0 --Targets stolen by enemies (#)

    --Preferred game speed
    refreshRate = .1 --Seconds between timer expirations (#)

    --Initial GUI setup
    ItemCommand(GUI_INGAME + 201, "SetString", GetText(GUI_INGAME + 201))
    ItemCommand(GUI_INGAME + 202, "SetString", GetText(GUI_INGAME + 202))
    ItemCommand(GUI_INGAME + 203, "SetString", GetText(GUI_INGAME + 203))
    ItemCommand(GUI_INGAME + 204, "SetString", "0")
    ItemCommand(GUI_INGAME + 205, "SetString", "0")
    ItemCommand(GUI_INGAME + 206, "SetString", "0")
    ClearGUI(GUI_ESCAPE)
    gEscapeOn = 0
    masterCellID = 100000 --Constant used for the IDs of the world's boundaries
end


--=======================================
-- function:  UpdateScore(enemyType)
-- author:    Nick Carlson
-- created:   February 11, 2005
-- returns:   nothing (process)
-- descrip:   increments and displays the player's score
--=======================================
function UpdateScore(enemyType)
--enemyType is the TYPE value of the killed enemy

    if enemyType == 1 then      --Box grabbers
        score = score + 50
    elseif enemyType == 2 then  --Shooters
        score = score + 100
    elseif enemyType == 3 then  --Rammers
        score = score + 200
    else                        --Hybrids
        score = score + 300
    end
    ItemCommand(GUI_INGAME + 206, "SetString", CommaFormatBigInteger(score))
end


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
-- function:  DrawCell(x,y)
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






--Initial run sets the event handler
SetEventHandler("InGameEvent")


--Event handler
function InGameEvent(id, eventCode)

    if EscapeEvent(id, eventCode) ~= 1 then


	if (eventCode == GUI_REENTER_INTERFACE)  or (eventCode == GUI_ENTER_INTERFACE) then
            InitialSetUp()
            MakeWorld()
            EnemyInit()
            TargetInit()
            ProjectileInit()
            StartTimer(refreshRate)
    end

    if eventCode == GUI_KEY_PRESS then
		if id == 27 then --Esc
            if gEscapeOn == 0 then
                RestoreGUI(GUI_ESCAPE)
                gEscapeOn = 1
            end
		end

        if id == gTurnLeftKey then --Default [
            --Turns player's ship counterclockwise
            myRotation = myRotation - 1
            if myRotation < 1 then
                myRotation = 8
            end
		end

        if id == gTurnRightKey then --Default ]
            --Turns player's ship clockwise
            myRotation = myRotation + 1
            if myRotation > 8 then
                myRotation = 1
            end
		end

        if id == gThrustKey then --Default space bar
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

        if id == gShootKey then --Default p
            --Fires a projectile from the player
            if alive == "yes" then
                FireProjectile("player", myXThrust, myYThrust)
            end
        end
	end


    if eventCode == GUI_TIMER_EXPIRED then
        if gEscapeOn == 0 then
            if alive == "yes" then
                --Refreshes the player's ship image
                DrawShip(myRotation, myXThrust, myYThrust)
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
                    EnemyFacing(indx)
                end
            end

            --Updates entities
            DrawProjectile()
            DrawEnemyShip()
            DrawTargets()
            --Checks for various collisions
            EnemyHitCheck()
            EnemyTowCheck()
            end
            --Resets the timer
            StartTimer(refreshRate)
	    end
    end --Escape Sub-GUI
end
