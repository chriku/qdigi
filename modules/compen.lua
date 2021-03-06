function paintEvent(painter,color)
  painter:setPen(color)
  painter:setBrush()
  painter:drawRect(0.5,0.5,3,3)
  painter:setPen("black")
  painter:drawText("A",0.5,0.5,1,1)
  painter:drawText("EN",0.5,1.5,1,1)
  painter:drawText("B",0.5,2.5,1,1)
  painter:drawText("A>B",1.5,0.5,2,1)
  painter:drawText("A=B",1.5,1.5,2,1)
  painter:drawText("A<B",1.5,2.5,2,1)
end
function getState(pin)
  state={false,false,false}
  if pins[1]==pins[2] then
    state[2]=true
  elseif pins[1] then
    state[1]=true
  else
    state[3]=true
  end
  return state[pin-3] and pins[3]
end
return {width=3,height=3,name="CompareEN",pins={{0,1,"INPUT"},{0,3,"INPUT"},{0,2,"INPUT"},{4,1,"OUTPUT"},{4,2,"OUTPUT"},{4,3,"OUTPUT"}},category="base",description="1 Bit Komparator mit Enable"}
