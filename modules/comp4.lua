function paintEvent(painter,color)
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,9)
  painter:setPen("black")
  painter:drawText("A",0.5,0.5,1,4)
  painter:drawText("B",0.5,5.5,1,4)
  painter:drawText("A>B",1.5,3.5,2,1)
  painter:drawText("A=B",1.5,4.5,2,1)
  painter:drawText("A<B",1.5,5.5,2,1)
end
function getState(pin)
  state={false,false,false}
  num=0
  for i=1,4 do
    num=num<<1
    if pins[(4-i)+1] then
      num=num|1
    end
  end
  n1=num
  num=0
  for i=1,4 do
    num=num<<1
    if pins[(4-i)+5] then
      num=num|1
    end
  end
  n2=num
  if n1==n2 then
    state[2]=true
  elseif n1>n2 then
    state[1]=true
  else
    state[3]=true
  end
  return state[pin-8]
end
return {width=3,height=9,name="Compare4",pins={{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{0,4,"INPUT"},{0,6,"INPUT"},{0,7,"INPUT"},{0,8,"INPUT"},{0,9,"INPUT"},{4,4,"OUTPUT"},{4,5,"OUTPUT"},{4,6,"OUTPUT"}},category="base",description="4 Bit Komparator"}

