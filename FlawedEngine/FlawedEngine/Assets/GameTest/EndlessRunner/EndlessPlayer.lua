local rotationSpeed = 0.05
local targetYaw = 0

function Create()
    positionX = 0
    positionZ = -20
    SetPos(positionX, 6, positionZ)
    SetCameraPos(0, 16, positionZ - 10)
    SetPhysics(true)
    SetDynamic(false)

    Scale(0.1, 0.1, 0.1)
    
    SetCameraRot(-35, 0, 0)

    obs1 = LoadObject("Assets\\GameTest\\EndlessRunner\\PoliceCar_1396.obj", "PoliceCar1")
    obs1:AddScript("Assets\\GameTest\\EndlessRunner\\Obstacle.lua")

    obs2 = LoadObject("Assets\\GameTest\\EndlessRunner\\PoliceCar_1396.obj", "PoliceCar2")
    obs2:AddScript("Assets\\GameTest\\EndlessRunner\\Obstacle.lua")

    obs3 = LoadObject("Assets\\GameTest\\EndlessRunner\\PoliceCar_1396.obj", "PoliceCar3")
    obs3:AddScript("Assets\\GameTest\\EndlessRunner\\Obstacle.lua")

    obs4 = LoadObject("Assets\\GameTest\\EndlessRunner\\PoliceCar_1396.obj", "PoliceCar4")
    obs4:AddScript("Assets\\GameTest\\EndlessRunner\\Obstacle.lua")

    isDead = false
    
end

function Update()
    if not isDead then
        if IsKeyDown(76) then
            positionX = math.max(positionX - 10, -10) -- Decrease position, but clamp to min -10
            targetYaw = math.max(targetYaw - 30, -30)  -- Set target yaw
        end
        
        if IsKeyDown(74) then
            positionX = math.min(positionX + 10, 10) -- Increase position, but clamp to max 10
            targetYaw = math.min(targetYaw + 30, 30)  -- Set target yaw
        end

        -- Smoothly rotate camera towards target orientation

        --SetCameraPos(positionX, 15, positionZ - 10) -- Debug
        RotateCameraSmooth(targetYaw)
        MovePlayerSmooth(positionX)
    else
        x = Pos:getX()
        y = Pos:getY()
        z = Pos:getZ()
        SetCameraPos(x, y + 15, z - 10)
        SetCameraRot(-35, 0, 0)

        if IsKeyDown(75) then
            resetGame()
        end
    end
end

function resetGame()
    SetDynamic(false)
    SetPhysics(false)
    SetPhysics(true)
    targetYaw = 0
    positionX = 0
    SetPos(positionX, 6, positionZ)
    SetCameraPos(0, 16, positionZ - 10)
    isDead = false
end

function RotateCameraSmooth(targetYaw)
    local cameraYaw = CamYaw()  -- Get current camera orientation
    local yawDiff = targetYaw - cameraYaw
    
    -- Incrementally adjust camera orientation towards the target orientation
    local newYaw = cameraYaw + yawDiff * rotationSpeed
    
    -- Set the new camera orientation
    SetCameraRot(-35, newYaw, 0)
end

function MovePlayerSmooth(targetX)
    local currentX = Pos:getX()
    local xDiff = targetX - currentX
    local newX = currentX + xDiff * rotationSpeed

    SetPos(newX, 6, positionZ)
end

function OnCollision(otherEntity)
    isDead = true
    SetDynamic(true)
    ApplyForce(50, 100, 0)
end