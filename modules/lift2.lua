pos=0.0
otime=0
function paintEvent(painter)
  if otime==0 then
    otime=time
  end
  passed=time-otime
  otime=time
  if pins[1] and not pins[2] then
    pos=pos+(passed/2000.0)
  elseif pins[2] and not pins[1] then
    pos=pos-(passed/2000.0)
  end
  if pos>1 then
    pos=1.0
  elseif pos<0 then
    pos=0.0
  end
  painter:setPen("balck")
  painter:setBrush(nil)
  painter:drawRect(0.5,0.5,2,9)
  painter:drawLine(1.5,2,2.5,2)
  painter:drawLine(1.5,8,2.5,8)
  painter:drawLine(1.5,5,2.5,5)
  painter:drawEllipse(1.5,2+(6.0*pos),0.5)
end
function getState(pin)
  if pin==3 then
    if pos<0.001 then
      return true
    end
  end
  if pin==4 then
    if pos>0.49 then
      if pos<0.51 then
        return true
      end
    end
  end
  if pin==5 then
    if pos>0.999 then
      return true
    end
  end
  return false
end
return {width=3,height=9,name="Lift 2",pins={{0,1,"INPUT"},{0,2,"INPUT"},{3,2,"OUTPUT"},{3,5,"OUTPUT"},{3,8,"OUTPUT"}},category="other"}
