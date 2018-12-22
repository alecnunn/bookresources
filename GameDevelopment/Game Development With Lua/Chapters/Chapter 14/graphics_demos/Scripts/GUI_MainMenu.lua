--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_MainMenu.lua
-- author:    Nick Carlson
-- created:   April 15, 2005
-- descrip:   Main menu screen
--=======================================

-- first steps


--dofile calls for sub-GUIs


--100s Sprites
CreateItem(GUI_MAIN_MENU + 100, "Sprite", "map2.bmp")
SetItemPosition(GUI_MAIN_MENU + 100, 0, 0, 800, 600)


--200s Text items
--Title
CreateItem(GUI_MAIN_MENU + 200,"TextField")
SetItemPosition(GUI_MAIN_MENU + 200, 50, 280, 0, 0)
SetFont(GUI_MAIN_MENU + 200, "Arial", 50)
ItemCommand(GUI_MAIN_MENU + 200, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 200, "SetString", "Graphics Demo")

--Button 1
CreateItem(GUI_MAIN_MENU + 201,"TextField")
SetItemPosition(GUI_MAIN_MENU + 201, 452, 50, 0, 0)
SetFont(GUI_MAIN_MENU + 201, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 201, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 201, "SetString", "GUI_Fingerprint.lua")

--Button 2
CreateItem(GUI_MAIN_MENU + 202,"TextField")
SetItemPosition(GUI_MAIN_MENU + 202, 452, 100, 0, 0)
SetFont(GUI_MAIN_MENU + 202, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 202, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 202, "SetString", "GUI_Explosion.lua")

--Button 3
CreateItem(GUI_MAIN_MENU + 203,"TextField")
SetItemPosition(GUI_MAIN_MENU + 203, 452, 150, 0, 0)
SetFont(GUI_MAIN_MENU + 203, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 203, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 203, "SetString", "GUI_Rotation.lua")

--Button 4
CreateItem(GUI_MAIN_MENU + 204,"TextField")
SetItemPosition(GUI_MAIN_MENU + 204, 452, 200, 0, 0)
SetFont(GUI_MAIN_MENU + 204, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 204, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 204, "SetString", "GUI_Movement.lua")

--Button 5
CreateItem(GUI_MAIN_MENU + 205,"TextField")
SetItemPosition(GUI_MAIN_MENU + 205, 452, 250, 0, 0)
SetFont(GUI_MAIN_MENU + 205, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 205, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 205, "SetString", "GUI_Collision.lua")

--Button 6
CreateItem(GUI_MAIN_MENU + 206,"TextField")
SetItemPosition(GUI_MAIN_MENU + 206, 452, 300, 0, 0)
SetFont(GUI_MAIN_MENU + 206, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 206, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 206, "SetString", "GUI_TankDemo.lua")


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
            gScenarioScript = "GUI_Fingerprint.lua"
            RunGUI(gScenarioScript)
		end

        if id == GUI_MAIN_MENU + 302 then
            gScenarioScript = "GUI_Explosion.lua"
            RunGUI(gScenarioScript)
		end

        if id == GUI_MAIN_MENU + 303 then
            gScenarioScript = "GUI_Rotation.lua"
            RunGUI(gScenarioScript)
        end

        if id == GUI_MAIN_MENU + 304 then
            gScenarioScript = "GUI_Movement.lua"
            RunGUI(gScenarioScript)
        end

        if id == GUI_MAIN_MENU + 305 then
            gScenarioScript = "GUI_Collision.lua"
            RunGUI(gScenarioScript)
        end

        if id == GUI_MAIN_MENU + 306 then
            gScenarioScript = "GUI_TankDemo.lua"
            RunGUI(gScenarioScript)
        end

--        if id == GUI_MAIN_MENU + 307 then
--            gScenarioScript = "GUI_ProtectionFire.lua"
--            RunGUI(gScenarioScript)
--        end
--
--        if id == GUI_MAIN_MENU + 308 then
--            gScenarioScript = "GUI_DamageFire.lua"
--            RunGUI(gScenarioScript)
--        end
    end
end
