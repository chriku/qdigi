state={0,nil}
function paintEvent(painter,color)
  painter:setPen(color)
  if pins[1] then
    painter:setBrush("yellow")
  else
    painter:setBrush("black")
  end
  painter:drawRect(0.5,0.5,1,1)
  if pins[1] then
    local diff=time-(state[2] or time)
    state[2]=time
    state[1]=(state[1]+(diff*0.0025))%2
    painter:setBrush("red")
    if state[1]<1 then
      painter:drawRect(0.5,0.5,state[1],1)
    else
      local w=state[1]-1
      painter:drawRect(0.5+w,0.5,1.0-w,1)
    end
  end
  return splines
end
function getState(pin)
end
return {width=1,height=1,name="Alarm",pins={{0,1,"INPUT"}},category="out",description="Alarm Leuchte"}

