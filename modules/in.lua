state=false
function paintEvent(painter,color)
  painter:setPen()
  if state then
    painter:setBrush("red")
  else
    painter:setBrush("black")
  end
  painter:drawRect(0.5,0.5,1,1)
end
function onclick()
  state=not state
end
function getState(pin)
  return state
end
return {width=2,height=1,name="IN",pins={{2,1,"OUTPUT"}},category="base"}
