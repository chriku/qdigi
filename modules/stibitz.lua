state=0
text={"1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"}
text[0]="0"
function paintEvent(painter,color)
  painter:setPen("black")
  painter:setBrush(nil)
  painter:setFont(nil,0.8)
  painter:drawText(text[state].."\nStibitz",0.5,0.5,2,4)
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
  nums={4,5,6,7,8,9,10,11,12,0,0,0,0,0,0}
  nums[0]=3
  x3=nums[x1]
  if (x3>>(pin-1))%2==1 then
    return true
  end
  return false
end
function makeState()
  state=0
  num=0
  for i=1,4 do
    num=num<<1
    if pins[(5-i)+4] then
      num=num|1
    end
  end
  state=num
end
return {width=2,height=4,name="Stibitz",alt={"Reverse-Stibitz"},pins={{3,1,"OUTPUT"},{3,2,"OUTPUT"},{3,3,"OUTPUT"},{3,4,"OUTPUT"},{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{0,4,"INPUT"}},category="coder",alt={"Reverse-Stibitz"},description="Stibitz Code Codierer"}
