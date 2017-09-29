pos=0.0
otime=0
state=nil
type=0
function paintEvent(painter,color)
  painter:setPen(color)
  painter:setBrush("white")
  painter:drawRect(0.5,0.5,9,9)
  if otime==0 then
    otime=time
  end
  passed=time-otime
  otime=time
  pos=pos+(passed/2000.0)
  if pos>1 then
    pos=0
    type=math.random(0,2)
  elseif pos<0 then
    pos=0.0
  end
  painter:setPen("black")
  painter:setBrush()
  y=0
  if pos<0.5 then
    y=0
    state=nil
  elseif state==nil then
    state=pins[1]
  elseif pos<0.7 then
    y=(pos-0.5)/0.2
    if state then
      y=-y
    end
  else
    y=1
    if state then
      y=-y
    end
  end
  if type==0 then
    painter:drawEllipse((8.0*pos)+1,(y*3.5)+5,0.5)
  elseif type==1 then
    px=(8.0*pos)+1
    py=(y*3.5)+5
    h=0.5*math.sqrt(3.0)
    h1=h*(2.0^0.5)*0.5
    h2=h-h1
    painter:drawLine(px-0.5,py+h2,px+0.5,py+h2)
    painter:drawLine(px-0.5,py+h2,px,py-h1)
    painter:drawLine(px+0.5,py+h2,px,py-h1)
  else
    painter:drawRect((8.0*pos)+0.5,(y*3.5)+4.5,1.0,1.0)
  end
end
function getState(pin)
  if pin==2 then
    return type%2==1
  elseif pin==3 then
    return math.floor(type/2)==1
  end
  return false
end
return {width=9,height=9,name="Band",pins={{0,1,"INPUT"},{10,1,"OUTPUT"},{10,3,"OUTPUT"}},category="other",description="Schlechte Band-Simulation"}
