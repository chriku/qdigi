function paintEvent(painter,color)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,1)
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,3)
  painter:setPen("black")
  painter:drawText("NAND",0.5,0.5,3,3)
end

function getState()
  return not (pins[1] and pins[2] and pins[3])
end
return {width=3,height=3,name="NAND3",pins={{0,1,"INPUT"},{0,3,"INPUT"},{0,2,"INPUT"},{4,2,"OUTPUT"}},category="base",alt={"AND3"},description="NAND-Gatter mit 3 Eingängen"}
