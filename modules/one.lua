state=false
function paintEvent(painter,color)
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,1,1)
  painter:setPen("black")
  painter:setFont(nil,0.75)
  painter:drawText("1",0.5,0.5,1,1)
end
function getState(pin)
  return true
end
return {width=1,height=1,name="ONE",pins={{2,1,"OUTPUT"}},category="in",description="Dauereins"}
