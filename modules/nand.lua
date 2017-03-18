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
  return not (pins[1] and pins[2])
end
return {width=3,height=3,name="NAND",pins={{0,1,"INPUT"},{0,3,"INPUT"},{4,2,"OUTPUT"}},category="base",description="NAND-Gatter mit 2 Eingängen"}
