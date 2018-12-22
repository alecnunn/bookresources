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


--100s Sprites

--board
CreateItem(100, "Sprite", "board.bmp")
SetItemPosition(100, 250, 150, 300, 300)

--X pieces
EX = 110
CreateItem(EX + 1, "Sprite", "piece_x.bmp")
SetItemPosition(EX + 1, 270, 170, 60, 60)
EnableObject(EX + 1, 0, 0)

CreateItem(EX + 2, "Sprite", "piece_x.bmp")
SetItemPosition(EX + 2, 370, 170, 60, 60)
EnableObject(EX + 2, 0, 0)

CreateItem(EX + 3, "Sprite", "piece_x.bmp")
SetItemPosition(EX + 3, 470, 170, 60, 60)
EnableObject(EX + 3, 0, 0)

CreateItem(EX + 4, "Sprite", "piece_x.bmp")
SetItemPosition(EX + 4, 270, 270, 60, 60)
EnableObject(EX + 4, 0, 0)

CreateItem(EX + 5, "Sprite", "piece_x.bmp")
SetItemPosition(EX + 5, 370, 270, 60, 60)
EnableObject(EX + 5, 0, 0)

CreateItem(EX + 6, "Sprite", "piece_x.bmp")
SetItemPosition(EX + 6, 470, 270, 60, 60)
EnableObject(EX + 6, 0, 0)

CreateItem(EX + 7, "Sprite", "piece_x.bmp")
SetItemPosition(EX + 7, 270, 370, 60, 60)
EnableObject(EX + 7, 0, 0)

CreateItem(EX + 8, "Sprite", "piece_x.bmp")
SetItemPosition(EX + 8, 370, 370, 60, 60)
EnableObject(EX + 8, 0, 0)

CreateItem(EX + 9, "Sprite", "piece_x.bmp")
SetItemPosition(EX + 9, 470, 370, 60, 60)
EnableObject(EX + 9, 0, 0)

--O pieces
OH = 120
CreateItem(OH + 1, "Sprite", "piece_o.bmp")
SetItemPosition(OH + 1, 270, 170, 60, 60)
EnableObject(OH + 1, 0, 0)

CreateItem(OH + 2, "Sprite", "piece_o.bmp")
SetItemPosition(OH + 2, 370, 170, 60, 60)
EnableObject(OH + 2, 0, 0)

CreateItem(OH + 3, "Sprite", "piece_o.bmp")
SetItemPosition(OH + 3, 470, 170, 60, 60)
EnableObject(OH + 3, 0, 0)

CreateItem(OH + 4, "Sprite", "piece_o.bmp")
SetItemPosition(OH + 4, 270, 270, 60, 60)
EnableObject(OH + 4, 0, 0)

CreateItem(OH + 5, "Sprite", "piece_o.bmp")
SetItemPosition(OH + 5, 370, 270, 60, 60)
EnableObject(OH + 5, 0, 0)

CreateItem(OH + 6, "Sprite", "piece_o.bmp")
SetItemPosition(OH + 6, 470, 270, 60, 60)
EnableObject(OH + 6, 0, 0)

CreateItem(OH + 7, "Sprite", "piece_o.bmp")
SetItemPosition(OH + 7, 270, 370, 60, 60)
EnableObject(OH + 7, 0, 0)

CreateItem(OH + 8, "Sprite", "piece_o.bmp")
SetItemPosition(OH + 8, 370, 370, 60, 60)
EnableObject(OH + 8, 0, 0)

CreateItem(OH + 9, "Sprite", "piece_o.bmp")
SetItemPosition(OH + 9, 470, 370, 60, 60)
EnableObject(OH + 9, 0, 0)

--result graphics

--bars
H_BAR = 130
CreateItem(H_BAR + 1, "Sprite", "bar_across.bmp")
SetItemPosition(H_BAR + 1, 300, 195, 200, 10)
EnableObject(H_BAR + 1, 0, 0)

CreateItem(H_BAR + 2, "Sprite", "bar_across.bmp")
SetItemPosition(H_BAR + 2, 300, 295, 200, 10)
EnableObject(H_BAR + 2, 0, 0)

CreateItem(H_BAR + 3, "Sprite", "bar_across.bmp")
SetItemPosition(H_BAR + 3, 300, 395, 200, 10)
EnableObject(H_BAR + 3, 0, 0)

V_BAR = 140
CreateItem(V_BAR + 1, "Sprite", "bar_down.bmp")
SetItemPosition(V_BAR + 1, 295, 200, 10, 200)
EnableObject(V_BAR + 1, 0, 0)

CreateItem(V_BAR + 2, "Sprite", "bar_down.bmp")
SetItemPosition(V_BAR + 2, 395, 200, 10, 200)
EnableObject(V_BAR + 2, 0, 0)

CreateItem(V_BAR + 3, "Sprite", "bar_down.bmp")
SetItemPosition(V_BAR + 3, 495, 200, 10, 200)
EnableObject(V_BAR + 3, 0, 0)

D_BAR = 150
CreateItem(D_BAR + 1, "Sprite", "bar_angle2.tga")
SetItemPosition(D_BAR + 1, 295, 195, 212, 212)
EnableObject(D_BAR + 1, 0, 0)

CreateItem(D_BAR + 2, "Sprite", "bar_angle1.tga")
SetItemPosition(D_BAR + 2, 295, 195, 212, 212)
EnableObject(D_BAR + 2, 0, 0)

--text

CreateItem(GUI_INGAME + 210,"TextField")
SetItemPosition(GUI_INGAME + 210, 270, 100, 200, 0)
SetFont(GUI_INGAME + 210, "Arial", 48)
ItemCommand(GUI_INGAME + 210, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 210, "SetString", "CAT'S GAME!")
EnableObject(GUI_INGAME + 210, 0, 0)

CreateItem(GUI_INGAME + 220,"TextField")
SetItemPosition(GUI_INGAME + 220, 330, 100, 200, 0)
SetFont(GUI_INGAME + 220, "Arial", 48)
ItemCommand(GUI_INGAME + 220, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 220, "SetString", "X WINS!")
EnableObject(GUI_INGAME + 220, 0, 0)

CreateItem(GUI_INGAME + 230,"TextField")
SetItemPosition(GUI_INGAME + 230, 330, 100, 200, 0)
SetFont(GUI_INGAME + 230, "Arial", 48)
ItemCommand(GUI_INGAME + 230, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 230, "SetString", "O WINS!")
EnableObject(GUI_INGAME + 230, 0, 0)




--200s Text items

CreateItem(GUI_INGAME + 200,"TextField")
SetItemPosition(GUI_INGAME + 200, 10, 10, 200, 20)
SetFont(GUI_INGAME + 200, "Arial", 24)
ItemCommand(GUI_INGAME + 200, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 200, "SetString", "{tic-tac-toe}")

CreateItem(GUI_INGAME + 240,"TextField")
SetItemPosition(GUI_INGAME + 240, 260, 500, 200, 20)
SetFont(GUI_INGAME + 240, "Arial", 24)
ItemCommand(GUI_INGAME + 240, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 240, "SetString", "tap the space bar to play again")
EnableObject(GUI_INGAME + 240, 0, 0)

CreateItem(GUI_INGAME + 250,"TextField")
SetItemPosition(GUI_INGAME + 250, 285, 90, 200, 20)
SetFont(GUI_INGAME + 250, "Arial", 24)
ItemCommand(GUI_INGAME + 250, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 250, "SetString", "X's turn: left click to place")
EnableObject(GUI_INGAME + 250, 0, 0)


--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)



--Initial run sets the event handler
SetEventHandler("InGameEvent")


--Event handler
function InGameEvent(id, eventCode)

    if eventCode == GUI_ENTER_INTERFACE then
        InitGame()
        if curTurn == OH then
            StartTimer(1.5)
        end
    end

    if eventCode == GUI_KEY_PRESS then
        if id == 32 then --space bar
            if theWinner ~= -1 then
                InitGame()
                if curTurn == OH then
                    StartTimer(1.5)
                end
            end
        end
    end

    if eventCode == GUI_MOUSE_BUTTON_DOWN then
    end

    if eventCode == GUI_MOUSE_BUTTON_UP then
        if (theWinner == -1) and (curTurn == EX) then
            MakeMove()
        end
    end

    if eventCode == GUI_TIMER_EXPIRED then
        MakeMove()
    end


end
