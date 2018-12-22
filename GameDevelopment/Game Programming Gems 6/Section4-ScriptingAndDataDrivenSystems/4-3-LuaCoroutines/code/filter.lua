-- Use of coroutines to implement filters

-- Example: external events are characters
-- ^c is converted to capital c, where c represents any character
-- Vogals are converted into two events: a slash and the vogal itself.

local last = false
local GetExternalEvent = function ()
    return io.read(1)
end

local ClassifyEvent = function (c)
    local vogal = {
        a = true,
        e = true,
        i = true,
        o = true,
        u = true,
    }
    if c == "^" then
        return "two-to-one"
    elseif vogal[c] then
        return "one-to-two"
    else
        return "one-to-one"
    end
end

local TranslateEvent = function (c)
    return c
end

local CombineEvent = function (c,d)
    return d:upper()
end

local SplitEvent = function (c)
    return "/",c
end

local EventFilter = function ()
    while true do
        local curr = GetExternalEvent()
        local kind = ClassifyEvent(curr)
        if kind == "one-to-one" then
            coroutine.yield(TranslateEvent(curr))
        elseif kind == "two-to-one" then
            local next = GetExternalEvent()
            coroutine.yield(CombineEvent(curr,next))
        elseif kind == "one-to-two" then
            local ev1, ev2 = SplitEvent(curr)
            coroutine.yield(ev1)
            coroutine.yield(ev2)
        else
            return nil
        end
    end
end

local GetNextEvent = coroutine.wrap(EventFilter)


repeat
    local e = GetNextEvent()
    print(e)
until e==nil

