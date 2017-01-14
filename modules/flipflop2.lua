state=false
old=false
function paintEvent(painter)
  if pins[2]==false and old==true then
    state=pins[1]
  end
  old=pins[2]
  painter:setPen("black")
  painter:setBrush()
  painter:drawRect(0.5,0.5,3,3)
  painter:setFont(nil,1)
  painter:drawText("D",0.5,0.5,1,1)
  painter:drawText("C",0.5,2.5,1,1)
  painter:drawText("Q",2.5,0.5,1,1)
end
function getState(pin)
  if pin==3 then
    return state
  else
    return not state
  end
end
return {width=3,height=3,name="DC-FlipFlop",pins={{0,1,"INPUT"},{0,3,"INPUT"},{4,1,"OUTPUT"},{4,3,"OUTPUT"}}}
