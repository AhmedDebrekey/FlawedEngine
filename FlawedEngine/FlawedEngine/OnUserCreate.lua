function lerp(startValue, endValue, t)
  return (1 - t) * startValue + t * endValue
end

LerpedValue = 0

Cube = 0
Sphere = 1
Cone = 2
Torus = 3
Triangle = 4
PointLight = 5
SpotLight = 6