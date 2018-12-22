--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  GUI_Escape.lua
-- author:    Paul Schuytema
-- created:   February 12, 2005
-- descrip:   Escape GUI
--=======================================

-- first steps
SetCoordTrans(800, 600)


--dofile calls for sub-GUIs


--100s Sprites
CreateItem(GUI_ESCAPE + 100, "Sprite", "ui_bg_escape.bmp")
SetItemPosition(GUI_ESCAPE + 100, 226, 32, 348, 460)

--200s Text items
-- to resume current game
CreateItem(GUI_ESCAPE + 300, "Button", "uib_resume_up.bmp", "uib_resume_hv.bmp", "uib_resume_dn.bmp");
SetItemPosition(GUI_ESCAPE + 300, 254, 229, 293, 67);

-- to save current game
CreateItem(GUI_ESCAPE + 301, "Button", "uib_save_up.bmp", "uib_save_hv.bmp", "uib_save_dn.bmp");
SetItemPosition(GUI_ESCAPE + 301, 254, 314, 293, 67);

-- to resume current game
CreateItem(GUI_ESCAPE + 302, "Button", "uib_mm_up.bmp", "uib_mm_hv.bmp", "uib_mm_dn.bmp");
SetItemPosition(GUI_ESCAPE + 302, 254, 399, 293, 67);


--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)


--functions related to this GUI (not used anywhere else)



--Event handler
function EscapeEvent(id, eventCode)
    result = 0

    if eventCode == GUI_KEY_PRESS then
    	if id == 27 then --Esc
            if gEscapeOn == 1 then
                --HideCursor()
                ClearGUI(GUI_ESCAPE)
                gEscapeOn = 0
                result = 1
                StartTimer(refreshRate)
            end
    	end
    end

    if eventCode == GUI_EVENT_BUTTON_UP then

        if id == GUI_ESCAPE + 300 then
        	PlaySound(1, "button.wav")
            --HideCursor()
            ClearGUI(GUI_ESCAPE)
            gEscapeOn = 0
            result = 1
            StartTimer(refreshRate)
        end

        if id == GUI_ESCAPE + 301 then
        	PlaySound(1, "button.wav")
            SaveGame()
            result = 1
        end

        if id == GUI_ESCAPE + 302 then
        	PlaySound(1, "button.wav")
            gEscapeOn = 0
            RunGUI("GUI_MainMenu.lua")
            --ShowCursor()
        end
	end
    if gEscapeOn == 1 then
        result = 1
    end
    return result
end
