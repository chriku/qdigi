state=0
text={"1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"}
text[0]="0"
function paintEvent(painter,color)
  makeState()
  painter:setPen("black")
  painter:setBrush(nil)
  painter:setFont(nil,0.8)
  painter:drawText(text[state].."\nGray",0.5,0.5,2,4)
  painter:setPen(color)
  painter:drawRect(0.5,0.5,2,4)
end
function getStateOrig(pin)
  if (state>>(pin-1))%2==1 then
    return true
  end
  return false
end
function getState(pin)
  makeState()
  x1=state
  x2=state>>1
  x3=x1~x2
  if (x3>>(pin-1))%2==1 then
    return true
  end
  return false
end
function makeState()
  state=0
  for i=1,4 do
    cstate=pins[i+4]
    if cstate==true then cstate=1 else cstate=0 end
    state=state|(cstate<<(i-1))
  end
end
return {width=2,height=4,name="Gray",alt={"Reverse-Gray"},pins={{3,1,"OUTPUT"},{3,2,"OUTPUT"},{3,3,"OUTPUT"},{3,4,"OUTPUT"},{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{0,4,"INPUT"}},category="coder",description="Gray Code Codierer"}
