-- This program calculates the angle between two points

function round(num, places)
  local mult = 10^(places or 0)
  return math.floor(num * mult + 0.5) / mult
end

function target_angle(x1,y1,x2,y2)
	deltaX = x2-x1
	deltaY = y2-y1
	return math.atan2( deltaY, deltaX )
end

x1 = 100
y1 = 100
x2 = 900
y2 = 600

print("Point 1 = " .. x1 .. "," .. y1 )
print("Point 2= " .. x2 .. "," .. y2 )

rangle = target_angle(x1,y1,x2,y2)
rangle = round(rangle,4)
dangle = math.deg( target_angle(x1,y1,x2,y2) )
dangle = round(dangle,4)


print("Target Angle in radians = " .. rangle )
print("Target Angle in degrees = " .. dangle )

