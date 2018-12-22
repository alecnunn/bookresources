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







--200s Text items

CreateItem(GUI_INGAME + 200,"TextField")
SetItemPosition(GUI_INGAME + 200, 10, 10, 200, 20)
SetFont(GUI_INGAME + 200, "Arial", 24)
ItemCommand(GUI_INGAME + 200, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 200, "SetString", "{blackjack demo}")

CreateItem(GUI_INGAME + 202,"TextField")
SetItemPosition(GUI_INGAME + 202, 10, 45, 200, 20)
SetFont(GUI_INGAME + 202, "Arial", 24)
ItemCommand(GUI_INGAME + 202, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 202, "SetString", "Tap Enter to play a hand.")


CreateItem(GUI_INGAME + 205,"TextField")
SetItemPosition(GUI_INGAME + 205, 50, 150, 200, 20)
SetFont(GUI_INGAME + 205, "Arial", 22)
ItemCommand(GUI_INGAME + 205, "SetColor", 255,255,255,255)
ItemCommand(GUI_INGAME + 205, "SetString", "...thinking...")
EnableObject(GUI_INGAME + 205, 0, 0)

MESSAGE = GUI_INGAME + 210
CreateItem(MESSAGE,"TextField")
SetItemPosition(MESSAGE, 165, 500, 200, 20)
SetFont(MESSAGE, "Arial", 24)
ItemCommand(MESSAGE, "SetColor", 255,255,255,255)




--300s Buttons


--400s Check Boxes and Radio buttons


--500s List objects


--600s Special objects (bar, chart, sprite ring, etc.)



--Initial run sets the event handler
SetEventHandler("InGameEvent")


--Event handler
function InGameEvent(id, eventCode)

    if eventCode == GUI_ENTER_INTERFACE then
    end

    if eventCode == GUI_KEY_PRESS then
        if id == 13 then --enter key
            EnableObject(GUI_INGAME + 202, 0, 0)
            ClearOldCards()
            CreateDeck()
            ShuffleDeck()
            InitHands()
            InitialDeal()
        end

        if id == 104 then -- h key
            if gameState == HUMAN_TURN then
                GetCard(HUMAN)
                if CheckHandValue(HUMAN) > 21 then
                    ItemCommand(MESSAGE, "SetString", "BUST!!")
                    gameState = HUMAN_BUST
                end
            end
        end

        if id == 115 then -- s key
            if gameState == HUMAN_TURN then
                gameState = DEALER_TURN
                EnableObject(GUI_INGAME + 205, 1, 1)
                StartTimer(1.5)
            end
        end

    end

    if eventCode == GUI_MOUSE_BUTTON_DOWN then
    end

    if eventCode == GUI_MOUSE_BUTTON_UP then
    end

    if eventCode == GUI_TIMER_EXPIRED then
        progress = Think(DEALER)
        if progress == BUST then
            EnableObject(GUI_INGAME + 205, 0, 0)
            gameState = DEALER_DONE
            DrawHand(DEALER)
            ItemCommand(MESSAGE, "SetString", "Dealer busts... YOU WIN!!")
        elseif progress == THINKING then
            StartTimer(1.5)
        else
            --dealer is done...process results
            EnableObject(GUI_INGAME + 205, 0, 0)
            gameState = DEALER_DONE
            DrawHand(DEALER)
            if CheckHandValue(HUMAN) > CheckHandValue(DEALER) then
                theScore = tostring(CheckHandValue(HUMAN))
                ItemCommand(MESSAGE, "SetString", string.format("%s%s%s","You win the hand with a ", theScore, "!"))
            else
                theScore = tostring(CheckHandValue(DEALER))
                ItemCommand(MESSAGE, "SetString", string.format("%s%s%s","Dealer takes the hand with a score of ", theScore, "."))
            end
            EnableObject(GUI_INGAME + 202, 1, 1)
        end
    end


end
