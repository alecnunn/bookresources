-- Use of coroutines to implement a simple scheduler

local scheduler = {
    tasks = {}
}

function scheduler:addtask (name, task)
    self.tasks[name] = coroutine.create(task)
end

function scheduler:loop ()
    repeat
        for name,co in pairs(self.tasks) do
            coroutine.resume(co)
            if coroutine.status(co) == "dead" then
                self.tasks[name] = nil
            end
        end
    until not next(self.tasks)
end

local odd = function ()
    for i = 1, 10, 2 do
        print(i)
        coroutine.yield()
    end
end

local even = function ()
    for i = 2, 10, 2 do
        print(i)
        coroutine.yield()
    end
end

scheduler:addtask("even",even)
scheduler:addtask("odd",odd)
scheduler:loop()
