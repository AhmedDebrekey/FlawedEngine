-- Controller.lua (refactored to use SpringArm) + spawn cooldown

-- ======================
-- Constants
-- ======================
local MOVE_FORCE       = 5.0
local MAX_SPEED        = 5.0
local JUMP_FORCE       = 8.0
local CAMERA_OFFSET    = { x = 0.0, y = 10.0, z = -7.0 } -- height + default distance (|z|)
local CAMERA_SMOOTHING = 0.1
local ANIM_COOLDOWN    = 0.5
local SPAWN_COOLDOWN   = 0.75 -- seconds between crate spawns

package.path = package.path .. ";./Assets/GameTest/TowerDefence/Soldier/?.lua"

local utils     = require("utils")
local Keys      = require("Keys")
local SpringArm = require("SpringArm")

local CrateCount = 0

-- ======================
-- Animation
-- ======================
local AnimPath    = "Assets\\GameTest\\TowerDefence\\Soldier\\fbx\\"
local CurrentAnim = "Idle.fbx"
local AnimTimer   = ANIM_COOLDOWN

-- ======================
-- Player state
-- ======================
local playerYRotation = 0.0

-- Spawn state
local spawnTimer = 0.0
local spawnHeld  = false

-- ======================
-- Camera (via SpringArm)
-- ======================
local CAM_YAW_SPEED   = 180.0    -- deg/sec with keys

-- Spring arm instance (owns yaw/pitch/distance & vertical offset)
local arm = SpringArm.new({
    offsetY  = CAMERA_OFFSET.y,
    distance = math.abs(CAMERA_OFFSET.z),
    pitch    = -20.0,
    yaw      = 0.0
})

-- Smoothed camera state (we'll smooth the arm's solved camera pose)
local smoothPos = { x = 0.0, y = 0.0, z = 0.0 }
local smoothYaw = 0.0

-- ======================
-- Lifecycle
-- ======================
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

    arm.yaw = playerYRotation    -- start camera aligned to player
    smoothYaw = arm.yaw

    -- Seed smooth position with the initial computed camera
    local px, py, pz = Pos:getX(), Pos:getY(), Pos:getZ()
    local cx, cy, cz = arm:compute(px, py, pz)
    smoothPos.x, smoothPos.y, smoothPos.z = cx, cy, cz

    SetCameraPos(smoothPos.x, smoothPos.y, smoothPos.z)
    SetCameraRot(arm.pitch, smoothYaw, 0.0)
end

-- ======================
-- Frame Update
-- ======================
function Update()
    local dt = GetDeltaTime()

    -- Tick down spawn cooldown
    spawnTimer = math.max(0.0, spawnTimer - dt)

    -- =======================
    -- Camera Input (orbit)
    -- Keys: Z/C yaw, X/V pitch, R/F zoom
    -- =======================
    if IsKeyDown(Keys.C) then arm:addYaw(-CAM_YAW_SPEED * dt) end
    if IsKeyDown(Keys.Z) then arm:addYaw( CAM_YAW_SPEED * dt) end

    -- =======================
    -- Movement & Animation
    -- =======================
    local dirX, dirZ = 0.0, 0.0
    local isMoving = false
    local animToPlay = nil

    if IsKeyDown(Keys.W) then
        dirZ = dirZ + 1.0
        animToPlay = "Jog Forward.fbx"
        isMoving = true
    end
    if IsKeyDown(Keys.S) then
        dirZ = dirZ - 1.0
        animToPlay = "Jog Backward.fbx"
        isMoving = true
    end
    if IsKeyDown(Keys.A) then
        dirX = dirX + 1.0
        animToPlay = "Jog Strafe Left.fbx"
        isMoving = true
    end
    if IsKeyDown(Keys.D) then
        dirX = dirX - 1.0
        animToPlay = "Jog Strafe Right.fbx"
        isMoving = true
    end
    if IsKeyDown(Keys.T) then
        local px, py, pz = Pos:getX(), Pos:getY(), Pos:getZ()
        hitObject = Raycast(px, py, pz, px, py - 1, pz)
        hitObject:ChangeColor(math.random(0, 1), math.random(0, 1), math.random(0, 1))
        Log(hitObject:GetName())
    end

    if isMoving then
        if animToPlay and animToPlay ~= CurrentAnim then
            CurrentAnim = animToPlay
            ChangeAnimation(AnimPath .. CurrentAnim)
        end
        AnimTimer = ANIM_COOLDOWN
    else
        AnimTimer = AnimTimer - dt
        if AnimTimer <= 0.0 and CurrentAnim ~= "Idle.fbx" then
            CurrentAnim = "Idle.fbx"
            ChangeAnimation(AnimPath .. "Idle.fbx")
            AnimTimer = ANIM_COOLDOWN
        end
    end

    -- =======================
    -- Spawn a crate (cooldown + edge detection)
    -- =======================
    if IsKeyDown(Keys.E) then
        if not spawnHeld and spawnTimer <= 0.0 then
            local crateName = "Crate_" .. CrateCount
            CrateCount = CrateCount + 1
            local obj = SpawnObject(crateName, 0)
            if obj then
                obj:SetPos(smoothPos.x, smoothPos.y + 5, smoothPos.z)
                obj:Rotate(0, smoothYaw, 0)
                obj:AddScript("Assets\\GameTest\\TowerDefence\\Soldier\\Crate.lua")
            end
            spawnTimer = SPAWN_COOLDOWN
            spawnHeld  = true
        end
    else
        -- key released -> arm next spawn after cooldown
        spawnHeld = false
    end

    -- Normalize input and apply force in player local space
    if dirX ~= 0.0 or dirZ ~= 0.0 then
        local length = math.sqrt(dirX * dirX + dirZ * dirZ)
        dirX, dirZ = dirX / length, dirZ / length

        -- Align player to camera yaw when moving (classic TPS)
        playerYRotation = arm.yaw
        Rotate(0, playerYRotation, 0)

        ApplyRelativeForce(dirX * MOVE_FORCE, 0, dirZ * MOVE_FORCE)
    end

    -- Jumping (simple grounded-ish check using vertical velocity)
    if IsKeyDown(32) then
        ApplyRelativeForce(0.0, JUMP_FORCE * 10, 0.0)
    end

    ClampVelocity(MAX_SPEED)

    -- =======================
    -- Third-person Camera (SpringArm + smoothing)
    -- =======================
    local px, py, pz = Pos:getX(), Pos:getY(), Pos:getZ()
    local cx, cy, cz, rx, ry, rz = arm:compute(px, py, pz)

    -- Smooth camera pos & rot
    smoothPos.x = utils.Lerp(smoothPos.x, cx, CAMERA_SMOOTHING)
    smoothPos.y = utils.Lerp(smoothPos.y, cy, CAMERA_SMOOTHING)
    smoothPos.z = utils.Lerp(smoothPos.z, cz, CAMERA_SMOOTHING)
    smoothYaw   = utils.LerpAngle(smoothYaw, ry, CAMERA_SMOOTHING)

    SetCameraPos(smoothPos.x, smoothPos.y, smoothPos.z)
    SetCameraRot(arm.pitch, smoothYaw, 0.0)
end

-- ======================
-- Helpers
-- ======================
function GetVelocityY()
    -- Replace with engine call if GetVelocity().y is accessible
    if GetVelocity then
        local v = GetVelocity()
        if v and v.getY then
            return v:getY()
        end
    end
    return 0.0
end

function OnCollision(otherEntity)
    -- Collision logic placeholder
end
