state=0
text={"1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"}
text[0]="0"
digits={{1,1,1,1,1,1,0},--0
{0,1,1,0,0,0,0},--1
{1,1,0,1,1,0,1},--2
{1,1,1,1,0,0,1},--3
{0,1,1,0,0,1,1},--4
{1,0,1,1,0,1,1},--5
{1,0,1,1,1,1,1},--6
{1,1,1,0,0,0,0},--7
{1,1,1,1,1,1,1},--8
{1,1,1,1,0,1,1},--9
{1,1,1,0,1,1,1},--A
{0,0,1,1,1,1,1},--B
{1,0,0,1,1,1,0},--C
{0,1,1,1,1,0,1},--D
{1,0,0,1,1,1,1},--E
        {1,0,0,0,1,1,1}}--F
function paintEvent(painter,color)
  makeState()
  painter:setPen("black")
  painter:setBrush(nil)
  painter:setFont(nil,0.8)
  painter:drawText(text[state].."\n7",0.5,0.5,2,4)
  painter:setPen(color)
  painter:drawRect(0.5,0.5,2,7)
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
  if digits[state+1][pin]==1 then
    return true
  end
  return false
end
function makeState()
  state=0
  for i=1,4 do
    cstate=pins[i+7]
    if cstate==true then cstate=1 else cstate=0 end
    state=state|(cstate<<(i-1))
  end
end
return {width=2,height=7,name="7-Segment Coder",pins={{3,1,"OUTPUT"},{3,2,"OUTPUT"},{3,3,"OUTPUT"},{3,4,"OUTPUT"},{3,5,"OUTPUT"},{3,6,"OUTPUT"},{3,7,"OUTPUT"},{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{0,4,"INPUT"}},category="coder",description="Binär nach 7-Segment-Kodierer"}
