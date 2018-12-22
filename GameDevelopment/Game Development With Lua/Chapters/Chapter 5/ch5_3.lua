-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- Published by Charles River Media

-- Lua script example

-- Listing 5.3


-- min/max example --

-- first, build a table of 10 elements

-- we can see that the min is 1 and the max is 89

myTable = {7,1,13,6,89,4,56,2,54,6}

--=======================================
-- function:  GetMin(theTable)
-- author:    Paul Schuytema
-- created:   February 7, 2005
-- returns:   the minimum entry in a table
-- descrip:   uses ipairs to build a string that loads the value into the math.min function
--=======================================
function GetMin(theTable)
    myString = "myValue = math.min("
    for index,value in ipairs(theTable) do
        myString = string.format("%s%d%s", myString, value, ",")
    end
    --remove final comma
    myString = string.sub (myString, 1, string.len(myString) - 1)
    myString = string.format("%s%s", myString, ")")
    loadstring(myString)() --run the chunk
    print(myString) -- see the string
    print(myValue) --see the result
    return myValue
end

--=======================================
-- function:  GetMax(theTable)
-- author:    Paul Schuytema
-- created:   February 7, 2005
-- returns:   the maximum entry in a table
-- descrip:   uses ipairs to build a string that loads the value into the math.max function
--=======================================
function GetMax(theTable)
    myString = "myValue = math.max("
    for index,value in ipairs(theTable) do
        myString = string.format("%s%d%s", myString, value, ",")
    end
    --remove final comma
    myString = string.sub (myString, 1, string.len(myString) - 1)
    myString = string.format("%s%s", myString, ")")
    loadstring(myString)() --run the chunk
    print(myString) -- see the string
    print(myValue) --see the result
    return myValue
end

--=======================================
-- function:  Sort(theTable, direction)
-- author:    Paul Schuytema
-- created:   February 7, 2005
-- returns:   nothing (process)
-- descrip:   sorts a table up or down
--=======================================
function Sort(theTable, direction)
    if direction ~= 1 then
        table.sort(theTable)
    else
        function Reverse(a, b)
            if a < b then
                return false
            else
                return true
            end
        end
        table.sort(theTable, Reverse)
    end
end
