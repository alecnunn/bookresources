-- Tables, Arrays, Iteration

--define the grades table
grades = {}
grades[1] = 90.5
grades[2] = 78.3
grades[3] = 85.8
grades[4] = 76.2
grades[5] = 68.1
grades[100] = 50.3
grades[200] = 100.0

--define the persons table
persons = {}
persons[1] = { name = "John", age = 30, weight = 180, IQ = 120 }
persons[2] = { name = "John", age = 18, weight = 150, IQ = 113 }
persons[3] = { name = "Sue", age = 19, weight = 110, IQ = 125 }
persons[4] = { name = "Dave", age = 20, weight = 160, IQ = 110 }
persons[5] = { name = "Laura", age = 24, weight = 100, IQ = 118 }
persons[6] = { name = "Don", age = 18, weight = 130, IQ = 122 }
persons[7] = { name = "Julie", age = 22, weight = 120, IQ = 105 }
persons[8] = { name = "Craig", age = 21, weight = 180, IQ = 112 }
persons[9] = { name = "Sarah", age = 20, weight = 115, IQ = 130 }


--print the title
print("TABLE EXAMPLE")
print()

--print the grades sequentially
print("SEQUENTIAL GRADES")
for n = 1, 5 do
	print( "Grade #" .. n .. " = " .. grades[n] )
end
print()

--print the grades conditionally
print("ALL GRADES (CONDITIONALLY)")
for n = 1, 1000 do
	if grades[n] ~= nil then
		print( "Grade #" .. n .. " = " .. grades[n] )
	end
end
print()


--print the grades dynamically
print("ALL GRADES (DYNAMICALLY)")
size = table.getn(grades)
for n = 1, size do
	if grades[n] ~= nil then
		print( "Grade #" .. n .. " = " .. grades[n] )
	end
end
print()



--print the persons
print("PERSONS")
size = table.getn(persons)
for n = 1, size do
	print( "Person #" .. n )
	print( "    Name   = " .. persons[n]["name"] )
	print( "    Age    = " .. persons[n].age )
	print( "    Weight = " .. persons[n]["weight"] )
	print( "    IQ     = " .. persons[n].IQ )
	print()
end

--[[ uncomment for demonstration

--]]
