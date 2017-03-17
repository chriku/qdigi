state=0
text={"1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"}
text[0]="0"
function paintEvent(painter,color)
  makeState()
  painter:setPen("black")
  painter:setBrush(nil)
  painter:setFont(nil,0.8)
  painter:drawText(text[state].."\nR\nWhite",0.5,0.5,2,4)
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
  if (state>>(pin-1))%2==1 then
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
  nums={1,3,5,7,8,9,11,13,15,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32}
  nums[0]=0
  for i=0,#nums do
    if nums[i]==num then
      state=i
      return
    end
  end
end
return {width=2,height=4,name="Reverse-White",alt={"White"},pins={{3,1,"OUTPUT"},{3,2,"OUTPUT"},{3,3,"OUTPUT"},{3,4,"OUTPUT"},{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{0,4,"INPUT"}},category="coder",description="White Code Rückcodierer"}
