

function Create()
	print("Create Function Being Called")

	ChangeColor(1, 1, 1)
	movementSpeed = 0.1
	Force = 10
end

function Update()
	if IsKeyDown(73) then
		Move(0, 0.0, movementSpeed)
	ApplyForce(0, 0.0, Force)
	end

	if IsKeyDown(75) then
		Move(0, 0.0, -movementSpeed)
		ApplyForce(0, 0.0, -Force)
	end

	if IsKeyDown(74) then
		Move(movementSpeed, 0.0, 0)
		ApplyForce(Force, 0.0, 0)
	end

	if IsKeyDown(76) then
		Move(-movementSpeed, 0.0, 0)
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
end