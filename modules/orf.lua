function paintEvent(painter)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,2)
  painter:drawRect(0.5,0.5,3,2)
  painter:drawText(utf8.char(8805).."1",0.5,0.5,3,2)
end

function getState()
  return pins[1] or pins[2]
end
return {width=3,height=2,name="OR-Flat",pins={{0,1,"INPUT"},{0,2,"INPUT"},{4,2,"OUTPUT"}},category="base"}
