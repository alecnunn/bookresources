-- Use of coroutines to implement iterators

local weapons = {"knife", "sword", "gun", "knife", "knife", "sword"}

local iterator = function (array, cb)
    for i = 1, #array do
        cb(array[i])
    end
end

local count = function (container, elem)
    local n = 0
    iterator(container, function (value) 
                            if value==elem then n = n+1 end 
                        end)
    return n
end

assert(count(weapons,"knife")==3)
assert(count(weapons,"sword")==2)
assert(count(weapons,"gun")==1)

--------------------------------------------------------------------

local elements_of = function (array)
    local i=0
    return function () i = i+1 return array[i] end
end

local count = function (container, elem)
    local n = 0
    local itr = elements_of(container)
    local value = itr()
    while value do
        if value == elem then
            n = n + 1
        end
        value = itr()
    end
    return n
end

assert(count(weapons,"knife")==3)
assert(count(weapons,"sword")==2)
assert(count(weapons,"gun")==1)

--------------------------------------------------------------------

local count = function (container, elem)
    local n = 0
    for value in elements_of(container) do
        if value == elem then
            n = n + 1
        end
    end
    return n
end

assert(count(weapons,"knife")==3)
assert(count(weapons,"sword")==2)
assert(count(weapons,"gun")==1)

--------------------------------------------------------------------
local Scene = function (self)
    self.kind = "Scene"
    return self
end

local LightSource = function (self)
    self.kind = "LightSource"
    return self
end

local Transform = function (self)
    self.kind = "Transform"
    return self
end

local Entity = function (self)
    self.kind = "Entity"
    return self
end

local Sphere = function (self)
    self.kind = "Sphere"
    return self
end

local Texture = function (self)
    self.kind = "Texture"
    return self
end


myscene = Scene {
    name = "my scene",
    LightSource {
        position = {0.0,5.0,0.0},
        cutoff = 60,
    },
    Transform {
        translation = {1.0, 0.0, 0.0},
        rotation = {45, 0.0,0.0,1.0},
        Entity {
            geometry = Sphere{radius=1.0},
            appearance = Texture{image="myimage.tif"},
        },
    }
}

local function traverse (graph)
    coroutine.yield(graph)
    for i=1,#graph do
        traverse (graph[i])
    end
    return nil
end

local nodes_of = function (graph)
    return coroutine.wrap(function () traverse(graph) end)
end

for n in nodes_of(myscene) do
    -- process node n
    print(n.kind)
end



