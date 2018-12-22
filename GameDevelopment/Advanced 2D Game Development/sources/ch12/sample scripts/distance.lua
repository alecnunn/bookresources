-- This program calculates the distance between two points



function distance(x1,y1,x2,y2)
	return math.sqrt( (x2-x1)^2 + (y2-y1)^2 )
end

x1 = 100
y1 = 150
x2 = 780
y2 = 620

print("Point 1: " .. x1 .. "," .. y1)
print("Point 2: " .. x2 .. "," .. y2)
print("Distance = " .. distance(x1,y1,x2,y2) )

