function paintEvent(painter,color)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,2)
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,5)
  painter:setPen("black")
  painter:drawText(utf8.char(8805).."1",0.5,0.5,3,5)
end

function getState()
  return pins[1] or pins[2] or pins[3] or pins[4] or pins[5]
end
return {width=3,height=5,name="OR5",pins={{0,1,"INPUT"},{0,3,"INPUT"},{0,2,"INPUT"},{0,4,"INPUT"},{0,5,"INPUT"},{4,2,"OUTPUT"}},category="base",alt={"AND5"}}
