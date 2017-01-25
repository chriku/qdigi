state=0
text={"1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"}
text[0]="0"
function paintEvent(painter)
  painter:setPen("black")
  painter:setBrush(nil)
  painter:setFont(nil,0.8)
  painter:drawText("PTE",0.5,0.5,2,4)
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
  if state>9 then
    return true
  end
  return false
end
function makeState()
  state=0
  num=0
  for i=1,4 do
    num=num<<1
    if pins[(5-i)+1] then
      num=num|1
    end
  end
  state=num
end
return {width=2,height=4,name="PTE",pins={{3,1,"OUTPUT"},{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{0,4,"INPUT"}},category="coder",alt={""}}
