-- SpringArm.lua
-- Minimal, beginner-friendly orbit/spring-arm camera "class".
-- No smoothing, no raycast, no key handling. Just math.

local SpringArm = {}
SpringArm.__index = SpringArm

-- new({
--   offsetY   = number (vertical offset above target, default 10)
--   distance  = number (camera distance, default 7)
--   pitch     = number (deg, default -20)
--   yaw       = number (deg, default 0)
--   pitchMin  = number (deg, default -89)
--   pitchMax  = number (deg, default  89)
-- })
function SpringArm.new(opts)
    opts = opts or {}
    local self = setmetatable({}, SpringArm)
    self.offsetY   = opts.offsetY   or 10.0
    self.distance  = opts.distance  or 7.0
    self.pitch     = opts.pitch     or -20.0
    self.yaw       = opts.yaw       or 0.0
    self.pitchMin  = opts.pitchMin  or -89.0
    self.pitchMax  = opts.pitchMax  or  89.0
    return self
end

local function clamp(v, a, b)
    if v < a then return a end
    if v > b then return b end
    return v
end

-- Set or adjust yaw
function SpringArm:setYaw(y)   self.yaw = (y or self.yaw) % 360 end
function SpringArm:addYaw(dy)  self.yaw = (self.yaw + (dy or 0)) % 360 end

-- Set or adjust pitch (clamped)
function SpringArm:setPitch(p) self.pitch = clamp(p or self.pitch, self.pitchMin, self.pitchMax) end
function SpringArm:addPitch(dp) self.pitch = clamp(self.pitch + (dp or 0), self.pitchMin, self.pitchMax) end

-- Compute camera pos/rot from a target position.
-- Returns: cx, cy, cz, rx, ry, rz  (use directly with SetCameraPos/Rot)
function SpringArm:compute(targetX, targetY, targetZ)
    local ty = targetY + self.offsetY
    local yawRad   = math.rad(self.yaw)
    local pitchRad = math.rad(self.pitch)

    -- Forward from yaw/pitch
    local fx =  math.cos(pitchRad) * math.sin(yawRad)
    local fy =  math.sin(pitchRad)
    local fz =  math.cos(pitchRad) * math.cos(yawRad)

    -- Camera position behind the target
    local cx = targetX - fx * self.distance
    local cy = ty      - fy * self.distance
    local cz = targetZ - fz * self.distance

    -- Camera rotation (pitch, yaw, roll=0)
    local rx, ry, rz = self.pitch, self.yaw, 0.0
    return cx, cy, cz, rx, ry, rz
end

-- Convenience: also apply to engine camera (and still return the values)
function SpringArm:apply(targetX, targetY, targetZ)
    local cx, cy, cz, rx, ry, rz = self:compute(targetX, targetY, targetZ)
    if SetCameraPos and SetCameraRot then
        SetCameraPos(cx, cy, cz)
        SetCameraRot(rx, ry, rz)
    end
    return cx, cy, cz, rx, ry, rz
end

return SpringArm
