state=false
function paintEvent(painter)
  painter:setPen()
  painter:setBrush("black")
  painter:drawRect(0.5,0.5,1,1)
end
function getState(pin)
  return false
end
return {width=2,height=1,name="NULL",pins={{2,1,"OUTPUT"}}}
