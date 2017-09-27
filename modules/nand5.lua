function paintEvent(painter,color)
  painter:setPen("black")
  painter:setBrush()
  painter:setFont(nil,1)
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,5)
  painter:setPen("black")
  painter:drawText("NAND",0.5,0.5,3,5)
end

function getState()
  return not (pins[1] and pins[2] and pins[3] and pins[4] and pins[5])
end
return {width=3,height=5,name="NAND5",pins={{0,1,"INPUT"},{0,3,"INPUT"},{0,2,"INPUT"},{0,4,"INPUT"},{0,5,"INPUT"},{4,2,"OUTPUT"}},category="base",alt={"AND5"},description="NAND-Gatter mit 5 Eingängen"}
