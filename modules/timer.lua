state=false
function paintEvent(painter)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,0.75)
  painter:drawRect(0.5,0.5,1,1)
  painter:drawText("T",0.5,0.5,1,1)
  state=(time%1000)>500
end
function getState(pin)
  return state
end
return {width=2,height=1,name="Timer",pins={{2,1,"OUTPUT"}},category="base"}
