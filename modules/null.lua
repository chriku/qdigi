state=false
function paintEvent(painter,color)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,0.75)
  painter:drawRect(0.5,0.5,1,1)
  painter:drawText("0",0.5,0.5,1,1)
end
function getState(pin)
  return false
end
return {width=2,height=1,name="NULL",pins={{2,1,"OUTPUT"}},category="base"}
