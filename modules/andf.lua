function paintEvent(painter,color)
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,2)
  painter:setPen("black")
  local spline={{2.5,2.5},{1.5,0.7},{2.5,0.7},{1.5,2.0},{1.5,2.5},{2.5,2.0}}
  local scale=0.8
  for i=1,#spline do
    local x=spline[i][1]
    local y=spline[i][2]
    x=x-2
    y=y-1.5
    x=x*scale
    y=y*scale
    x=x+2
    y=y+1.5
    spline[i]={x,y}
  end
  painter:drawSpline(spline)
end
function getState()
  return pins[1] and pins[2]
end
return {width=3,height=2,name="AND-Flat",pins={{0,1,"INPUT"},{0,2,"INPUT"},{4,2,"OUTPUT"}},category="base",alt={"OR-Flat"},description="Und-Gatter mit 2 Eingängen, Flach"}
