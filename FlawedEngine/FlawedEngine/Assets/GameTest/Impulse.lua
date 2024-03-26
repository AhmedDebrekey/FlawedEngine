function Create()

end

function Update()
    if IsKeyDown(76) then -- L
        ApplyForce(0, 1000, 0)
    end

    if IsKeyDown(75) then -- K
        Rotate(10, 40, 23)
    end

    if IsKeyDown(74) then -- J
        SetPhysics(true)
        SetDynamic(true)
    end

    if IsKeyDown(73) then -- H
        SetDynamic(false)
        SetPhysics(false)
    end

end

function OnCollision(otherEntity)

end