--[[
	ANGULAR VELOCITY

]]--

function round(num, places)
  local mult = 10^(places or 0)
  return math.floor(num * mult + 0.5) / mult
end

function VelocityX(angle)
	return math.cos( (angle-90) * math.pi/180)
end

function VelocityY(angle)
	return math.sin( (angle-90) * math.pi/180)
end


ang = 120
vx = round(VelocityX(ang),2)
vy = round(VelocityY(ang),2)
print("Velocity(" .. ang .. ") = " .. vx .. "," .. vy)

