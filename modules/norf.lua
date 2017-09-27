function paintEvent(painter,color)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,1)
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,2)
  painter:setPen("black")
  painter:drawText("NOR",0.5,0.5,3,2)
end

function getState()
  return not(pins[1] or pins[2])
end
return {width=3,height=2,name="NOR-Flat",pins={{0,1,"INPUT"},{0,2,"INPUT"},{4,2,"OUTPUT"}},category="base",alt={"AND-Flat"},description="NOR-Gatter mit 2 Eingängen, Flach"}
