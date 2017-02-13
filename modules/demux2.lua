function paintEvent(painter,color)
  makeState()
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,2)
  painter:setPen("black")
  painter:drawText("G",0.5,0.5,1,1)
  painter:drawText("IN",0.5,1.5,1,1)
  painter:drawText("OUT0",1.5,0.5,2,1)
  painter:drawText("OUT1",1.5,1.5,2,1)
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
  state={false,false,false,false}
  state[dialed+1]=pins[2]
end
return {width=3,height=2,name="DeMUX 2-1",pins={{0,1,"INPUT"},{0,2,"INPUT"},{4,1,"OUTPUT"},{4,2,"OUTPUT"}},category="multi",description="Demultiplexer"}
