function Create()
	print("PlayerCreated")
	ChangeColor(1, 0, 1)
	Force = 100
end

function Update()
	if IsKeyDown(73) then -- I
		print("[LUA] I is pressed")
		ApplyForce(0, Force, 0)
	end

	if TestFunction(5) then
		print("Test Function Succeeded")
	end

	print(Pos:getX())

end