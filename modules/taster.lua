state=false
function paintEvent(painter,color)
  painter:setPen()
  painter:setBrush("transparent")
  painter:drawRect(0.5,0.5,1,1)
  if state then
    painter:setBrush("red")
  else
    painter:setBrush("black")
  end
  painter:setPen(color)
  painter:drawEllipse(1,1,0.5,0.5)
end
function onpress()
  state=true
end
function onrelease()
  state=false
end
function getState(pin)
  return state
end
return {width=1,height=1,name="Taster",pins={{2,1,"OUTPUT"}},category="in",description="Taster"}
