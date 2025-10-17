function Create()
    ChangeColor(math.random(0, 1), math.random(0, 1), math.random(0, 1))
    SetPhysics(true)
    SetDynamic(true)
    ApplyRelativeForce(0, 0, 25)
end

function Update()

end

function OnCollision(otherEntity)
    if otherEntity == "Cube(0)" then
        Remove()
    elseif otherEntity:find("Crate") then
        Remove()
    elseif otherEntity:find("Zombie") then
        Log("Zombie Got hit [Crate.lua]")
    end
    
end