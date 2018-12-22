--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  LuaSupport.lua
-- author:    Paul Schuytema
-- created:   April 11, 2005
-- descrip:   function files for tic-tac-toe
--=======================================

--=======================================
-- function:  InitGame()
-- author:    Paul Schuytema
-- created:   April 11, 2005
-- returns:   nothing (process)
-- descrip:   rests the game world to start of game
--=======================================
function InitGame()
    --turn off graphics
    --first, the pieces
    for indx = 1,9 do
        EnableObject(EX + indx, 0, 0)
        EnableObject(OH + indx, 0, 0)
    end
    --the bars
    for indx = 1,3 do
        EnableObject(H_BAR + indx, 0, 0)
        EnableObject(V_BAR + indx, 0, 0)
    end
    EnableObject(D_BAR + 1, 0, 0)
    EnableObject(D_BAR + 2, 0, 0)
    --the text
    for indx = 1,5 do
        EnableObject(GUI_INGAME + 200 + (indx * 10), 0, 0)
    end

    --set up the game data
    myBoard = {0,0,0,0,0,0,0,0,0}
    theWinner = -1

    --set up for the first turn
    if math.random(1,100) > 49 then
        curTurn = EX
        ItemCommand(GUI_INGAME + 250, "SetString", "X's turn: left click to place")
    else
        ItemCommand(GUI_INGAME + 250, "SetString", "O's turn: AI thinking")
        curTurn = OH
    end
    EnableObject(GUI_INGAME + 250, 1, 1)

end

function GetBoardLocation(myX, myY)
    --set default response (not valid)
    myPos = -1
    --modify values to make it easier to comprehend
    myX = myX - 250
    myY = myY - 150
    --now check for the click area
    if (myX > 0) and (myX < 100) and (myY > 0) and (myY < 100) then
        myPos = 1
    end
    if (myX > 100) and (myX < 200) and (myY > 0) and (myY < 100) then
        myPos = 2
    end
    if (myX > 200) and (myX < 300) and (myY > 0) and (myY < 100) then
        myPos = 3
    end
    if (myX > 0) and (myX < 100) and (myY > 100) and (myY < 200) then
        myPos = 4
    end
    if (myX > 100) and (myX < 200) and (myY > 100) and (myY < 200) then
        myPos = 5
    end
    if (myX > 200) and (myX < 300) and (myY > 100) and (myY < 200) then
        myPos = 6
    end
    if (myX > 0) and (myX < 100) and (myY > 200) and (myY < 300) then
        myPos = 7
    end
    if (myX > 100) and (myX < 200) and (myY > 200) and (myY < 300) then
        myPos = 8
    end
    if (myX > 200) and (myX < 300) and (myY > 200) and (myY < 300) then
        myPos = 9
    end
    if myPos ~= -1 then
        --if the click is legal, see if the space is occupied
        if myBoard[myPos] ~= 0 then
            myPos = -1
        end
    end
    return myPos
end

function WinCheck()
    --set default for on-going
    theGame = -1
    theID = -1
    --first, check for cat's game
    openSpace = false
    for indx = 1,9 do
        if myBoard[indx] == 0 then
            openSpace = true
        end
    end
    if openSpace == false then
        --no open spaces, so cat's game
        theGame = 0
    end

        --now check for a win
        --row1
        if myBoard[1] == curTurn then
            if myBoard[2] == curTurn then
                if myBoard [3] == curTurn then
                    theGame = curTurn
                    theID = H_BAR + 1
                end
            end
        end
        --row2
        if myBoard[4] == curTurn then
            if myBoard[5] == curTurn then
                if myBoard [6] == curTurn then
                    theGame = curTurn
                    theID = H_BAR + 2
                end
            end
        end
        --row3
        if myBoard[7] == curTurn then
            if myBoard[8] == curTurn then
                if myBoard [9] == curTurn then
                    theGame = curTurn
                    theID = H_BAR + 3
                end
            end
        end
        --col1
        if myBoard[1] == curTurn then
            if myBoard[4] == curTurn then
                if myBoard [7] == curTurn then
                    theGame = curTurn
                    theID = V_BAR + 1
                end
            end
        end
        --col2
        if myBoard[2] == curTurn then
            if myBoard[5] == curTurn then
                if myBoard [8] == curTurn then
                    theGame = curTurn
                    theID = V_BAR + 2
                end
            end
        end
        --col3
        if myBoard[3] == curTurn then
            if myBoard[6] == curTurn then
                if myBoard [9] == curTurn then
                    theGame = curTurn
                    theID = V_BAR + 3
                end
            end
        end
        --diag1
        if myBoard[1] == curTurn then
            if myBoard[5] == curTurn then
                if myBoard [9] == curTurn then
                    theGame = curTurn
                    theID = D_BAR + 1
                end
            end
        end
        --diag2
        if myBoard[3] == curTurn then
            if myBoard[5] == curTurn then
                if myBoard [7] == curTurn then
                    theGame = curTurn
                    theID = D_BAR + 2
                end
            end
        end


    return theGame, theID
end

function MakeMove()
    if curTurn == EX then
        --X human player
        thePos = GetBoardLocation(GetMousePosition())
    else
        --O AI player
        thePos = GetMove(myBoard[1],myBoard[2],myBoard[3],myBoard[4],myBoard[5],myBoard[6],myBoard[7],myBoard[8],myBoard[9],OH)
    end
    if thePos ~= -1 then
        --turn on the space
        EnableObject(curTurn + thePos, 1, 1)
        --change text
        if curTurn == EX then
            ItemCommand(GUI_INGAME + 250, "SetString", "O's turn: AI thinking")
        else
            ItemCommand(GUI_INGAME + 250, "SetString", "X's turn: left click to place")
        end
        --now update the table
        myBoard[thePos] = curTurn
        --check for win
        theWinner = -1
        theWinner, slashID = WinCheck()
        if theWinner == -1 then
            --no winner
            --now swap turns
            if curTurn == OH then
                curTurn = EX
            else
                curTurn = OH
            end
        elseif theWinner == 0 then
            --cat's game
            EnableObject(GUI_INGAME + 210, 1, 1)
            EnableObject(GUI_INGAME + 240, 1, 1)
            EnableObject(GUI_INGAME + 250, 0, 0)
        else
            --we have a winner
            EnableObject(GUI_INGAME + 250, 0, 0)
            EnableObject(GUI_INGAME + 240, 1, 1)
            EnableObject(slashID, 1, 1)
            if theWinner == EX then
                EnableObject(GUI_INGAME + 220, 1, 1)
            else
                EnableObject(GUI_INGAME + 230, 1, 1)
            end
        end
    end
    if (curTurn == OH) and (theWinner == -1) then
        StartTimer(1.5)
    end
end

