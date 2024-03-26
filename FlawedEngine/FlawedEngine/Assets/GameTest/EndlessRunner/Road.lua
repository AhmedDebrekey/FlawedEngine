function Create()

end

function Update()

end

function OnCollision(otherEntity)
    if otherEntity == "RoadKiller" then
        resetPos()
    end
end

function resetPos()
    SetPos(selectedX, 5, 0)
end