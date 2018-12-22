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
SetItemPosition(GUI_INGAME + 201, 26, 48, 200, 20)
SetFont(GUI_INGAME + 201, "Arial", 18)
ItemCommand(GUI_INGAME + 201, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 201, "SetString", GetText(GUI_INGAME + 201))

--Stolen targets text
CreateItem(GUI_INGAME + 202,"TextField")
SetItemPosition(GUI_INGAME + 202, 26, 68, 200, 20)
SetFont(GUI_INGAME + 202, "Arial", 18)
ItemCommand(GUI_INGAME + 202, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 202, "SetString", GetText(GUI_INGAME + 202))

--Score text
CreateItem(GUI_INGAME + 203,"TextField")
SetItemPosition(GUI_INGAME + 203, 26, 88, 200, 20)
SetFont(GUI_INGAME + 203, "Arial", 18)
ItemCommand(GUI_INGAME + 203, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 203, "SetString", GetText(GUI_INGAME + 203))

--Number of projectiles fired by the player
CreateItem(GUI_INGAME + 204,"TextField")
SetItemPosition(GUI_INGAME + 204, 150, 48, 200, 20)
SetFont(GUI_INGAME + 204, "Arial", 18)
ItemCommand(GUI_INGAME + 204, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 204, "SetString", "0")

--Number of targets stolen by enemies
CreateItem(GUI_INGAME + 205,"TextField")
SetItemPosition(GUI_INGAME + 205, 120, 68, 200, 20)
SetFont(GUI_INGAME + 205, "Arial", 18)
ItemCommand(GUI_INGAME + 205, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 205, "SetString", "0")

--Player's score
CreateItem(GUI_INGAME + 206,"TextField")
SetItemPosition(GUI_INGAME + 206, 75, 88, 200, 20)
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


--Initial run sets the event handler
SetEventHandler("InGameEvent")


--Event handler
function InGameEvent(id, eventCode)

    if EscapeEvent(id, eventCode) ~= 1 then


	if (eventCode == GUI_REENTER_INTERFACE)  or (eventCode == GUI_ENTER_INTERFACE) then
            InitialSetUp()
            EnemyInit()
            TargetInit()
            ProjectileInit()
            StartTimer(refreshRate)
    end

    if eventCode == GUI_EVENT_BUTTON_UP then
    end

	if eventCode == GUI_KEY_PRESS then
		if id == 27 then --Esc
            if gEscapeOn == 0 then
                RestoreGUI(GUI_ESCAPE)
                gEscapeOn = 1
            end
		end

	end


    if eventCode == GUI_TIMER_EXPIRED then
        if gEscapeOn == 0 then
            StartTimer(refreshRate)
	    end
    end
    end --Escape Sub-GUI
end
