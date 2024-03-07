local hueValue = 0
local hueIncrement = 0.0001 -- Adjust the increment as per your requirement

-- Function to convert HSV to RGB
function HSVToRGB(h, s, v)
    local r, g, b

    local i = math.floor(h * 6)
    local f = h * 6 - i
    local p = v * (1 - s)
    local q = v * (1 - f * s)
    local t = v * (1 - (1 - f) * s)

    i = i % 6

    if i == 0 then r, g, b = v, t, p
    elseif i == 1 then r, g, b = q, v, p
    elseif i == 2 then r, g, b = p, v, t
    elseif i == 3 then r, g, b = p, q, v
    elseif i == 4 then r, g, b = t, p, v
    elseif i == 5 then r, g, b = v, p, q
    end

    return r, g, b
end

function Create()
    ChangeColor(1, 1, 1) -- Assuming ChangeColor function takes RGB values
end

function Update()
    hueValue = hueValue + hueIncrement
    if hueValue > 1 then
        hueValue = 0 -- Reset hue value when it exceeds 1
    end

    local r, g, b = HSVToRGB(hueValue, 1, 1) -- Convert HSV to RGB
    ChangeColor(r, g, b)
end



function OnCollision(otherEntityName)
    entity = GetEntity(otherEntityName)
    entity:ChangeColor(0, 1, 0)
end