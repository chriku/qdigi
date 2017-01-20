function paintEvent(painter)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,2)
  painter:drawRect(0.5,0.5,3,3)
  painter:drawText(utf8.char(8805).."1",0.5,0.5,3,3)
end

function getState()
  return pins[1] or pins[2] or pins[3]
end
return {width=3,height=3,name="OR3",pins={{0,1,"INPUT"},{0,3,"INPUT"},{0,2,"INPUT"},{4,2,"OUTPUT"}},category="base"}
