function paintEvent(painter,color)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,2)
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,3)
  painter:setPen("black")
  painter:drawText(utf8.char(8805).."1",0.5,0.5,3,3)
end

function getState()
  return pins[1] or pins[2]
end
return {width=3,height=3,name="OR",pins={{0,1,"INPUT"},{0,3,"INPUT"},{4,2,"OUTPUT"}},category="base",alt={"AND"},description="OR-Gatter mit 2 Eingängen"}
