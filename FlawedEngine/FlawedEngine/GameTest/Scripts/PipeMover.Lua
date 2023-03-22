function Create()
	print("PlayerCreated")
	ChangeColor(0, 1, 0)
	MovementSpeed = 0.01
	WinFlag = false
end

function Update()
	--If there is a collision, End game, Dont move.
	Move(0, 0, MovementSpeed)
	if Pos.getZ() > 35 then

		local randomZ = math.random(1, 10)
		local name = GetName()

		if name == "Pipe" then
			SetPos(0, -5, -15)
			Scale(3, 3, randomZ)
		elseif name == "Pipe2" then
			SetPos(0, 5, -15)
			Scale(3, 3, randomZ)
		end

	end
end 