function Create()
    Scale(50, 50, 50)
    local crate = GetEntity("Crate")
    crate:Scale(10, 10, 10)
    crate:ChangeColor(1, 0, 0)
end


function Update()

end

function OnCollision(otherEntity)

end