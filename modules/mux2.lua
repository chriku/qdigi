function paintEvent(painter,color)
  painter:setBrush()
  painter:setPen(color)
  painter:drawLine(0.5,0.5,3.5,0.5)
  painter:drawLine(0.5,0.5,0.5,1.5)
  painter:drawLine(3.5,0.5,3.5,1.5)
  painter:drawLine(1,1.5,0.5,1.5)
  painter:drawLine(3,1.5,3.5,1.5)
  painter:drawLine(3,2.5,3,1.5)
  painter:drawLine(1,2.5,1,1.5)
  painter:drawRect(0.5,2.5,3,2)
  painter:setPen("black")
  painter:drawText("G",0.5,0.5,1,1)
  painter:drawText("I0",0.5,2.5,1,1)
  painter:drawText("I1",0.5,3.5,1,1)
  painter:drawText("OUT",1.5,3.5,2,1)
end
function getState()
  dialed=0
  if pins[1] then
    dialed=dialed|1
  end
  ins={pins[2],pins[3]}
  return ins[dialed+1]
end
return {width=3,height=5,name="MUX 2-1",pins={{0,1,"INPUT"},{0,3,"INPUT"},{0,4,"INPUT"},{4,4,"OUTPUT"}},category="multi",description="Multiplexer"}
