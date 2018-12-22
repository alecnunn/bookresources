-- Use of coroutines to implement cooperative multitasking


-- Exported Dispatcher "class"
Dispatcher = { }
Dispatcher.__index = Dispatcher

-- Create new dispatcher object
function Dispatcher.create ()
    local obj = {
        reftime = os.time(), -- set initial time
        tasks = {}           -- list of tasks
    }
    setmetatable(obj,Dispatcher)
    return obj
end

function Dispatcher:addtask (name, task)
    local t = { 
        name = name,
        status = "awake",
        co = coroutine.create(task),
        wakeuptime = 0,
        sender = nil,
        message = nil,
    }
    self.tasks[name] = t
    self.active = t
    coroutine.resume(t.co,self)
end

function Dispatcher:loop ()
    repeat
        self.currtime = os.difftime(os.time(),self.reftime)
        for name,task in pairs(self.tasks) do
            if task.status == "awake" then
                self:execute(task)
            elseif task.status == "sleeping" then
                if task.wakeuptime <= self.currtime then
                    task.status = "awake"
                    self:execute(task)
                end
            end
            task.sender = nil
            task.message = nil
        end
    until not next(self.tasks)
end

function Dispatcher:execute (task)
    self.active = task
    local ok, errmsg =
        coroutine.resume(task.co,task.sender,task.message)
    if not ok then
        error (errmsg)
    end
    if coroutine.status(task.co) == "dead" then
        self.tasks[task.name] = nil
    end
end

function Dispatcher:transfer (receiver, message)
    if receiver and self.tasks[receiver] then
        local task = self.active
        self.tasks[receiver].sender = task.name
        self.tasks[receiver].message = message
        if self.tasks[receiver].status == "waiting" then
            self.tasks[receiver].status = "awake"
        end
    end
    return coroutine.yield() 
end

function Dispatcher:sleep (dtime)
    local task = self.active
    task.wakeuptime = self.currtime + dtime
    task.status = "sleeping"
    return coroutine.yield()
end

function Dispatcher:wait ()
    local task = self.active
    task.status = "waiting"
    return coroutine.yield()
end

----------------------------------------------------------------
-- Simple dialogue

local character = function (dispatcher)
    local sender, message = dispatcher:wait()
    local reply = {
        hi = "Hi",
        who = "I am a scripter",
        advice = "Try Lua",
    }
    while message ~= "bye" do
        if message=="sleep" then
            io.write("(Zzz)\n")
            sender, message = dispatcher:sleep(10)
        else 
            local answer = reply[message] or "Sorry?"
            sender,message = dispatcher:transfer(sender,answer)
        end
    end
end


local player = function (dispatcher)
    local sender, message = dispatcher:transfer()
    while true do
        if sender then
            io.write(sender,": ",message,"\n")
        end
        io.write("Player: ")
        local line = io.read()
        if line == "pause" then
            sender,message = dispatcher:sleep(5)
        else
            sender, message = dispatcher:transfer("Character",line)
        end
        if line == "bye" then
            return
        end
    end
end

---------------------------------------------------------

local d = Dispatcher.create()
d:addtask("Character",character)
d:addtask("Player",player)
d:loop()

