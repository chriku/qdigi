function paintEvent(painter)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,2)
  painter:drawRect(0.5,0.5,3,4)
  painter:drawText(utf8.char(8805).."1",0.5,0.5,3,4)
end

function getState()
  return pins[1] or pins[2] or pins[3] or pins[4]
end
return {width=3,height=4,name="OR4",pins={{0,1,"INPUT"},{0,3,"INPUT"},{0,2,"INPUT"},{0,4,"INPUT"},{4,2,"OUTPUT"}}}
