local utils = {}

function utils.Lerp(a, b, t)
    return a + (b - a) * t
end

function utils.LerpAngle(a, b, t)
    local delta = (b - a + 180) % 360 - 180
    return (a + delta * t) % 360
end

return utils
