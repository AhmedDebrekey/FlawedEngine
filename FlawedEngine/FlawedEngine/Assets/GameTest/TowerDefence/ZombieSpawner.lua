function Create()
    Log("Zombie Spawner Created")
end

local spawnTimer = 0.0
local spawnInterval = 5.0
local zombieCount = 0

function Update()
    local dt = GetDeltaTime()
    spawnTimer = spawnTimer + dt

    if spawnTimer >= spawnInterval then
        spawnTimer = 0.0
        zombieCount = zombieCount + 1
        local zombieName = "Zombie_" .. zombieCount

        local zombieObj = LoadObject("Assets\\GameTest\\TowerDefence\\Zombie\\Zombie.gltf", zombieName)
        if zombieObj then
            local x = Pos:getX()
            local y = Pos:getY()
            local z = Pos:getZ()

            zombieObj:SetPos(x, y, z)
            zombieObj:AddScript("Assets\\GameTest\\TowerDefence\\Zombie_ai.lua")
        else
            Log("Failed to load zombie object: " .. zombieName)
        end
    end
end

function OnCollision(otherEntity)

end
