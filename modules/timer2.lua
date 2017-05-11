state=false
function paintEvent(painter,color)
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,1,1)
  painter:setPen("black")
  painter:drawText("T",0.5,0.5,1,1)
  painter:setFont(nil,0.75)
  state=(time%400)>200
end
function getState(pin)
  return state
end
return {width=1,height=1,name="Timer2",pins={{2,1,"OUTPUT"}},category="in",description="Timer"}
