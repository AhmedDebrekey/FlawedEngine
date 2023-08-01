function Create()
	print("PlayerCreated")
	ChangeColor(1, 0, 1)
	Force = 100
end

function Update()
	if IsKeyDown(73) then -- I
		ApplyForce(0, Force, 0)
	end

	print(Pos:getX())

end