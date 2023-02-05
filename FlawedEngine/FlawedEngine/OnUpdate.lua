movementSpeed = 0.1
Force = 10

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

if IsKeyDown(80) then
	LerpedValue = lerp(LerpedValue, 1.0, 0.01)
	ChangeColor(1.0, LerpedValue, 1.0)
	if LerpedValue == 1 then
		print(LerpedValue)
		LerpedValue = 0
	end
end