function Create()
	print("Create Function Being Called")

	ChangeColor(1, 1, 1)
	movementSpeed = 0.1
	Force = 10
	
	firstSphere = SpawnObject("Deb", 1)
	secondSphere = SpawnObject("Reky", 1)

	firstSphere:SetPos(2, 4, 1)
	secondSphere:SetPos(-2, 5, 1)
end

function Update()
		
	if movementSpeed == 0.5 then
		print("It is 0.5")
	end

	if IsKeyDown(73) then
		firstSphere:Rotate(0.2, 0.1, 0.4)
		secondSphere:Rotate(0.4, 0.1, 0.2)
		--Move(0, 0.0, movementSpeed)
		ApplyForce(0, 0.0, Force)
	end

	if IsKeyDown(75) then
		MoveCamera(0.0, 0.0, -movementSpeed)
		--Move(0, 0.0, -movementSpeed)
		ApplyForce(0, 0.0, -Force)
	end

	if IsKeyDown(74) then
		--Move(movementSpeed, 0.0, 0)
		ApplyForce(Force, 0.0, 0)
	end

	if IsKeyDown(76) then
		--Move(-movementSpeed, 0.0, 0)
		ApplyForce(-Force, 0.0, 0)
	end

	if IsKeyDown(79) then
		Rotate(0.0, -1, 0.0)
	end

	if IsKeyDown(85) then
		Rotate(0.0, 1, 0.0)
	end

	if IsKeyDown(77) then
		Scale(0.1, 0.1, 0.1)
	end

	if IsKeyDown(78) then 
		Scale(-0.1, -0.1, -0.1)
	end

	if IsKeyDown(90) then
		SpawnObject("Debreky", 1)
	end
end