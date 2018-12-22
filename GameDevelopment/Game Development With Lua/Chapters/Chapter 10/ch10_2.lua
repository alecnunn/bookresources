
function GetLines(fileName)
    indx = 0
    myLines = {}
    for line in io.lines(string.format("%s%s", "c:/lua_scripts/", fileName)) do
        indx = indx + 1
        myLines[indx] = line
    end
    return indx, myLines --returns number of lines and line table
end

function GetValues(myString)
    num = 0
    values = {}
    if myString ~= nil then
        while string.find(myString,",") ~= nil do
            i,j = string.find(myString,",")
            num = num + 1
            values[num] = string.sub(myString,1, j-1)
            myString = string.sub(myString, j+1, string.len(myString))
        end
        num = num + 1
        values[num] = myString
    end
    return num, values
end

function LoadCharacters()
    myCharacters = {}
    numLines, allLines = GetLines("data1.csv")
    --load labels (the first line)
    count, myLabels = GetValues(allLines[1])
    --ignore line 1, it's got the labels
    for indx = 2, numLines do
        count, charHold = GetValues(allLines[indx])
        myCharacters[indx-1] = {}
        for indx2 = 1, count do
            myCharacters[indx-1][indx2] = charHold[indx2]
        end
    end
    --now print them
    for indx = 1, 3 do
        for indx2 = 1, table.getn(myLabels) do
            print(myLabels[indx2], myCharacters[indx][indx2])
        end
    end
end

--script to test
--GetLines("data1.csv")
--count, myCharacters = GetValues(myLines[1])
--for indx = 1,count do
--    print(myCharacters[indx])
--end

