-- Constants
local MOVE_FORCE = 5.0
local MAX_SPEED = 5.0
local JUMP_FORCE = 8.0
local CAMERA_OFFSET = { x = 0.0, y = 10.0, z = -7.0 } -- Behind and above
local CAMERA_SMOOTHING = 0.1
local ANIM_COOLDOWN = 0.5


-- Camera state
local cameraPos = { x = 0.0, y = 0.0, z = 0.0 }
local smoothedYaw = 0.0

--Animation
local AnimPath = "Assets\\GameTest\\TowerDefence\\Soldier\\fbx\\"
local CurrentAnim = "Idle.fbx"
local AnimTimer = ANIM_COOLDOWN

package.path = package.path .. ";./Assets/GameTest/TowerDefence/Soldier/?.lua"
local utils = require("utils")
local Keys = require("Keys")

-- State
local playerYRotation = 0

-- Called once when entity is created
function Create()
    SetPhysics(true)
    SetDynamic(true)
    ClampVelocity(MAX_SPEED)
    --Rotate(-90, 0, 0)
    SetPos(0, 10, 0)
    Scale(0.05, 0.05, 0.05)
    AddAnimation(AnimPath .. CurrentAnim)
    SetAngularFactor(0.0, 0.0, 0.0)   -- Lock Rotations
    SetLinearFactor(1.0, 1.0, 1.0)    -- Can Lock vertical movement

    -- Initialize camera position
    cameraPos.x = Pos:getX()
    cameraPos.y = Pos:getY() + CAMERA_OFFSET.y
    cameraPos.z = Pos:getZ() + CAMERA_OFFSET.z

    smoothedYaw = Rot:getY()
end

-- Called every frame
function Update()
    local dt = GetDeltaTime()

    local dirX = 0.0
    local dirZ = 0.0
    local isMoving = false

    if IsKeyDown(Keys.W) then -- W 
        dirZ = dirZ + 1.0
        CurrentAnim = "Jog Forward.fbx"
        isMoving = true
    end
    if IsKeyDown(Keys.S) then -- S
        dirZ = dirZ - 1.0 
        CurrentAnim = "Jog Backward.fbx"
        isMoving = true
    end 
    if IsKeyDown(Keys.A) then -- A
        dirX = dirX + 1.0 
        CurrentAnim = "Jog Strafe Left.fbx"
        isMoving = true
    end 
    if IsKeyDown(Keys.D) then -- D
        dirX = dirX - 1.0
        CurrentAnim = "Jog Strafe Right.fbx"
        isMoving = true
    end
    if IsKeyDown(Keys.Q) then
        playerYRotation = playerYRotation + 10
        Rotate(0, playerYRotation, 0)
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
    
    
    if IsKeyDown(Keys.E) then -- E
        local obj = SpawnObject("Crate", 1)
        obj:ChangeColor(1.0, 0.0, 0.0)
        obj:SetPos(cameraPos.x, cameraPos.y, cameraPos.z + 10)
        obj:SetPhysics(true)
        obj:SetDynamic(true)
        obj:ApplyRelativeForce(0, 0, 50)
    end
    

    if dirX ~= 0.0 or dirZ ~= 0.0 then
        local length = math.sqrt(dirX * dirX + dirZ * dirZ)
        dirX = dirX / length
        dirZ = dirZ / length

        ApplyRelativeForce(dirX * MOVE_FORCE, 0, dirZ * MOVE_FORCE)
    end

    -- Jumping
    if IsKeyDown(32) then
        local velocityY = GetVelocityY()
        if math.abs(velocityY) < 0.1 then
            ApplyRelativeForce(0.0, JUMP_FORCE * 10, 0.0)
        end
    end

    ClampVelocity(MAX_SPEED)

    -- Camera smoothing
    local px = Pos:getX()
    local py = Pos:getY()
    local pz = Pos:getZ()

    local yRot = Rot:getY()
    smoothedYaw = utils.LerpAngle(smoothedYaw, yRot, CAMERA_SMOOTHING)

    local radians = math.rad(smoothedYaw)

    local offsetX = CAMERA_OFFSET.x * math.sin(radians)
    local offsetZ = CAMERA_OFFSET.z * math.cos(radians)

    local targetX = px + offsetX - 2
    local targetY = py + CAMERA_OFFSET.y
    local targetZ = pz + offsetZ

    cameraPos.x = utils.Lerp(cameraPos.x, targetX, CAMERA_SMOOTHING)
    cameraPos.y = utils.Lerp(cameraPos.y, targetY, CAMERA_SMOOTHING)
    cameraPos.z = utils.Lerp(cameraPos.z, targetZ, CAMERA_SMOOTHING)

    SetCameraPos(cameraPos.x, cameraPos.y, cameraPos.z)
    SetCameraRot(-30.0, smoothedYaw, 0.0)
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
