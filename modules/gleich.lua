function paintEvent(painter,color)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,2)
  painter:drawText("=",0.5,0.5,3,3)
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,3)
end

function getState()
  return pins[1]==pins[2]
end
return {width=3,height=3,name="Gleich",pins={{0,1,"INPUT"},{0,3,"INPUT"},{4,2,"OUTPUT"}},category="base",description="Gleich-Gatter"}
