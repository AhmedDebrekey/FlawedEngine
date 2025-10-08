-- Constants
local MOVE_FORCE = 5.0
local MAX_SPEED = 5.0
local JUMP_FORCE = 8.0
local CAMERA_OFFSET = { x = 0.0, y = 10.0, z = -7.0 } -- height + default distance (|z|)
local CAMERA_SMOOTHING = 0.1
local ANIM_COOLDOWN = 0.5

package.path = package.path .. ";./Assets/GameTest/TowerDefence/Soldier/?.lua"
local utils = require("utils")
local Keys  = require("Keys")

-- Animation
local AnimPath   = "Assets\\GameTest\\TowerDefence\\Soldier\\fbx\\"
local CurrentAnim = "Idle.fbx"
local AnimTimer   = ANIM_COOLDOWN

-- Player state
local playerYRotation = 0.0

-- Camera (orbit/spring-arm) state
local camYaw = 0.0
local camPitch = -20.0
local camDist = math.abs(CAMERA_OFFSET.z)

local CAM_MIN_PITCH = -60.0
local CAM_MAX_PITCH =  30.0
local CAM_MIN_DIST  = 2.5
local CAM_MAX_DIST  = 15.0

local CAM_YAW_SPEED   = 180.0    -- deg/sec with keys
local CAM_PITCH_SPEED = 120.0    -- deg/sec with keys
local CAM_SMOOTHING   = CAMERA_SMOOTHING

-- Smoothed camera state
local cameraPos = { x = 0.0, y = 0.0, z = 0.0 }
local smoothPos = { x = 0.0, y = 0.0, z = 0.0 }
local smoothYaw = 0.0
local smoothPitch = -20.0

-- Called once when entity is created
function Create()
    SetPhysics(true)
    SetDynamic(true)
    ClampVelocity(MAX_SPEED)
    SetPos(0, 10, 0)
    Scale(0.05, 0.05, 0.05)
    AddAnimation(AnimPath .. CurrentAnim)
    SetAngularFactor(0.0, 0.0, 0.0)   -- Lock Rotations
    SetLinearFactor(1.0, 1.0, 1.0)    -- Can Lock vertical movement

    -- Initialize from current transform
    playerYRotation = Rot:getY()
    camYaw = playerYRotation
    smoothYaw = camYaw
    smoothPitch = camPitch

    cameraPos.x = Pos:getX()
    cameraPos.y = Pos:getY() + CAMERA_OFFSET.y
    cameraPos.z = Pos:getZ() + CAMERA_OFFSET.z

    smoothPos.x, smoothPos.y, smoothPos.z = cameraPos.x, cameraPos.y, cameraPos.z
end

-- Called every frame
function Update()
    local dt = GetDeltaTime()

    -- =======================
    -- Camera Input (orbit)
    -- =======================
    -- Keys: Z/C yaw, X/V pitch, R/F zoom
    if IsKeyDown(Keys.C) then camYaw   = camYaw - CAM_YAW_SPEED * dt end
    if IsKeyDown(Keys.Z) then camYaw   = camYaw + CAM_YAW_SPEED * dt end
    if IsKeyDown(Keys.X) then camPitch = camPitch + CAM_PITCH_SPEED * dt end
    if IsKeyDown(Keys.V) then camPitch = camPitch - CAM_PITCH_SPEED * dt end
    if IsKeyDown(Keys.R) then camDist  = camDist  - 8.0 * dt end
    if IsKeyDown(Keys.F) then camDist  = camDist  + 8.0 * dt end

    -- Clamp pitch and distance
    if camPitch < CAM_MIN_PITCH then camPitch = CAM_MIN_PITCH end
    if camPitch > CAM_MAX_PITCH then camPitch = CAM_MAX_PITCH end
    if camDist  < CAM_MIN_DIST  then camDist  = CAM_MIN_DIST  end
    if camDist  > CAM_MAX_DIST  then camDist  = CAM_MAX_DIST  end

    -- =======================
    -- Movement & Animation
    -- =======================
    local dirX = 0.0
    local dirZ = 0.0
    local isMoving = false

    if IsKeyDown(Keys.W) then
        dirZ = dirZ + 1.0
        CurrentAnim = "Jog Forward.fbx"
        isMoving = true
    end
    if IsKeyDown(Keys.S) then
        dirZ = dirZ - 1.0
        CurrentAnim = "Jog Backward.fbx"
        isMoving = true
    end
    if IsKeyDown(Keys.A) then
        dirX = dirX + 1.0
        CurrentAnim = "Jog Strafe Left.fbx"
        isMoving = true
    end
    if IsKeyDown(Keys.D) then
        dirX = dirX - 1.0
        CurrentAnim = "Jog Strafe Right.fbx"
        isMoving = true
    end

    if isMoving then
        ChangeAnimation(AnimPath .. CurrentAnim)
        AnimTimer = ANIM_COOLDOWN
    else
        AnimTimer = AnimTimer - dt
        if AnimTimer <= 0.0 then
            ChangeAnimation(AnimPath .. "Idle.fbx")
            AnimTimer = ANIM_COOLDOWN
        end
    end

    -- Spawn a crate in front of the camera
    if IsKeyDown(Keys.E) then
        local obj = SpawnObject("Crate", 1)
        obj:ChangeColor(1.0, 0.0, 0.0)
        obj:SetPos(smoothPos.x, smoothPos.y + 5, smoothPos.z)
        obj:Rotate(0, playerYRotation, 0)
        obj:SetPhysics(true)
        obj:SetDynamic(true)
        obj:ApplyRelativeForce(0, 0, 50)
    end

    -- Normalize input and apply force in player local space
    if dirX ~= 0.0 or dirZ ~= 0.0 then
        local length = math.sqrt(dirX * dirX + dirZ * dirZ)
        dirX = dirX / length
        dirZ = dirZ / length

        -- Align player to camera yaw when moving (classic TPS)
        playerYRotation = camYaw
        Rotate(0, playerYRotation, 0)

        ApplyRelativeForce(dirX * MOVE_FORCE, 0, dirZ * MOVE_FORCE)
    end

    -- Jumping (simple check)
    if IsKeyDown(32) then
        local velocityY = GetVelocityY()
        if math.abs(velocityY) < 0.1 then
            ApplyRelativeForce(0.0, JUMP_FORCE * 10, 0.0)
        end
    end

    ClampVelocity(MAX_SPEED)

    -- =======================
    -- Third-person Spring-Arm Camera
    -- =======================
    local px, py, pz = Pos:getX(), Pos:getY(), Pos:getZ()

    -- Shoulder/torso height target
    local targetX = px
    local targetY = py + CAMERA_OFFSET.y
    local targetZ = pz

    -- Forward from yaw/pitch
    local yawRad   = math.rad(camYaw)
    local pitchRad = math.rad(camPitch)
    local fwdX =  math.cos(pitchRad) * math.sin(yawRad)
    local fwdY =  math.sin(pitchRad)
    local fwdZ =  math.cos(pitchRad) * math.cos(yawRad)

    -- Desired camera position behind the target
    local desiredX = targetX - fwdX * camDist
    local desiredY = targetY - fwdY * camDist
    local desiredZ = targetZ - fwdZ * camDist

    -- Optional: spring-arm collision (uncomment if Raycast is available)
    -- if Raycast then
    --     local hit, hx, hy, hz, t = Raycast(targetX, targetY, targetZ,
    --         desiredX - targetX, desiredY - targetY, desiredZ - targetZ, camDist)
    --     if hit then
    --         desiredX = hx - fwdX * 0.2
    --         desiredY = hy - fwdY * 0.2
    --         desiredZ = hz - fwdZ * 0.2
    --     end
    -- end

    -- Smooth camera
    smoothPos.x = utils.Lerp(smoothPos.x, desiredX, CAM_SMOOTHING)
    smoothPos.y = utils.Lerp(smoothPos.y, desiredY, CAM_SMOOTHING)
    smoothPos.z = utils.Lerp(smoothPos.z, desiredZ, CAM_SMOOTHING)
    smoothYaw   = utils.LerpAngle(smoothYaw, camYaw,   CAM_SMOOTHING)
    smoothPitch = utils.Lerp(     smoothPitch, camPitch, CAM_SMOOTHING)

    SetCameraPos(smoothPos.x, smoothPos.y, smoothPos.z)
    SetCameraRot(smoothPitch, smoothYaw, 0.0)
end

function GetVelocityY()
    -- Replace with engine call if GetVelocity().y is accessible
    local vy = 0.0
    local v = GetVelocity()
    return vy
end

function OnCollision(otherEntity)
    -- Collision logic placeholder
end
