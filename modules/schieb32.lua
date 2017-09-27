local count=32
state={old=false,read=false}
for i=1,count do
  state[i]=false
end
function paintEvent(painter,color)
  painter:setFont(nil,0.75)
  painter:setBrush()
  painter:setPen(color)
  painter:drawLine(0.5,0.5,3.5,0.5)
  painter:drawLine(0.5,0.5,0.5,2.5)
  painter:drawLine(3.5,0.5,3.5,2.5)
  painter:drawLine(1,2.5,0.5,2.5)
  painter:drawLine(3,2.5,3.5,2.5)
  painter:drawLine(3,3.5,3,2.5)
  painter:drawLine(1,3.5,1,2.5)
  painter:drawRect(0.5,3.5,3,count)
  painter:setPen("black")

  painter:drawLine(0.5,0.6,1,1.0)
  painter:drawLine(0.5,1.4,1,1.0)

  painter:drawText("C1",1,0.5,1.4,1)
  painter:drawText("1IN",0.5,3.5,2,1)
  for i=1,count do
    painter:drawText("Q"..(i-1),2.0,2.5+i,1.5,1)
    if i<count then
      painter:drawLine(0.5,3.5+i,3.5,3.5+i)
    end
  end
end
function getState(pin)
  if pins[1]==true and state.old==false then
    state.read=pins[2]
  end
  if pins[1]==false and state.old==true then
    for i=count,2,-1 do
      state[i]=state[i-1]
    end
    state[1]=state.read
  end
  state.old=pins[1]
  return state[pin-2]
end
local pins={{0,1,"INPUT"},{0,4,"INPUT"}}
for i=1,count do
  pins[i+2]={4,3+i,"OUTPUT"}
end
return {width=3,height=3+count,name="Schieberegister-"..count,pins=pins,category="flip",description="Schieberegister"}
