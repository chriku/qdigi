function paintEvent(painter)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,2)
  painter:drawRect(0.5,0.5,3,3)
  painter:drawText("=1",0.5,0.5,3,3)
end
function getState()
  return (pins[1] or pins[2]) and not (pins[1] and pins[2])
end
return {width=3,height=3,name="EXOR",pins={{0,1,"INPUT"},{0,3,"INPUT"},{4,2,"OUTPUT"}}}
