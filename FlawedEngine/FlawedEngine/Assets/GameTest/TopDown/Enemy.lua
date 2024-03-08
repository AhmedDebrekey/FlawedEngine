function Create()
    print("Enemy Created!")
    player = GetEntity("Player")
    SetPhysics(true)
    resetEnemy()
    hitPoints = 20
end

function Update()

    if hitPoints == 0 then
        resetEnemy()
    end

end

function OnCollision(otherEntity)
    if otherEntity == "Bullet" then
        hitPoints = hitPoints - 1
        print(hitPoints)
    end
end

function resetEnemy()
    x = player:pgetX()
    y = player:pgetY()
    z = player:pgetZ()

    local range = 30

    -- Generate random offsets within the range
    local offsetX = math.random(-range, range)
    local offsetZ = math.random(-range, range)
    
    -- Calculate the enemy position relative to the player with the random offsets
    local enemyX = x + offsetX
    local enemyZ = z + offsetZ
    
    -- Set the enemy's position
    SetPos(enemyX, 10, enemyZ)
    SetDynamic(false)
    hitPoints = 20
end

