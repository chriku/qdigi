state={false,old=false}
function paintEvent(painter,color)
  if pins[2]==true and state.old==false then
    if pins[1] and pins[3] then
      state[1]=not state[1]
    elseif pins[1] then
      state[1]=true
    elseif pins[3] then
      state[1]=false
    end
  end
  state.old=pins[2]
  painter:setPen("black")
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,3)
  painter:drawLine(0.5,1.6,1,2.0)
  painter:drawLine(0.5,2.4,1,2.0)
  painter:setPen("black")
  painter:setFont(nil,1)
  painter:drawText("1J",1,0.5,1.4,1)
  painter:drawText("C1",1,1.5,1.4,1)
  painter:drawText("1K",1,2.5,1.4,1)
  painter:drawText("Q",2.5,0.5,1,1)
end
function getState(pin)
  if pin==4 then
    return state[1]
  else
    return not state[1]
  end
end
return {width=3,height=3,name="JK-FlipFlop",pins={{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{4,1,"OUTPUT"},{4,3,"OUTPUT"}},category="flip",description="Jump/Kill-FlipFlop"}
