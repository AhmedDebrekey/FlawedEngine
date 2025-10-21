local waypoints = {}
local currentTarget = nil
local currentIndex = 1
local moveSpeed = 5.0 -- units per second
local reachThreshold = 0.5
local rotationSpeed = 180.0 -- degrees per second
local HitPoints = 2
local yawState = nil

function Create()
    Scale(2, 2, 2)
    SetPhysics(true)
    SetDynamic(false)
    SetAABBOffset(0.0, 5.0, 0.0)
    -- Load all waypoints in order
    local i = 1
    while true do
        local wp = GetEntity("Waypoint" .. i)
        if not wp then break end
        table.insert(waypoints, wp)
        i = i + 1
    end
    
    if #waypoints > 0 then
        currentTarget = waypoints[1]
    else
        Log("No waypoints found!")
    end
    yawState = Rot:getY()
end

function Update()
    if not currentTarget then return end
    dt = GetDeltaTime()
    -- Get current position
    local x = Pos:getX()
    local y = Pos:getY()
    local z = Pos:getZ()

    -- Get target position
    local tx = currentTarget:pgetX()
    local ty = currentTarget:pgetY()
    local tz = currentTarget:pgetZ()

    -- Direction vector
    local dx = tx - x
    local dy = ty - y
    local dz = tz - z

    local distSq = dx*dx + dy*dy + dz*dz

    if distSq < reachThreshold * reachThreshold then
        currentIndex = currentIndex + 1
        if currentIndex > #waypoints then
            ReachGoal()
            return
        end
        currentTarget = waypoints[currentIndex]
    else
        -- Normalize direction
        local len = math.sqrt(distSq)
        dx = dx / len
        dy = dy / len
        dz = dz / len
        -- Calculate desired yaw and current yaw
        local desiredYaw = math.deg(math.atan(dx, dz))
        yawState = MoveTowardsAngle(yawState, desiredYaw, dt * rotationSpeed)

        Rotate(0.0, yawState, 0.0)

        -- Move scaled by speed and delta time
        Move(dx * moveSpeed * dt, dy * moveSpeed * dt, dz * moveSpeed * dt)
    end
    
end

function MoveTowardsAngle(current, target, maxDelta)
    current = (current % 360 + 360) % 360
    target  = (target  % 360 + 360) % 360
    local delta = ((target - current + 540) % 360) - 180  -- [-180,180]
    if math.abs(delta) <= maxDelta then
        return target
    end
    return (current + (delta > 0 and maxDelta or -maxDelta) + 360) % 360
end

function ReachGoal()
    local base = GetEntity("Base")
    if base then
        base:Remove() -- Or call a function to damage it
    end
    Remove() -- Remove the zombie
end

function OnCollision(otherEntity)
    if otherEntity:find("Crate") then
        local crate = GetEntity(otherEntity)
        crate:Remove()
        if HitPoints == 1 then
            Remove()
        end
        HitPoints = HitPoints - 1
        Log("I got hit. HP: " .. HitPoints)
    end
end