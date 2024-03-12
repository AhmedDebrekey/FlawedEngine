-- Initialize variables
local player
local bulletSpawned = false

function Create()
    print("Bullet Created!")
    ChangeColor(1, 0, 0)
    SetPhysics(true)
    resetBullet()
    SetPos(0, -10, 0)
    player = GetEntity("Player")
end

function Update()
    -- Shoot bullet if 'K' key is pressed
    if IsKeyDown(75) then
        if not bulletSpawned then
            shootBullet()
        end
    end

    -- Reset bullet if 'O' key is pressed
    if IsKeyDown(79) then
        resetBullet()
    end

    if bulletSpawned then
        ApplyRelativeForce(500, 0, 0)
    end
end

function OnCollision(otherEntityName)
    if otherEntityName ~= "Player" then
        resetBullet()
        Remove()
    end
end

-- Function to shoot the bullet
function shootBullet()
    x = player:pgetX()
    y = player:pgetY()
    z = player:pgetZ()

    rX = player:rgetX()
    rY = player:rgetY()
    rZ = player:rgetZ()

    Rotate(rX, rY, rZ)

    SetPos(x, y + 7, z)
    SetDynamic(true) -- Enable physics for the bullet
    bulletSpawned = true -- Update bullet status
end

-- Function to reset the bullet
function resetBullet()
    SetPos(0, -5, 0) -- Reset bullet position
    SetDynamic(false) -- Disable physics for the bullet
    bulletSpawned = false -- Update bullet status
end
