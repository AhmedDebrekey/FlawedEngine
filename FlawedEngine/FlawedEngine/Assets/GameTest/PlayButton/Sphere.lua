local time = 0.0
local amplitude = 2.0     -- Maximum height offset
local speed = 2.0         -- Frequency of oscillation
local baseY = 0.0         -- Original Y position

function Create()
    baseY = Pos:getY()    -- Save starting Y position
    Log("Sphere jump (cos wave) script started")
end

function Update()
    local dt = GetDeltaTime()
    time = time + dt

    -- Calculate new Y position using cosine
    local y = baseY + math.cos(time * speed) * amplitude

    SetPos(0, y, 0)
end

function OnCollision(otherEntity)

end