--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_MainMenu.lua
-- author:    Paul Schuytema
-- created:   February 10, 2005
-- descrip:   Main menu screen
--=======================================

-- first steps


--dofile calls for sub-GUIs


--100s Sprites
CreateItem(GUI_MAIN_MENU + 100, "Sprite", "ui_bg_mainmenu.bmp")
SetItemPosition(GUI_MAIN_MENU + 100, 0, 0, 800, 600)


--200s Text items
--Saved game date
CreateItem(GUI_MAIN_MENU + 200,"TextField")
SetItemPosition(GUI_MAIN_MENU + 200, 498, 292, 0, 0)
SetFont(GUI_MAIN_MENU + 200, "Arial", 24)
ItemCommand(GUI_MAIN_MENU + 200, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 200, "SetString", "")

--High score date 1
CreateItem(GUI_MAIN_MENU + 201,"TextField")
SetItemPosition(GUI_MAIN_MENU + 201, 452, 50, 0, 0)
SetFont(GUI_MAIN_MENU + 201, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 201, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 201, "SetString", "GUI_StationaryTrack.lua")

--High score date 2
CreateItem(GUI_MAIN_MENU + 202,"TextField")
SetItemPosition(GUI_MAIN_MENU + 202, 452, 100, 0, 0)
SetFont(GUI_MAIN_MENU + 202, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 202, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 202, "SetString", "GUI_ProximityTrack.lua")

--High score date 3
CreateItem(GUI_MAIN_MENU + 203,"TextField")
SetItemPosition(GUI_MAIN_MENU + 203, 452, 150, 0, 0)
SetFont(GUI_MAIN_MENU + 203, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 203, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 203, "SetString", "GUI_MovingTrack.lua")

--High score date 4
CreateItem(GUI_MAIN_MENU + 204,"TextField")
SetItemPosition(GUI_MAIN_MENU + 204, 452, 200, 0, 0)
SetFont(GUI_MAIN_MENU + 204, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 204, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 204, "SetString", "GUI_AnticipationTrack.lua")

--High score date 5
CreateItem(GUI_MAIN_MENU + 205,"TextField")
SetItemPosition(GUI_MAIN_MENU + 205, 452, 250, 0, 0)
SetFont(GUI_MAIN_MENU + 205, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 205, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 205, "SetString", "GUI_TurretFire.lua")

--High score date 6
CreateItem(GUI_MAIN_MENU + 206,"TextField")
SetItemPosition(GUI_MAIN_MENU + 206, 452, 300, 0, 0)
SetFont(GUI_MAIN_MENU + 206, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 206, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 206, "SetString", "GUI_AvoidFire.lua")

--High score date 7
CreateItem(GUI_MAIN_MENU + 207,"TextField")
SetItemPosition(GUI_MAIN_MENU + 207, 452, 350, 0, 0)
SetFont(GUI_MAIN_MENU + 207, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 207, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 207, "SetString", "GUI_ProtectionFire.lua")

--High score date 8
CreateItem(GUI_MAIN_MENU + 208,"TextField")
SetItemPosition(GUI_MAIN_MENU + 208, 452, 400, 0, 0)
SetFont(GUI_MAIN_MENU + 208, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 208, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 208, "SetString", "GUI_DamageFire.lua")

--High score date 9
CreateItem(GUI_MAIN_MENU + 209,"TextField")
SetItemPosition(GUI_MAIN_MENU + 209, 66, 524, 0, 0)
SetFont(GUI_MAIN_MENU + 209, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 209, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 209, "SetString", "")

--High score date 10
CreateItem(GUI_MAIN_MENU + 210,"TextField")
SetItemPosition(GUI_MAIN_MENU + 210, 66, 544, 0, 0)
SetFont(GUI_MAIN_MENU + 210, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 210, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 210, "SetString", "")

--High score amount 1
CreateItem(GUI_MAIN_MENU + 211,"TextField")
SetItemPosition(GUI_MAIN_MENU + 211, 290, 364, 0, 0)
SetFont(GUI_MAIN_MENU + 211, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 211, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 211, "SetString", "")

--High score amount 2
CreateItem(GUI_MAIN_MENU + 212,"TextField")
SetItemPosition(GUI_MAIN_MENU + 212, 290, 384, 0, 0)
SetFont(GUI_MAIN_MENU + 212, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 212, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 212, "SetString", "")

--High score amount 3
CreateItem(GUI_MAIN_MENU + 213,"TextField")
SetItemPosition(GUI_MAIN_MENU + 213, 290, 404, 0, 0)
SetFont(GUI_MAIN_MENU + 213, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 213, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 213, "SetString", "")

--High score amount 4
CreateItem(GUI_MAIN_MENU + 214,"TextField")
SetItemPosition(GUI_MAIN_MENU + 214, 290, 424, 0, 0)
SetFont(GUI_MAIN_MENU + 214, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 214, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 214, "SetString", "")

--High score amount 5
CreateItem(GUI_MAIN_MENU + 215,"TextField")
SetItemPosition(GUI_MAIN_MENU + 215, 290, 444, 0, 0)
SetFont(GUI_MAIN_MENU + 215, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 215, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 215, "SetString", "")

--High score amount 6
CreateItem(GUI_MAIN_MENU + 216,"TextField")
SetItemPosition(GUI_MAIN_MENU + 216, 290, 464, 0, 0)
SetFont(GUI_MAIN_MENU + 216, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 216, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 216, "SetString", "")

--High score amount 7
CreateItem(GUI_MAIN_MENU + 217,"TextField")
SetItemPosition(GUI_MAIN_MENU + 217, 290, 484, 0, 0)
SetFont(GUI_MAIN_MENU + 217, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 217, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 217, "SetString", "")

--High score amount 8
CreateItem(GUI_MAIN_MENU + 218,"TextField")
SetItemPosition(GUI_MAIN_MENU + 218, 290, 504, 0, 0)
SetFont(GUI_MAIN_MENU + 218, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 218, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 218, "SetString", "")

--High score amount 9
CreateItem(GUI_MAIN_MENU + 219,"TextField")
SetItemPosition(GUI_MAIN_MENU + 219, 290, 524, 0, 0)
SetFont(GUI_MAIN_MENU + 219, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 219, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 219, "SetString", "")

--High score amount 10
CreateItem(GUI_MAIN_MENU + 220,"TextField")
SetItemPosition(GUI_MAIN_MENU + 220, 290, 544, 0, 0)
SetFont(GUI_MAIN_MENU + 220, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 220, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 220, "SetString", "")

--300s Buttons

--to exit the game completely
CreateItem(GUI_MAIN_MENU + 300, "Button", "uib_exit_up.bmp", "uib_exit_hv.bmp", "uib_exit_dn.bmp")
SetItemPosition(GUI_MAIN_MENU + 300, 452, 500, 293, 67)

-- to start a new game
CreateItem(GUI_MAIN_MENU + 301, "Button", "uib_newgame_up.bmp", "uib_newgame_hv.bmp", "uib_newgame_dn.bmp")
SetItemPosition(GUI_MAIN_MENU + 301, 675, 50, 90, 20)

-- to start a new game
CreateItem(GUI_MAIN_MENU + 302, "Button", "uib_newgame_up.bmp", "uib_newgame_hv.bmp", "uib_newgame_dn.bmp")
SetItemPosition(GUI_MAIN_MENU + 302, 675, 100, 90, 20)

-- to start a new game
CreateItem(GUI_MAIN_MENU + 303, "Button", "uib_newgame_up.bmp", "uib_newgame_hv.bmp", "uib_newgame_dn.bmp")
SetItemPosition(GUI_MAIN_MENU + 303, 675, 150, 90, 20)

-- to start a new game
CreateItem(GUI_MAIN_MENU + 304, "Button", "uib_newgame_up.bmp", "uib_newgame_hv.bmp", "uib_newgame_dn.bmp")
SetItemPosition(GUI_MAIN_MENU + 304, 675, 200, 90, 20)

-- to start a new game
CreateItem(GUI_MAIN_MENU + 305, "Button", "uib_newgame_up.bmp", "uib_newgame_hv.bmp", "uib_newgame_dn.bmp")
SetItemPosition(GUI_MAIN_MENU + 305, 675, 250, 90, 20)

-- to start a new game
CreateItem(GUI_MAIN_MENU + 306, "Button", "uib_newgame_up.bmp", "uib_newgame_hv.bmp", "uib_newgame_dn.bmp")
SetItemPosition(GUI_MAIN_MENU + 306, 675, 300, 90, 20)

-- to start a new game
CreateItem(GUI_MAIN_MENU + 307, "Button", "uib_newgame_up.bmp", "uib_newgame_hv.bmp", "uib_newgame_dn.bmp")
SetItemPosition(GUI_MAIN_MENU + 307, 675, 350, 90, 20)

-- to start a new game
CreateItem(GUI_MAIN_MENU + 308, "Button", "uib_newgame_up.bmp", "uib_newgame_hv.bmp", "uib_newgame_dn.bmp")
SetItemPosition(GUI_MAIN_MENU + 308, 675, 400, 90, 20)

--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--functions related to this GUI (not used anywhere else)


--first run calls
SetEventHandler("MainMenuEvent")


--Event handler
function MainMenuEvent(id, eventCode)

    if eventCode == GUI_ENTER_INTERFACE then
    end

    if eventCode == GUI_REENTER_INTERFACE then
    end

    if eventCode == GUI_EVENT_BUTTON_UP then

        if id == GUI_MAIN_MENU + 300 then
            QuitProgram()
        end

	    if id == GUI_MAIN_MENU + 301 then
            gScenarioScript = "GUI_StationaryTrack.lua"
            RunGUI(gScenarioScript)
		end

        if id == GUI_MAIN_MENU + 302 then
            gScenarioScript = "GUI_ProximityTrack.lua"
            RunGUI(gScenarioScript)
		end

        if id == GUI_MAIN_MENU + 303 then
            gScenarioScript = "GUI_MovingTrack.lua"
            RunGUI(gScenarioScript)
        end

        if id == GUI_MAIN_MENU + 304 then
            gScenarioScript = "GUI_AnticipationTrack.lua"
            RunGUI(gScenarioScript)
        end

        if id == GUI_MAIN_MENU + 305 then
            gScenarioScript = "GUI_TurretFire.lua"
            RunGUI(gScenarioScript)
        end

        if id == GUI_MAIN_MENU + 306 then
            gScenarioScript = "GUI_AvoidFire.lua"
            RunGUI(gScenarioScript)
        end

        if id == GUI_MAIN_MENU + 307 then
            gScenarioScript = "GUI_ProtectionFire.lua"
            RunGUI(gScenarioScript)
        end

        if id == GUI_MAIN_MENU + 308 then
            gScenarioScript = "GUI_DamageFire.lua"
            RunGUI(gScenarioScript)
        end
    end
end
