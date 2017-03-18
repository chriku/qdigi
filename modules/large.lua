state={}
text={"B","C","D","E","F","G","H","I","J","K","L","M","N","O","P"}
text[0]="A"
for i=1,16 do
  state[i]=false
end
function paintEvent(painter,color)
  for i=1,16 do
    if state[i] then
      painter:setBrush("red")
    else
      painter:setBrush("black")
    end
    painter:setPen(color)
    painter:drawRect(0.5,0.5+(i-1),1,1)
    painter:setPen("white")
    txt=""
    painter:drawText(text[i-1],0.5,0.5+(i-1),1,1)
  end
end
function onclick(x,y)
  y=y-0.5
  y=y+1
  pos=math.floor(y)
  if state[pos]~=nil then
    state[pos]=not state[pos]
  end
end
function onkey(pos)
  pos=pos+1
  if state[pos]~=nil then
    state[pos]=not state[pos]
  end
end
function getState(pin)
  return state[pin]
end
return {width=1,height=16,name="Large-IN",pins={{2,1,"OUTPUT"},{2,2,"OUTPUT"},{2,3,"OUTPUT"},{2,4,"OUTPUT"},{2,5,"OUTPUT"},{2,6,"OUTPUT"},{2,7,"OUTPUT"},{2,8,"OUTPUT"},{2,9,"OUTPUT"},{2,10,"OUTPUT"},{2,11,"OUTPUT"},{2,12,"OUTPUT"},{2,13,"OUTPUT"},{2,14,"OUTPUT"},{2,15,"OUTPUT"},{2,16,"OUTPUT"}},category="in",description="16 Bit Eingang"}
