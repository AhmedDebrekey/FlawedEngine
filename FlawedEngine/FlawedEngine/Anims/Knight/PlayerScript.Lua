function Create()
	print("Knight Created")
	ChangeColor(0, 0, 0)
	Movement = 0.005
end

function Update()

	if IsKeyDown(73) then -- I
		Move(0, 0, Movement)
		ChangeAnimation("C:\\Dev\\FlawedEngine\\FlawedEngine\\FlawedEngine\\Anims\\Knight\\StrutWalking.dae")
	else
		ChangeAnimation("C:\\Dev\\FlawedEngine\\FlawedEngine\\FlawedEngine\\Anims\\Knight\\Idle.dae")
	end

end