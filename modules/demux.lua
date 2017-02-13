function paintEvent(painter,color)
  makeState()
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,4)
  painter:setPen("black")
  painter:drawText("G",0.5,0.5,1,2)
  painter:drawText("IN",0.5,3.5,1,1)
  painter:drawText("OUT0",1.5,0.5,2,1)
  painter:drawText("OUT1",1.5,1.5,2,1)
  painter:drawText("OUT2",1.5,2.5,2,1)
  painter:drawText("OUT3",1.5,3.5,2,1)
end
function getState(pin)
  makeState()
  return state[pin-3]
end
function makeState()
  dialed=0
  if pins[1] then
    dialed=dialed|1
  end
  if pins[2] then
    dialed=dialed|2
  end
  state={false,false,false,false}
  state[dialed+1]=pins[3]
end
return {width=3,height=4,name="DeMUX 4-1",pins={{0,1,"INPUT"},{0,2,"INPUT"},{0,4,"INPUT"},{4,1,"OUTPUT"},{4,2,"OUTPUT"},{4,3,"OUTPUT"},{4,4,"OUTPUT"}},category="multi",description="Demultiplexer"}
