state=false
function paintEvent(painter,color)
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,1,1)
  painter:setPen("black")
  painter:drawText("T",0.5,0.5,1,1)
  painter:setFont(nil,0.75)
  state=(time%1000)>500
end
function getState(pin)
  return state
end
return {width=1,height=1,name="Timer",pins={{2,1,"OUTPUT"}},category="base",description="Timer"}
