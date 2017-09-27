state={false,false,old=false}
function paintEvent(painter,color)
  painter:setPen("black")
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,3)
  painter:drawLine(0.5,2.6,1,3.0)
  painter:drawLine(0.5,3.4,1,3.0)

  painter:drawLine(3.0,0.7,3.0,1.3)
  painter:drawLine(3.0,0.7,2.8,0.7)

  painter:drawLine(3.0,2.7,3.0,3.3)
  painter:drawLine(3.0,2.7,2.8,2.7)

  painter:setPen("black")
  painter:setFont(nil,1)
  painter:drawText("1D",1,0.5,1.4,1)
  painter:drawText("C1",1,2.5,1.4,1)
end
function getState(pin)
  if pins[2]==true and state.old==false then
    state[1]=pins[1]
  end
  if pins[2]==false and state.old==true then
    state[2]=state[1]
  end
  state.old=pins[2]
  if pin==3 then
    return state[2]
  else
    return not state[2]
  end
end
return {width=3,height=3,name="MS-D-FlipFlop",pins={{0,1,"INPUT"},{0,3,"INPUT"},{4,1,"OUTPUT"},{4,3,"OUTPUT"}},category="flip",description="Master Slave FlipFlop mit Daten und Clock"}
