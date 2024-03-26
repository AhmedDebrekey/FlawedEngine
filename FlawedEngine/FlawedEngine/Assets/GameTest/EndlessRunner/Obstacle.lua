speed = 0.5
function Create()
    resetPos()
    Scale(1.5, 1.5, 1.5)
    ChangeColor(math.random(0, 1), math.random(0, 1), math.random(0, 1))
    SetPhysics(true)
    SetDynamic(false)
end

function Update()
    Move(0, 0, -speed)
end

function OnCollision(otherEntity)
    if otherEntity == "Wall" then
        resetPos()
    end
end

function resetPos()
    local xValues = {-10, 0, 10,}
    local selectedX = xValues[math.random(1, #xValues)]

    SetPos(selectedX, 5, 50 + selectedX)

    -- Set the range limits
    local lower_limit = 0.5
    local upper_limit = 0.8

    -- Generate a random number within the specified range
    local random_number = math.random() * (upper_limit - lower_limit) + lower_limit
    speed = random_number

    ChangeColor(math.random(0, 1), math.random(0, 1), math.random(0, 1))
end