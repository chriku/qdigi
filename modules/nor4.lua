function paintEvent(painter,color)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,1)
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,4)
  painter:setPen("black")
  painter:drawText("NOR",0.5,0.5,3,4)
end

function getState()
  return not (pins[1] or pins[2] or pins[3] or pins[4])
end
return {width=3,height=4,name="NOR4",pins={{0,1,"INPUT"},{0,3,"INPUT"},{0,2,"INPUT"},{0,4,"INPUT"},{4,2,"OUTPUT"}},category="base",alt={"AND4"},description="NOR-Gatter mit 4 Eingängen"}
