-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- Published by Charles River Media

-- Lua script example

-- Listing 5.2

-- ============ --
-- maze example --
-- ============ --

--=======================================
-- function:  ExploreMaze()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   nothing (process)
-- descrip:   starts the maze by running RoomA()
--=======================================
function ExploreMaze()
    RoomA()
end

--=======================================
-- function:  RoomA()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   function calls to next room
-- descrip:   example of using a function call as a return value
--=======================================
function RoomA()
    print("You are in a stone entry room")
    print("There is a single oaken door to the east.")
    local myInput = io.read()
    if myInput == "east" then
        return HallA()
    else
        print("You can't go that way!")
        return RoomA()
    end
end

--=======================================
-- function:  HallA()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   function calls to next room
-- descrip:   example of using a function call as a return value
--=======================================
function HallA()
    print("You are in a dark, wet corridor.")
    print("The hallway slopes down to the east.")
    local myInput = io.read()
    if myInput == "east" then
        return RoomB()
    elseif myInput == "west" then
        return RoomA()
    else
        print("You can't go that way!")
        return HallA()
    end
end

--=======================================
-- function:  RoomB()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   function calls to next room
-- descrip:   example of using a function call as a return value
--=======================================
function RoomB()
    print("You're standing in a limestone cavern.")
    print("A hallway slopes up to the west.")
    print("A damp tunnel leads to the south.")
    local myInput = io.read()
    if myInput == "west" then
        return RoomA()
    elseif myInput == "south" then
        return HallB()
    else
        print("You can't go that way!")
        return RoomB()
    end
end

--=======================================
-- function:  HallB()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   function calls to next room
-- descrip:   example of using a function call as a return value
--=======================================
function HallB()
    print("You're ankle-deep in a muddy cave tunnel.")
    print("You see light to the south.")
    local myInput = io.read()
    if myInput == "north" then
        return RoomB()
    elseif myInput == "south" then
        return RoomC()
    else
        print("You can't go that way!")
        return HallB()
    end
end

--=======================================
-- function:  RoomC()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   function calls to next room
-- descrip:   example of using a function call as a return value
--=======================================
function RoomC()
    print("You find yourself in a tiled vestibule, lit by a single torch.")
    print("A dark tunnerl slopes to the north.")
    print("A carved stone door is to the south.")
    print("An iron vent grating is on the east wall.")
    print("A modest service door leads to the west.")
    local myInput = io.read()
    if myInput == "north" then
        return HallB()
    elseif myInput == "south" then
        return RoomE()
    elseif myInput == "east" then
        return RoomF()
    elseif myInput == "west" then
        return RoomD()
    else
        print("You can't go that way!")
        return RoomC()
    end
end

--=======================================
-- function:  RoomD()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   function calls to next room
-- descrip:   example of using a function call as a return value
--=======================================
function RoomD()
    print("You find yourself in a modest servent's quarters.")
    print("In the room is a straw bed, a wash basin and a plate of old bread.")
    print("A service door leads to the east.")
    local myInput = io.read()
    if myInput == "east" then
        return RoomC()
    else
        print("You can't go that way!")
        return RoomD()
    end
end

--=======================================
-- function:  RoomE()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   function calls to next room
-- descrip:   example of using a function call as a return value
--=======================================
function RoomE()
    print("You are standing in a fancy Moorish bedroom.")
    print("A canopied bed is covered with silk and pillows.")
    print("On a carved table is a bowl of dates and a pitcher of water.")
    print("An ornate door leads north.")
    print("A low animal burrow leads to the east.")
    local myInput = io.read()
    if myInput == "east" then
        return HallC()
    elseif myInput == "north" then
        return RoomC()
    elseif myInput == "eat dates" then
        return Dates()
    else
        print("You can't go that way!")
        return RoomE()
    end
end

--=======================================
-- function:  HallC()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   function calls to next room
-- descrip:   example of using a function call as a return value
--=======================================
function HallC()
    print("You are crouched in a dank animal burrow.")
    print("The air smells of wet fur and oozone.")
    print("You see light to the east and to the north.")
    local myInput = io.read()
    if myInput == "east" then
        return RoomE()
    elseif myInput == "north" then
        return RoomF()
    else
        print("You can't go that way!")
        return HallC()
    end
end

--=======================================
-- function:  RoomF()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   function calls to next room
-- descrip:   example of using a function call as a return value
--=======================================
function RoomF()
    print("You are standing in a simple stone room with a sand floor.")
    print("In the center of the room is a small chest, open.")
    print("In the chest is a treasure of gold coins and ruby rings.")
    print("You are rich!")
end

--=======================================
-- function:  Dates()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   function calls to next room
-- descrip:   example of using a function call as a return value
--=======================================
function Dates()
    print("The world begins swirling around you...")
    print("...your vision blurs into a smear of color...")
    print("...and then darkness.")
end
