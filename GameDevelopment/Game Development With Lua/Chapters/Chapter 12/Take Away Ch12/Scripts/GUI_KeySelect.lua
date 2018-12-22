--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_KeySelect.lua
-- author:    Paul Schuytema
-- created:   February 15, 2005
-- descrip:   Screen that allows player to configure their control keys
--=======================================

-- first steps


--dofile calls for sub-GUIs


--100s Sprites
CreateItem(GUI_KEY_SELECT + 100, "Sprite", "ui_bg_keyboard.bmp")
SetItemPosition(GUI_KEY_SELECT + 100, 0, 0, 800, 600)


--200s Text items
--Thrust control
CreateItem(GUI_KEY_SELECT + 200,"TextField")
SetItemPosition(GUI_KEY_SELECT + 200, 555, 290, 0, 0)
SetFont(GUI_KEY_SELECT + 200, "Arial", 22)
ItemCommand(GUI_KEY_SELECT + 200, "SetColor", 176,173,254,255)
ItemCommand(GUI_KEY_SELECT + 200, "SetString", ASCIIToString(gThrustKey))

--Shoot control
CreateItem(GUI_KEY_SELECT + 201,"TextField")
SetItemPosition(GUI_KEY_SELECT + 201, 555, 331, 0, 0)
SetFont(GUI_KEY_SELECT + 201, "Arial", 22)
ItemCommand(GUI_KEY_SELECT + 201, "SetColor", 176,173,254,255)
ItemCommand(GUI_KEY_SELECT + 201, "SetString", ASCIIToString(gShootKey))

--Turn right
CreateItem(GUI_KEY_SELECT + 202,"TextField")
SetItemPosition(GUI_KEY_SELECT + 202, 555, 369, 0, 0)
SetFont(GUI_KEY_SELECT + 202, "Arial", 22)
ItemCommand(GUI_KEY_SELECT + 202, "SetColor", 176,173,254,255)
ItemCommand(GUI_KEY_SELECT + 202, "SetString", ASCIIToString(gTurnRightKey))

--Turn left
CreateItem(GUI_KEY_SELECT + 203,"TextField")
SetItemPosition(GUI_KEY_SELECT + 203, 555, 409, 0, 0)
SetFont(GUI_KEY_SELECT + 203, "Arial", 22)
ItemCommand(GUI_KEY_SELECT + 203, "SetColor", 176,173,254,255)
ItemCommand(GUI_KEY_SELECT + 203, "SetString", ASCIIToString(gTurnLeftKey))


--300s Buttons
--Play button
CreateItem(GUI_KEY_SELECT + 300, "Button", "uib_back_up.bmp", "uib_back_hv.bmp", "uib_back_dn.bmp");
SetItemPosition(GUI_KEY_SELECT + 300, 452, 493, 293, 67)

CreateItem(GUI_KEY_SELECT + 301, "Button", "uib_key_up.bmp", "uib_key_hv.bmp", "uib_key_dn.bmp");
SetItemPosition(GUI_KEY_SELECT + 301, 241, 283, 297, 33)

CreateItem(GUI_KEY_SELECT + 302, "Button", "uib_key_up.bmp", "uib_key_hv.bmp", "uib_key_dn.bmp");
SetItemPosition(GUI_KEY_SELECT + 302, 241, 323, 297, 33)

CreateItem(GUI_KEY_SELECT + 303, "Button", "uib_key_up.bmp", "uib_key_hv.bmp", "uib_key_dn.bmp");
SetItemPosition(GUI_KEY_SELECT + 303, 241, 363, 297, 33)

CreateItem(GUI_KEY_SELECT + 304, "Button", "uib_key_up.bmp", "uib_key_hv.bmp", "uib_key_dn.bmp");
SetItemPosition(GUI_KEY_SELECT + 304, 241, 403, 297, 33)





--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--functions related to this GUI (not used anywhere else)


--Initial run sets the event handler
SetEventHandler("KeySelectEvent")


--Event handler
function KeySelectEvent(id, eventCode)

    if (eventCode == GUI_REENTER_INTERFACE)  or (eventCode == GUI_ENTER_INTERFACE) then
        keySelect = 0
        HighlightText(0)
    end

    if eventCode == GUI_EVENT_BUTTON_UP then

        if id == GUI_KEY_SELECT + 300 then
        	PlaySound(1, "button.wav")
            RunGUI("GUI_MainMenu.lua")
        end

        if id == GUI_KEY_SELECT + 301 then
            PlaySound(1, "button.wav")
        	keySelect = 1
            HighlightText(GUI_KEY_SELECT + 200)
        end

        if id == GUI_KEY_SELECT + 302 then
            PlaySound(1, "button.wav")
        	keySelect = 2
            HighlightText(GUI_KEY_SELECT + 201)
        end

        if id == GUI_KEY_SELECT + 303 then
            PlaySound(1, "button.wav")
        	keySelect = 3
            HighlightText(GUI_KEY_SELECT + 202)
        end

        if id == GUI_KEY_SELECT + 304 then
            PlaySound(1, "button.wav")
        	keySelect = 4
            HighlightText(GUI_KEY_SELECT + 203)
        end
    end

    if eventCode == GUI_KEY_PRESS then

        if keySelect == 0 then
            --Nothing
        elseif keySelect == 1 then
            --Thrust
            if (gShootKey ~= id) and (gTurnLeftKey ~= id) and (gTurnRightKey ~= id) and (27 ~= id) then
                gThrustKey = id
                ItemCommand(GUI_KEY_SELECT + 200, "SetString", ASCIIToString(id))
            end
            keySelect = 0
            HighlightText(0)
        elseif keySelect == 2 then
            --Shoot
            if (gThrustKey ~= id) and (gTurnLeftKey ~= id) and (gTurnRightKey ~= id) and (27 ~= id) then
                gShootKey = id
                ItemCommand(GUI_KEY_SELECT + 201, "SetString", ASCIIToString(id))
            end
            keySelect = 0
            HighlightText(0)
        elseif keySelect == 3 then
            --Turn Right
            if (gThrustKey ~= id) and (gShootKey ~= id) and (gTurnLeftKey ~= id) and (27 ~= id) then
                gTurnRightKey = id
                ItemCommand(GUI_KEY_SELECT + 202, "SetString", ASCIIToString(id))
            end
            keySelect = 0
            HighlightText(0)
        elseif keySelect == 4 then
            --Turn Left
            if (gThrustKey ~= id) and (gShootKey ~= id) and (gTurnRightKey ~= id) and (27 ~= id) then
                gTurnLeftKey = id
                ItemCommand(GUI_KEY_SELECT + 203, "SetString", ASCIIToString(id))
            end
            keySelect = 0
            HighlightText(0)
        end
    end
end







