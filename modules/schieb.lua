state={false,false,false,false,old=false,read=false}
function paintEvent(painter,color)
  painter:setFont(nil,1)
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

  painter:drawLine(0.5,0.6,1,1.0)
  painter:drawLine(0.5,1.4,1,1.0)

  painter:drawText("C1",1,0.5,1.4,1)
  painter:drawText("1IN",0.5,3.5,2,1)
  painter:drawText("Q0",2.0,3.5,1.5,1)
  painter:drawLine(0.5,4.5,3.5,4.5)
  painter:drawText("Q1",2.0,4.5,1.5,1)
  painter:drawLine(0.5,5.5,3.5,5.5)
  painter:drawText("Q2",2.0,5.5,1.5,1)
  painter:drawLine(0.5,6.5,3.5,6.5)
  painter:drawText("Q3",2.0,6.5,1.5,1)
end
function getState(pin)
  if pins[1]==true and state.old==false then
    state.read=pins[2]
  end
  if pins[1]==false and state.old==true then
    state[4]=state[3]
    state[3]=state[2]
    state[2]=state[1]
    state[1]=state.read
  end
  state.old=pins[1]
  return state[pin-2]
end
return {width=3,height=7,name="Schieberegister",pins={{0,1,"INPUT"},{0,4,"INPUT"},{4,4,"OUTPUT"},{4,5,"OUTPUT"},{4,6,"OUTPUT"},{4,7,"OUTPUT"}},category="flip",description="Schieberegister"}
