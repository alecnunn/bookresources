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
SetItemPosition(GUI_MAIN_MENU + 201, 66, 364, 0, 0)
SetFont(GUI_MAIN_MENU + 201, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 201, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 201, "SetString", "date")

--High score date 2
CreateItem(GUI_MAIN_MENU + 202,"TextField")
SetItemPosition(GUI_MAIN_MENU + 202, 66, 384, 0, 0)
SetFont(GUI_MAIN_MENU + 202, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 202, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 202, "SetString", "date")

--High score date 3
CreateItem(GUI_MAIN_MENU + 203,"TextField")
SetItemPosition(GUI_MAIN_MENU + 203, 66, 404, 0, 0)
SetFont(GUI_MAIN_MENU + 203, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 203, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 203, "SetString", "date")

--High score date 4
CreateItem(GUI_MAIN_MENU + 204,"TextField")
SetItemPosition(GUI_MAIN_MENU + 204, 66, 424, 0, 0)
SetFont(GUI_MAIN_MENU + 204, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 204, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 204, "SetString", "date")

--High score date 5
CreateItem(GUI_MAIN_MENU + 205,"TextField")
SetItemPosition(GUI_MAIN_MENU + 205, 66, 444, 0, 0)
SetFont(GUI_MAIN_MENU + 205, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 205, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 205, "SetString", "date")

--High score date 6
CreateItem(GUI_MAIN_MENU + 206,"TextField")
SetItemPosition(GUI_MAIN_MENU + 206, 66, 464, 0, 0)
SetFont(GUI_MAIN_MENU + 206, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 206, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 206, "SetString", "date")

--High score date 7
CreateItem(GUI_MAIN_MENU + 207,"TextField")
SetItemPosition(GUI_MAIN_MENU + 207, 66, 484, 0, 0)
SetFont(GUI_MAIN_MENU + 207, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 207, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 207, "SetString", "date")

--High score date 8
CreateItem(GUI_MAIN_MENU + 208,"TextField")
SetItemPosition(GUI_MAIN_MENU + 208, 66, 504, 0, 0)
SetFont(GUI_MAIN_MENU + 208, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 208, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 208, "SetString", "date")

--High score date 9
CreateItem(GUI_MAIN_MENU + 209,"TextField")
SetItemPosition(GUI_MAIN_MENU + 209, 66, 524, 0, 0)
SetFont(GUI_MAIN_MENU + 209, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 209, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 209, "SetString", "date")

--High score date 10
CreateItem(GUI_MAIN_MENU + 210,"TextField")
SetItemPosition(GUI_MAIN_MENU + 210, 66, 544, 0, 0)
SetFont(GUI_MAIN_MENU + 210, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 210, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 210, "SetString", "date")

--High score amount 1
CreateItem(GUI_MAIN_MENU + 211,"TextField")
SetItemPosition(GUI_MAIN_MENU + 211, 290, 364, 0, 0)
SetFont(GUI_MAIN_MENU + 211, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 211, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 211, "SetString", "0000")

--High score amount 2
CreateItem(GUI_MAIN_MENU + 212,"TextField")
SetItemPosition(GUI_MAIN_MENU + 212, 290, 384, 0, 0)
SetFont(GUI_MAIN_MENU + 212, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 212, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 212, "SetString", "0000")

--High score amount 3
CreateItem(GUI_MAIN_MENU + 213,"TextField")
SetItemPosition(GUI_MAIN_MENU + 213, 290, 404, 0, 0)
SetFont(GUI_MAIN_MENU + 213, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 213, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 213, "SetString", "0000")

--High score amount 4
CreateItem(GUI_MAIN_MENU + 214,"TextField")
SetItemPosition(GUI_MAIN_MENU + 214, 290, 424, 0, 0)
SetFont(GUI_MAIN_MENU + 214, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 214, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 214, "SetString", "0000")

--High score amount 5
CreateItem(GUI_MAIN_MENU + 215,"TextField")
SetItemPosition(GUI_MAIN_MENU + 215, 290, 444, 0, 0)
SetFont(GUI_MAIN_MENU + 215, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 215, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 215, "SetString", "0000")

--High score amount 6
CreateItem(GUI_MAIN_MENU + 216,"TextField")
SetItemPosition(GUI_MAIN_MENU + 216, 290, 464, 0, 0)
SetFont(GUI_MAIN_MENU + 216, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 216, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 216, "SetString", "0000")

--High score amount 7
CreateItem(GUI_MAIN_MENU + 217,"TextField")
SetItemPosition(GUI_MAIN_MENU + 217, 290, 484, 0, 0)
SetFont(GUI_MAIN_MENU + 217, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 217, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 217, "SetString", "0000")

--High score amount 8
CreateItem(GUI_MAIN_MENU + 218,"TextField")
SetItemPosition(GUI_MAIN_MENU + 218, 290, 504, 0, 0)
SetFont(GUI_MAIN_MENU + 218, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 218, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 218, "SetString", "0000")

--High score amount 9
CreateItem(GUI_MAIN_MENU + 219,"TextField")
SetItemPosition(GUI_MAIN_MENU + 219, 290, 524, 0, 0)
SetFont(GUI_MAIN_MENU + 219, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 219, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 219, "SetString", "0000")

--High score amount 10
CreateItem(GUI_MAIN_MENU + 220,"TextField")
SetItemPosition(GUI_MAIN_MENU + 220, 290, 544, 0, 0)
SetFont(GUI_MAIN_MENU + 220, "Arial", 22)
ItemCommand(GUI_MAIN_MENU + 220, "SetColor", 218,216,253,255)
ItemCommand(GUI_MAIN_MENU + 220, "SetString", "0000")

--300s Buttons
-- to start a new game
CreateItem(GUI_MAIN_MENU + 300, "Button", "uib_newgame_up.bmp", "uib_newgame_hv.bmp", "uib_newgame_dn.bmp");
SetItemPosition(GUI_MAIN_MENU + 300, 452, 123, 293, 67);

-- to go back to the saved game
CreateItem(GUI_MAIN_MENU + 301, "Button", "uib_savedgame_up.bmp", "uib_savedgame_hv.bmp", "uib_savedgame_dn.bmp");
SetItemPosition(GUI_MAIN_MENU + 301, 452, 223, 293, 67);

-- to go to menu to see and possibly remap the control keys
CreateItem(GUI_MAIN_MENU + 302, "Button", "uib_controls_up.bmp", "uib_controls_hv.bmp", "uib_controls_dn.bmp");
SetItemPosition(GUI_MAIN_MENU + 302, 452, 326, 293, 67);

--to exit the game completely
CreateItem(GUI_MAIN_MENU + 303, "Button", "uib_exit_up.bmp", "uib_exit_hv.bmp", "uib_exit_dn.bmp");
SetItemPosition(GUI_MAIN_MENU + 303, 452, 428, 293, 67);


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--functions related to this GUI (not used anywhere else)


--first run calls
SetEventHandler("MainMenuEvent")


--Event handler
function MainMenuEvent(id, eventCode)

    if eventCode == GUI_ENTER_INTERFACE then
        LoadSettings()
        SortScoreLists()
        UpdateMainMenu()
    end

    if eventCode == GUI_REENTER_INTERFACE then
        SortScoreLists()
        UpdateMainMenu()
    end

    if eventCode == GUI_EVENT_BUTTON_UP then

        if id == GUI_MAIN_MENU + 300 then
        	PlaySound(1, "button.wav")
            RunGUI("GUI_InGame.lua")
        end

	    if id == GUI_MAIN_MENU + 301 then
		    PlaySound(1, "button.wav")
            RunGUI("GUI_InGame.lua")
            LoadGame()
		end

        if id == GUI_MAIN_MENU + 302 then
			PlaySound(1, "button.wav")
            RunGUI("GUI_KeySelect.lua")
		end


        if id == GUI_MAIN_MENU + 303 then
        	PlaySound(1, "button.wav")
            SaveSettings()
            QuitProgram()
        end
    end
end
