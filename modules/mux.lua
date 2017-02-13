function paintEvent(painter,color)
  painter:setBrush()
  painter:setPen(color)
  painter:drawLine(0.5,0.5,3.5,0.5)
  painter:drawLine(0.5,0.5,0.5,2.5)
  painter:drawLine(3.5,0.5,3.5,2.5)
  painter:drawLine(1,2.5,0.5,2.5)
  painter:drawLine(3,2.5,3.5,2.5)
  painter:drawLine(3,3.5,3,2.5)
  painter:drawLine(1,3.5,1,2.5)
  painter:drawRect(0.5,3.5,3,4)
  painter:setPen("black")
  painter:drawText("G",0.5,0.5,1,2)
  painter:drawText("I0",0.5,3.5,1,1)
  painter:drawText("I1",0.5,4.5,1,1)
  painter:drawText("I2",0.5,5.5,1,1)
  painter:drawText("I3",0.5,6.5,1,1)
  painter:drawText("OUT",1.5,4.5,2,1)
end
function getState()
  dialed=0
  if pins[1] then
    dialed=dialed|1
  end
  if pins[2] then
    dialed=dialed|2
  end
  ins={pins[3],pins[4],pins[5],pins[6]}
  return ins[dialed+1]
end
return {width=3,height=7,name="MUX 4-1",pins={{0,1,"INPUT"},{0,2,"INPUT"},{0,4,"INPUT"},{0,5,"INPUT"},{0,6,"INPUT"},{0,7,"INPUT"},{4,5,"OUTPUT"}},category="multi",description="Multiplexer"}
