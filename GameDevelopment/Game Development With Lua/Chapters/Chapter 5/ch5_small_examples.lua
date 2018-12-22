-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- Published by Charles River Media


--small examples from Chapter 5
--simply uncomment out the example you'd like to run and
--then load the file using dofile


--myString = "1234"
--myNumber = tonumber(myString)
--print(myNumber + 2)


--myNumber = 1234
--myString = tostring(myNumber)
--print(type(myString))


--myString = "1234"
--print(string.len(myString))


--myString = "hello world"
--newString = string.sub(myString, 1, 5)
--print(newString)


--newString = string.sub(myString, -5, 10)
--print(newString)


--myString = "hello world"
--newString = string.sub(myString, -5)
--print(newString)


--string1 = "hello"
--string2 = "world"
--for index = 1, 3 do
    --string1 = string.format("%s%s", string1, string2)
--end
--print(string1)


--myName = "Fred"
--myStr = 16
--myString = string.format("%s%s%d%s", myName, "'s strength is ", myStr, ".")
--myString = string.format("%s's strength is %d.", myName, myStr)


--myHealth = 17.34556
--myString = string.format("%.2f%s", myHealth, "% of health remaining.")
--print(myHealth)
--print(myString)


--myString = "My name is John Smith."
--sStart, sEnd = string.find(myString, "John")
--print(sStart, sEnd)


--myString = "The price is $17.50."
--filter = "$%d%d.%d%d"
--print(string.sub(myString, string.find(myString, filter)))


--myString = "My name is John Smith. My phone is 555-3257."
--newString = string.gsub (myString, "%d", "*")
--print(newString)


--custData = "(309) 555-1234"
--custData = string.gsub(custData, "%(%d%d%d%)", "(781)")
--print(custData)


--myString = "happy, hello, home, hot, hudson"
--myString = string.gsub(myString, "h%a+", "An H word!", 2)
--print(myString)


--myString = "This is my rather long string."
--print(myString)
--counter = 1
--for myWord in string.gfind(myString, "%a+") do
    --print(string.format("Word #%d: %s", counter, myWord))
    --counter = counter + 1
--end


--myTable = {}
--for index = 1,100 do
    --myTable[index] = math.random(1,1000)
--end
--print(table.getn(myTable))
--table.sort(myTable)
--Sort(myTable)
--Sort(myTable, 1)

--table.insert(myTable, 25, "hello")

--print(table.remove(myTable, 25))

--for index = 1, table.getn(myTable) do
    --print(myTable[index])
--end

--print(table.getn(myTable))


--myData = {}
--myData.name = "Thardwick"
--myData.class = "Barbarian"
--myData.str = math.random(3,18)
--myData.dex = math.random(3,18)
--myData[1] = 17
--myData[2] = 34
--myData[3] = 24
--print(myData.dex)


--widget = {}
--widget.name = {}
--widget.cost = {}

--widget.name[1] = "Can opener"
--widget.cost[1] = "$12,75"

--widget.name[2] = "Scissors"
--widget.cost[2] = "$8.99"

--for key, value in pairs(widget) do
    --for subkey, subvalue in pairs(value) do
        --print(subkey, subvalue)
    --end
--end


--myNames = {"Fred", "Ethel", "Lucy", "Ricky", "Rockey", "Betsy", "Bill"}
--for index, value in pairs(myNames) do
    --print(index, value)
--end

--for index = 1, table.getn(myNames) do
    --print(index, myNames[index])
--end


--myData = {}
--myData.name = "Billy"
--myData.interest = "Wind surfing"
--myData.quote = "Cold out, eh?"
--myData.shoesize = 11

--for index, value in pairs(myData) do
    --print(index, value)
--end


--myFile = io.open("test_data.lua", "w")
--if myFile ~= nil then
    --myFile:write("-- Test lua file")
    --myFile:write(string.char (10))
    --myFile:write(string.char (10))
    --myFile:write(string.format("%s%s", "-- File created on: ", os.date()))
    --myFile:write(string.char (10))
    --myFile:write(string.char (10))
    --myFile:write("print(\"hello world!\")")
    --io.close(myFile)
--end






