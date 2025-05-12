local waypoints = {}
local currentTarget = nil
local currentIndex = 1
local moveSpeed = 2.0 -- units per second
local reachThreshold = 0.5
local rotationSpeed = 180.0 -- degrees per second


function Create()
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
        local desiredYaw = math.atan2(dx, dz) * (180.0 / math.pi)
        local currentYaw = Rot:getY()

        -- Smooth yaw interpolation
        local newYaw = lerp_angle(currentYaw, desiredYaw, dt * 5.0)

        Rotate(0.0, newYaw, 0.0)

        -- Move scaled by speed and delta time
        Move(dx * moveSpeed * dt, dy * moveSpeed * dt, dz * moveSpeed * dt)
    end

end

function ReachGoal()
    local base = GetEntity("Base")
    if base then
        base:Remove() -- Or call a function to damage it
    end
    Remove() -- Remove the zombie
end

function lerp_angle(a, b, t)
    local diff = (b - a + 180) % 360 - 180
    return a + diff * t
end

function OnCollision(otherEntity)

end