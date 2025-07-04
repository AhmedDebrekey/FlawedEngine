local waypoints = {}
local currentTarget = nil
local currentIndex = 1
local moveSpeed = 2.0 -- units per second
local reachThreshold = 0.5 

function Create()
 Log("Zombie got Script")
end

function Update()
    dt = GetDeltaTime()
    Move(0.0, 1.0 * dt, 0.0)
end

function OnCollision(otherEntity)

end