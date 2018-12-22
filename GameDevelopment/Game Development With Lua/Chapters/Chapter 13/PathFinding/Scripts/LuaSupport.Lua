--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  LuaSupport.lua
-- author:    Paul Schuytema
-- created:   March 17, 2005
-- descrip:   function files for AI pathfinding
--=======================================


--pathfinding

function DefineWorld_norm()
    world = {}
     world[1] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
     world[2] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
     world[3] = {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
     world[4] = {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}
     world[5] = {0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}
     world[6] = {0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0}
     world[7] = {0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}
     world[8] = {0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}
     world[9] = {0,0,1,1,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}
    world[10] = {0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}
    world[11] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[12] = {0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[15] = {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[16] = {0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[17] = {0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[18] = {0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[19] = {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[20] = {0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0}
    world[21] = {0,0,0,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0}
    world[22] = {0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0}
    world[23] = {0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0}
    world[24] = {0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0}
    world[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0}
    world[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[27] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[28] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[29] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
end

function DefineWorld()
    world = {}
     world[1] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
     world[2] = {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
     world[3] = {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
     world[4] = {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}
     world[5] = {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}
     world[6] = {1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0}
     world[7] = {0,0,0,0,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}
     world[8] = {0,0,0,0,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}
     world[9] = {0,0,1,1,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0}
    world[10] = {0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0}
    world[11] = {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0}
    world[12] = {0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0}
    world[13] = {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0}
    world[14] = {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0}
    world[15] = {0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0}
    world[16] = {0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0}
    world[17] = {0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,1,0}
    world[18] = {0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}
    world[19] = {0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}
    world[20] = {0,0,0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0}
    world[21] = {0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,0,0,0,0,0}
    world[22] = {0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0}
    world[23] = {0,0,0,1,0,0,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0}
    world[24] = {0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0}
    world[25] = {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,0}
    world[26] = {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0}
    world[27] = {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,0}
    world[28] = {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0}
    --don't use these
    world[29] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    world[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
end


--=======================================
-- function:  DrawCell(x,y)
-- author:    Paul Schuytema
-- created:   February 7, 2005
-- returns:   nothing (process)
-- descrip:   draws a cell in an 40x30 world
--=======================================
function DrawCell(x,y)
    --x is the down coordinate
    --y is the across coordinate
    CreateItem(masterCellID, "Sprite", "box1.bmp")
    SetItemPosition(masterCellID, (y * 20) + worldOffset, (x * 20) + worldOffset, 20, 20)
    masterCellID = masterCellID + 1
end

--=======================================
-- function:  DrawStart(x,y)
-- author:    Paul Schuytema
-- created:   March 17, 2005
-- returns:   nothing (process)
-- descrip:   draws a start cell in an 40x30 world
--=======================================
function DrawStart(x,y)
    --x is the down coordinate
    --y is the across coordinate
    CreateItem(masterCellID, "Sprite", "box_start.bmp")
    SetItemPosition(masterCellID, (y * 20) + worldOffset, (x * 20) + worldOffset, 20, 20)
    masterCellID = masterCellID + 1
end

--=======================================
-- function:  DrawTarget(x,y)
-- author:    Paul Schuytema
-- created:   March 17, 2005
-- returns:   nothing (process)
-- descrip:   draws a target cell in an 40x30 world
--=======================================
function DrawTarget(x,y)
    --x is the down coordinate
    --y is the across coordinate
    CreateItem(masterCellID, "Sprite", "box_target.bmp")
    SetItemPosition(masterCellID, (y * 20) + worldOffset, (x * 20) + worldOffset, 20, 20)
    masterCellID = masterCellID + 1
end

--=======================================
-- function:  DrawPath(x,y)
-- author:    Paul Schuytema
-- created:   March 17, 2005
-- returns:   nothing (process)
-- descrip:   draws the path in an 40x30 world
--=======================================
function DrawPath(myPath)
    --x is the down coordinate
    --y is the across coordinate
    if table.getn(myPath) > 0 then
        for indx = 1, table.getn(myPath) do
            CreateItem(masterCellID, "Sprite", "box_path.bmp")
            SetItemPosition(masterCellID, (myPath[indx].Y * 20) + worldOffset, (myPath[indx].X * 20) + worldOffset, 20, 20)
            masterCellID = masterCellID + 1
        end
    end
    --now draw the end as the target
    DrawTarget(myPath[table.getn(myPath)].X,myPath[table.getn(myPath)].Y)
end


--=======================================
-- function:  MakeWorld()
-- author:    Paul Schuytema
-- created:   February 7, 2005
-- returns:   nothing (process)
-- descrip:   fills the world with cells according to the world table
--=======================================
function MakeWorld()
    for x = 1,maxX do
        for y = 1,maxY do
            if world[x][y] == 1 then
                DrawCell(x,y)
            end
        end
    end
end


function InitWorld()
    DefineWorld()
    maxX = 30 --down
    maxY = 30 --across
    masterCellID = 5000
    startCellID = masterCellID
    worldOffset = 20
    MakeWorld()
    open = {}
    closed = {}
    path = {}
end

function DefineStart(myX, myY)
    startX = myX
    startY = myY
    --add start location to first item in open list
    open[1] = {}
    open[1].X = startX
    open[1].Y = startY
    open[1].Parent = 0
    open[1].G = 0
    open[1].H = 0
end

function DefineTarget(myX, myY)
    --define the target location
    targetX = myX
    targetY = myY
end

function FindH(curX,curY,tarX,tarY)
    --manhattan distance
    myH = 10*(math.abs(curX-tarX) + math.abs(curY-tarY))
    return myH
end

function FindG(nodeTable, node)
    parentG = closed[nodeTable[node].Parent].G
    if (nodeTable[node].X == closed[nodeTable[node].Parent].X) or (nodeTable[node].Y == closed[nodeTable[node].Parent].Y) then
        --NOT diagonal
        curValue = 10
    else
        curValue = 14
    end
    myG = parentG + curValue
    return myG
end

function FindLowestF(nodeTable)
    count = table.getn(nodeTable)
    minF = 1000000
    minFIndex = 0
    if count > 0 then
        for indx = 1,count do
            curF = nodeTable[indx].G + nodeTable[indx].H
            if curF <= minF then
                minF = curF
                minFIndex = indx
            end
        end
    end
    return minFIndex
end

function NewOpenEntry(newX, newY)
    myIndx = table.getn(open) + 1
    table.setn(open, myIndx)
    open[myIndx] = {}
    open[myIndx].X = newX
    open[myIndx].Y = newY
    open[myIndx].Parent = closedIndx
    open[myIndx].G = FindG(open, myIndx)
    open[myIndx].H = FindH(open[myIndx].X,open[myIndx].Y,targetX,targetY)
end

function AlreadyExists(xX, xY)
    location = -1
    count = table.getn(open)
    if count > 0 then
        for indx = 1,count do
            if (open[indx].X == xX) and (open[indx].Y == xY) then
                location = indx
            end
        end
    end
    return location
end

function ProcessNode(newX, newY)
    targetFound = false
    exists = AlreadyExists(newX, newY)
    if exists == -1 then
        --it's a new open node
        --is it the target????

        if (newX == targetX) and (newY == targetY) then
            --target is found
            targetFound = true
            print("target found")
        else
            --it's a new open node
            NewOpenEntry(newX, newY)
            print("new entry")
        end
    else
        --the node is already in the open table
        print("exists!")
        existingG = open[exists].G
        curValue = 0
        parentG = closed[closedIndx].G
        if (open[exists].X == closed[closedIndx].X) or (open[exists].Y == closed[closedIndx].Y) then
            --NOT diagonal
            curValue = 10
        else
            curValue = 14
        end
        newG = parentG + curValue
        if newG < existingG then
            --open[exists].Parent = openIndx
            open[exists].Parent = closedIndx
        end
    end
    return targetFound
end



--find lowest F on open list
--move it to closed list
--remove it from open list
--it's our current square

function Dump()
    myFile = io.open("a_dump.lua", "w")
    if myFile ~= nil then
        myFile:write("-- A* dump file")
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%s", "-- File created on: ", os.date()))
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        myFile:write("-- Open table")
        myFile:write(string.char (10))
        count = table.getn(open)
        if count > 0 then
            for indx = 1,count do
                myString = string.format("%s%d%s%d", "open[", indx, "].X:         ", open[indx].X)
                myFile:write(myString)
                myFile:write(string.char (10))
                myString = string.format("%s%d%s%d", "open[", indx, "].Y:         ", open[indx].Y)
                myFile:write(myString)
                myFile:write(string.char (10))
                myString = string.format("%s%d%s%d", "open[", indx, "].Parent:    ", open[indx].Parent)
                myFile:write(myString)
                myFile:write(string.char (10))
                myString = string.format("%s%d%s%d", "open[", indx, "].G:         ", open[indx].G)
                myFile:write(myString)
                myFile:write(string.char (10))
                myString = string.format("%s%d%s%d", "open[", indx, "].H:         ", open[indx].H)
                myFile:write(myString)
                myFile:write(string.char (10))
                myFile:write(string.char (10))
            end
        end
        myFile:write(string.char (10))
        myFile:write("-- Closed table")
        myFile:write(string.char (10))
        count = table.getn(closed)
        if count > 0 then
            for indx = 1,count do
                myString = string.format("%s%d%s%d", "closed[", indx, "].X:         ", closed[indx].X)
                myFile:write(myString)
                myFile:write(string.char (10))
                myString = string.format("%s%d%s%d", "closed[", indx, "].Y:         ", closed[indx].Y)
                myFile:write(myString)
                myFile:write(string.char (10))
                myString = string.format("%s%d%s%d", "closed[", indx, "].Parent:    ", closed[indx].Parent)
                myFile:write(myString)
                myFile:write(string.char (10))
                myString = string.format("%s%d%s%d", "closed[", indx, "].G:         ", closed[indx].G)
                myFile:write(myString)
                myFile:write(string.char (10))
                myString = string.format("%s%d%s%d", "closed[", indx, "].H:         ", closed[indx].H)
                myFile:write(myString)
                myFile:write(string.char (10))
                myFile:write(string.char (10))
            end
        end
        io.close(myFile)
    end

end

function ReverseTable(myTable)
    hold = {}
    endCount = table.getn(myTable)
    for indx = 1, table.getn(myTable) do
        hold[indx] = myTable[endCount]
        endCount = endCount -1
    end
    return hold
end


function BuildPath()
    --once the path is found, this will build the path table
    --load the first node, the target
    count = 1
    path[count] = {}
    path[count].X =  targetX
    path[count].Y =  targetY
    --load in the last node in the closed table
    count = count + 1
    path[count] = {}
    pathIndx = table.getn(closed)
    path[count].X =  closed[pathIndx].X
    path[count].Y =  closed[pathIndx].Y
    newPathIndx = closed[pathIndx].Parent
    --now walk through closed table
    while newPathIndx ~= 1 do
        count = count + 1
        path[count] = {}
        path[count].X =  closed[newPathIndx].X
        path[count].Y =  closed[newPathIndx].Y
        oldPathIndx = newPathIndx
        newPathIndx = closed[oldPathIndx].Parent
    end
    path = ReverseTable(path)
    --FinalDump()
end

function FinalDump()
    myFile = io.open("path_dump.lua", "w")
    if myFile ~= nil then
        myFile:write("-- A* path dump file")
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%s", "-- File created on: ", os.date()))
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%d", "StartX: ", startX))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%d", "StartY: ", startY))
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        myFile:write("-- Path table")
        myFile:write(string.char (10))
        count = table.getn(path)
        if count > 0 then
            for indx = 1,count do
                myString = string.format("%s%d%s%d", "path[", indx, "].X:         ", path[indx].X)
                myFile:write(myString)
                myFile:write(string.char (10))
                myString = string.format("%s%d%s%d", "path[", indx, "].Y:         ", path[indx].Y)
                myFile:write(myString)
                myFile:write(string.char (10))
                myFile:write(string.char (10))
            end
        end
        myFile:write(string.format("%s%d", "TargetX: ", targetX))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%d", "TargetY: ", targetY))
        myFile:write(string.char (10))
        io.close(myFile)
    end
end


function FindPath()

    targetFound = false

    openIndx = FindLowestF(open)
    closedIndx = table.getn(closed) + 1
    table.setn(closed, closedIndx)
    closed[closedIndx] = {}
    closed[closedIndx].X = open[openIndx].X
    closed[closedIndx].Y = open[openIndx].Y
    closed[closedIndx].Parent = open[openIndx].Parent
    closed[closedIndx].G = open[openIndx].G
    closed[closedIndx].H = open[openIndx].H

    --Dump()

    --now check out the squares around it
    curX = closed[closedIndx].X
    curY = closed[closedIndx].Y

    --square1
    if not targetFound then
        myX = curX - 1
        myY = curY - 1
        if (myX > 0) and (myY > 0) and (world[myX][myY] ~= 1) then
            --in bounds and in play
            --does it exist?
            ProcessNode(myX, myY)
        end
    end

    --square2
    if not targetFound then
        myX = curX - 1
        myY = curY
        if (myX > 0) and (myY > 0) and (world[myX][myY] ~= 1) then
            --in bounds and in play
            --does it exist?
            ProcessNode(myX, myY)
        end
    end

    --square3
    if not targetFound then
        myX = curX - 1
        myY = curY + 1
        if (myX > 0) and (myY > 0) and (world[myX][myY] ~= 1) then
            --in bounds and in play
            --does it exist?
            ProcessNode(myX, myY)
        end
    end

    --square4
    if not targetFound then
        myX = curX
        myY = curY - 1
        if (myX > 0) and (myY > 0) and (world[myX][myY] ~= 1) then
            --in bounds and in play
            --does it exist?
            ProcessNode(myX, myY)
        end
    end

    --square5
    if not targetFound then
        myX = curX
        myY = curY + 1
        if (myX > 0) and (myY > 0) and (world[myX][myY] ~= 1) then
            --in bounds and in play
            --does it exist?
            ProcessNode(myX, myY)
        end
    end

    --square6
    if not targetFound then
        myX = curX + 1
        myY = curY - 1
        if (myX > 0) and (myY > 0) and (world[myX][myY] ~= 1) then
            --in bounds and in play
            --does it exist?
            ProcessNode(myX, myY)
        end
    end

    --square7
    if not targetFound then
        myX = curX + 1
        myY = curY
        if (myX > 0) and (myY > 0) and (world[myX][myY] ~= 1) then
            --in bounds and in play
            --does it exist?
            ProcessNode(myX, myY)
        end
    end

    --square8
    if not targetFound then
        myX = curX + 1
        myY = curY + 1
        if (myX > 0) and (myY > 0) and (world[myX][myY] ~= 1) then
            --in bounds and in play
            --does it exist?
            ProcessNode(myX, myY)
        end
    end

    --Dump()

    table.remove(open, openIndx)


    return targetFound

end
