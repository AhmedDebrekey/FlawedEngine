-- Constants
local MOVE_FORCE = 5.0
local UP_FORCE = 0.001
local MAX_SPEED = 5.0  -- Max speed for clamping

-- Called once when entity is created
function Create()
    SetPhysics(true)
    SetDynamic(true)
    SetAngularFactor(0.0, 1.0, 0.0)   -- Lock pitch and roll; allow yaw (Y-axis rotation)
    SetLinearFactor(1.0, 0.0, 1.0)    -- Lock vertical movement (Y-axis)
    ClampVelocity(MAX_SPEED)         -- Set initial max speed
    Log("Hello World!")
end

-- Called every frame
function Update()
    local dirX = 0.0
    local dirZ = 0.0

    if IsKeyDown(87) then dirZ = dirZ + 1.0 end -- W
    if IsKeyDown(83) then dirZ = dirZ - 1.0 end -- S
    if IsKeyDown(65) then dirX = dirX + 1.0 end -- A (inverted for local forward)
    if IsKeyDown(68) then dirX = dirX - 1.0 end -- D

    if dirX ~= 0.0 or dirZ ~= 0.0 then
        local length = math.sqrt(dirX * dirX + dirZ * dirZ)
        dirX = dirX / length
        dirZ = dirZ / length

        ApplyRelativeForce(dirX * MOVE_FORCE, UP_FORCE, dirZ * MOVE_FORCE)
    end

    -- Clamp velocity every frame to prevent infinite acceleration
    ClampVelocity(MAX_SPEED)
end

-- Optional: For debugging
function OnCollision(otherEntity)

end
