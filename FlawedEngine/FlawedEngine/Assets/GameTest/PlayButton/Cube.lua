function Create()
    Log("Cube is Created")
end

speed = 45

function Update()
    dt = GetDeltaTime()

    newY = Rot:getY() + speed * dt

    Rotate(0, newY, 0)
end

function OnCollision(otherEntity)

end