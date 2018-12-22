--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  LuaSupport.lua
-- author:    Paul Schuytema
-- created:   April 11, 2005
-- descrip:   function files for blackjack demo
--=======================================

--blackjack constants
SPADE = 1
DIAMOND = 2
CLUB = 3
HEART = 4
ACE = 1
JACK = 11
QUEEN = 12
KING = 13
HUMAN = 1
DEALER = 2

HOUSE_STAND = 17

--gamestate values
HUMAN_TURN = 1
DEALER_TURN = 2
HUMAN_BUST = 3
DEALER_DONE = 4

THINKING = 1
DONE_THINKING = 2
BUST = 3


function CreateDeck()
    curDeckLocation = 1
    myDeck = {}
    cardNum = 1
    for suit = SPADE, HEART do
        for card = ACE, KING do
            myDeck[cardNum] = {}
            myDeck[cardNum].suit = suit
            myDeck[cardNum].card = card
            cardNum = cardNum + 1
        end
    end
    print("deck created")
end

function ShuffleDeck()
    shuffleValue = 1000
    for indx = 1, shuffleValue do
        card1 = math.random(1,52)
        card2 = math.random(1,52)
        hold = {}
        hold = myDeck[card1]
        myDeck[card1] = myDeck[card2]
        myDeck[card2] = hold
    end
    print("deck shuffled")
end

function DrawCard(suit, card, player, position)
    if player == DEALER then
        yValue = 100
        xValue = 50 + (115 * position)
        cardID = 100 + position
    else
        yValue = 300
        xValue = 50 + (115 * position)
        cardID = 120 + position
    end
    if suit == SPADE then
        theSuit = "s"
    end
    if suit == DIAMOND then
        theSuit = "d"
    end
    if suit == CLUB then
        theSuit = "c"
    end
    if suit == HEART then
        theSuit = "h"
    end
    cardName = string.format("%s%s%s%s%s", "crd_", theSuit, "_", tostring(card), ".bmp")
    --check if it's the dealer's first card (hidden)
    if (gameState == HUMAN_TURN) and (player == DEALER) and (position == 1) then
        cardName = "crdback.bmp"
    end
    CreateItem(cardID, "Sprite", cardName)
    SetItemPosition(cardID, xValue, yValue, 107, 150)
    print(string.format("%s%s", "card drawn: ", cardName))
end

function DrawHand(player)
    if table.getn(theHand[player]) > 0 then
        --there are cards in the hand
        for indx = 1, table.getn(theHand[player]) do
            DrawCard(myDeck[theHand[player][indx]].suit, myDeck[theHand[player][indx]].card, player, indx)
        end
    end
end

function InitHands()
    theHand = {}
    theHand[DEALER] = {}
    theHand[HUMAN] = {}
    print("player hands initialized")
end

function GetCard(player)
    if curDeckLocation > 52 then
        print("Out of cards! Start new game!")
    else
        table.insert(theHand[player], curDeckLocation)
        DrawHand(player)
        curDeckLocation = curDeckLocation + 1
    end
end

function InitialDeal()
    gameState = HUMAN_TURN
    for indx = 1,2 do
        GetCard(DEALER)
        GetCard(HUMAN)
    end
    ItemCommand(MESSAGE, "SetString", "tap H for hit, S for stand")
    print("initial deal")
end

function ClearOldCards()
    for indx = 1,7 do
        cardID = 100 + indx
        DeleteItem(cardID)
        cardID = 120 + indx
        DeleteItem(cardID)
    end
end


function CheckHandValue(player)
    myValue = 0
    numAces = 0
    for indx = 1, table.getn(theHand[player]) do
        curCardValue = myDeck[theHand[player][indx]].card
        if curCardValue > 10 then
            curCardValue = 10
        end
        if curCardValue == ACE then
            --we have an ace
            numAces = numAces + 1
        else
            myValue = myValue + curCardValue
        end
    end
    if numAces > 0 then
        --deal with one ace
        if numAces == 1 then
            if myValue + 11 > 21 then
                myValue = myValue + 1
            else
                myValue = myValue + 11
            end
        end
        --deal with two aces
        if numAces == 2 then
            if myValue + 12 > 21 then
                myValue = myValue + 2
            else
                myValue = myValue + 12
            end
        end
        --deal with three aces
        if numAces == 3 then
            if myValue + 13 > 21 then
                myValue = myValue + 3
            else
                myValue = myValue + 13
            end
        end
        --deal with four aces
        if numAces == 4 then
            if myValue + 14 > 21 then
                myValue = myValue + 4
            else
                myValue = myValue + 14
            end
        end
    end
    return myValue
end

function Think(player)
    result = THINKING
    if CheckHandValue(player) < HOUSE_STAND then
        GetCard(player)
    else
        result = DONE_THINKING
    end
    if CheckHandValue(player) > 21 then
        result = BUST
    end
    return result
end
