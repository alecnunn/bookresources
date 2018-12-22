local N = 10
local ave = 0
local att = 0

for i = 0, N do
	local h = Hero.Create(i)
--[[
        h.myFactor = 2     -- stores an extra attribute
        h.GetEnergy =    -- redefine the GetEnergy method
             function (h) 
                return Hero.GetEnergy(h) * h.myFactor
             end
--]]

	local t0 = os.clock()
	for i=1,1000000 do
		h:SetEnergy(i)
		if h:GetEnergy()~=i then
			error("failed");
		end
	end
	local dt = os.clock()-t0

	local t0 = os.clock()
	for i=1,1000000 do
		h.myattrib = i
		if h.myattrib~=i then
			error("failed");
		end
	end
	local dtt = os.clock()-t0

	if i~=0 then
	 ave = ave + dt
	 att = att + dtt
	 print(h:GetName(),dt,dtt)
	end

end

print('access (average elapsed time):',ave/N)
print('attribute (average elapsed time):',att/N)
