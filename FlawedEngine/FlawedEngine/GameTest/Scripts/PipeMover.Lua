function Create()
	print("PlayerCreated")
	ChangeColor(0, 1, 0)
	MovementSpeed = 0.01
end

function Update()
	Move(0, 0, MovementSpeed)

	local randomY = math.random(1, 10)
	if Pos.getZ() > 35 then
		SetPos(0, -5, -15)
		Scale(1, randomY, 1)
	end
end