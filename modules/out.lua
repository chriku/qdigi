state=false
function paintEvent(painter,color)
  painter:setPen(color)
  if pins[1] then
    painter:setBrush("red")
  else
    painter:setBrush("black")
  end
  painter:drawRect(0.5,0.5,1,1)
  return splines
end
function getState(pin)
end
return {width=1,height=1,name="OUT",pins={{0,1,"INPUT"}},category="base",description="LED"}
