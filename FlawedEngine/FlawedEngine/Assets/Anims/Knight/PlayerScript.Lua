function Create()
    print("Knight Created")
    ChangeColor(0, 0, 0)
    x = Pos.getX()
    y = Pos.getY()
    z = Pos.getZ()

    print(x, y, z)
    SetCameraPos(x, y + 15, z - 15)

    Movement = 0.1
    isMoving = false
end

function Update()
    local key1, key2 = nil, nil
    local moveX, moveZ = 0, 0

    if IsKeyDown(73) then
        key1 = "I"
        moveZ = Movement
    end

    if IsKeyDown(76) then
        key2 = "L"
        moveX = -Movement
    end

    if IsKeyDown(74) then
        key2 = "J"
        moveX = Movement
    end

    if IsKeyDown(75) then
        key1 = "K"
        moveZ = -0.02
    end

    if IsKeyDown(32) then
        key1 = "Space"
        moveZ = Movement
    end

    if IsKeyDown(90) then
        SpawnObject("Object", 0)
    end

    local animationPath = "Assets\\Anims\\Knight\\"
    local animationName = "Idle.dae"

    local animations = {
        ["I"] = "Jog Forward.dae",
        ["L"] = "Jog Strafe Right.dae",
        ["J"] = "Jog Strafe Left.dae",
        ["K"] = "Crouch Walk Back.dae",
        ["Space"] = "Jump.dae"
    }

    -- Table to store valid key combinations and their corresponding animations
    local combinationAnimations = {
        ["IL"] = "Jog Forward Diagonal.dae",
        ["IJ"] = "Jog Forward Diagonal Left.dae",
        ["KL"] = "Jog Backward Diagonal.dae",
        ["KJ"] = "Jog Backward Diagonal Left.dae"
    }

    local animation = nil

    -- Check for combinations of keys and set the appropriate animation
    if key1 and key2 then
        local combinationKey = key1 .. key2
        animation = combinationAnimations[combinationKey]
    else
        -- If no combination is found, check for individual key animations
        animation = animations[key1] or animations[key2]
    end

    if animation then
        animationName = animationPath .. animation
	else
    	animationName = animationPath .. animationName
    end

    ChangeAnimation(animationName)
	Move(moveX, 0, moveZ)
    MoveCamera(-moveX, 0, moveZ)
end