function Create()
    print("Player Created!")
    SetPhysics(true)

    bullet = SpawnObject("Bullet", 1)
    bullet:AddScript("Assets\\GameTest\\TopDown\\Bullet.lua") -- adding scripts calls the create function
    bulletSpawned = false

    enemy = SpawnObject("Enemy", 0)
    enemy:Scale(1, 10, 1)
    enemy:ChangeColor(0, 0, 1)
    enemy:AddScript("Assets\\GameTest\\TopDown\\Enemy.lua")
end
local rotation = 0
function Update()

    if IsKeyDown(74) then
        rotation = rotation + 0.1
        Rotate(0, rotation, 0)
    end

    if IsKeyDown(76) then
        rotation = rotation - 0.1
        Rotate(0, rotation, 0)
    end

end

function OnCollision(otherEntity)

end

