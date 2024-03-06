function Create()
    print("Cube Contorller Created")
    ChangeColor(0.8, 0.3, 0.5)
    x = Pos.getX()
    y = Pos.getY()
    z = Pos.getZ()

    Movement = 0.03
    isMoving = false
    Force = 50
end

function Update()
    if IsKeyDown(73) then
        ApplyForce(0, Force, 0)
    end

    if IsKeyDown(76) then
        ApplyForce(-Force, 0, 0)
    end

    if IsKeyDown(74) then
        ApplyForce(Force, 0, 0)
    end
end