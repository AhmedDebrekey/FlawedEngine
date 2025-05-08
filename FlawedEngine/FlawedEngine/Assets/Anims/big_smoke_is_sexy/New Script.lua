function Create()
end

function Update()

end

function OnCollision(otherEntity)
    entity = GetEntity(otherEntity)
    entity:Remove()
end