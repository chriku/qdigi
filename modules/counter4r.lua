state={0,false}
function paintEvent(painter,color)
  if state[2]~=pins[1] and pins[1]==false then
    state[1]=(state[1]+1)%4
  end
  state[2]=pins[1]
  if pins[4] then
    state[1]=0
  end
  painter:setPen(color)
  painter:setBrush()
  painter:drawRect(0.5,0.5,3,2)
  painter:setPen("black")
  painter:setFont(nil,1)
  painter:drawText("IN",0.5,0.5,1,1)
end
function getState(pin)
  return ((state[1]>>(pin-2))%2)==1
end
return {width=3,height=3,name="Counter Div-4 R",pins={
{0,1,"INPUT"},
{4,1,"OUTPUT"},{4,2,"OUTPUT"},
{0,2,"INPUT"}},category="flip",description="Zähler bis 4 Reset"}

