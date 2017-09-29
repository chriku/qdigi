function paintEvent(painter,color)
  painter:setPen(color)
  painter:setBrush("white")
  painter:drawRect(0.5,0.5,1,4)
end
function onclick()
end
function getState(pin)
  return math.random(0,1)==1
end
return {width=1,height=4,name="RNG",pins={
{2,1,"OUTPUT"},
{2,2,"OUTPUT"},
{2,3,"OUTPUT"},
{2,4,"OUTPUT"}
},category="other",description="1 Bit Eingang"}