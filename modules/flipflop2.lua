state={false,old=false}
function paintEvent(painter,color)
  if pins[2]==true and state.old==false then
    state[1]=pins[1]
  end
  state.old=pins[2]
  painter:setPen("black")
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,3)
  painter:drawLine(0.5,2.6,1,3.0)
  painter:drawLine(0.5,3.4,1,3.0)
  painter:setPen("black")
  painter:setFont(nil,1)
  painter:drawText("1D",1,0.5,1.4,1)
  painter:drawText("C1",1,2.5,1.4,1)
  painter:drawText("Q",2.5,0.5,1,1)
end
function getState(pin)
  if pin==3 then
    return state[1]
  else
    return not state[1]
  end
end
return {width=3,height=3,name="D-FlipFlop",pins={{0,1,"INPUT"},{0,3,"INPUT"},{4,1,"OUTPUT"},{4,3,"OUTPUT"}},category="flip",description="FlipFlop mit Daten und Clock"}
