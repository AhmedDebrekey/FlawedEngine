function lerp(startValue, endValue, t)
  return (1 - t) * startValue + t * endValue
end

LerpedValue = 0