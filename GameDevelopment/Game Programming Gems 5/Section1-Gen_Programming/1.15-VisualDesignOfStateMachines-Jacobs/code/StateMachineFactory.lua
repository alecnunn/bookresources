--[[------------------------------------------------------------------------

    Used in conjunction with state machine diagrams created in UMLpad and 
    converted to Lua using the ussToState.lua script, this file will 
    provide simple state machines to other scripts which would like to 
    use them.

	$HeadURL: svn://localhost/trunk/gpg5/code/StateMachineFactory.lua $
	$Id: StateMachineFactory.lua 502 2004-08-27 03:17:26Z sjacobs $

]]

--[[---------------------------------------
The StateMachine table provides the main functions for operating the state 
machine.  It will be set as the index member of the meta table of all state 
machines created by the StateMachineFactory.
]]
local StateMachine = {}

--[[---------------------------------------
]]
function StateMachine:begin()
    assert(not self.currentState)
    self.currentState = self.machineData.states[ self.machineData.start ]
    assert(self.currentState)
end

--[[---------------------------------------
]]
function StateMachine:isDone()
    return not self.currentState
end

--[[---------------------------------------
]]
function StateMachine:update()

    local newState = false

    ------------------------------
    -- update the current state
    if self.currentState.OnUpdate then
        self.currentState.OnUpdate( self, self.stateData )
    end

    ------------------------------
    -- see if it is time to advance the 
    -- machine to a new state
    for _, link in pairs(self.currentState.links) do
        assert( link.advance )
        if link.advance( self, self.stateData ) then
           
            -- run any method that needs to be run when leaving a state
            if self.currentState.OnExit then
                self.currentState.OnExit( self, self.stateData )
            end

            self.currentState = self.machineData.states[ link.target ]

            if not self:isDone() then

                --create new state data
                self.stateData = {}

                -- run any method that needs to be run when entering a state
                if self.currentState.OnEnter then
                    self.currentState.OnEnter( self, self.stateData )
                end

            end
            
            newState = true

            ------------------------------
            -- stop iterating over the links to new states, since we traversed
            -- one.
            break
        end
    end
    
    return newState

end

--[[
]]
local StateMachine_meta =
{
    __index = StateMachine
}

--[[-----------------------------------------
    Returns state machine objects, based on the
    machines created with UMLpad.
]]
StateMachineFactory = {}

--[[-----------------------------------------
]]
function StateMachineFactory.create( machineName )
    local machineData = dofile( machineName )
    assert( machineData, "Failed to load state machine " .. tostring(machineName) )
    assert( type(machineData) == "table", "State machine data file did not return a table: " .. tostring(machineName) )
    
    local machine = 
    {
        currentState = nil,
        machineData = machineData,
    }

    machine.machine = machine
    
    setmetatable( machine, StateMachine_meta )
    
    return machine
end

