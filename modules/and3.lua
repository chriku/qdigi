function paintEvent(painter,color)
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,3)
  painter:setPen("black")
  painter:drawSpline({{2.5,3},{1.5,1.2},{2.5,1.2},{1.5,2.5},{1.5,3},{2.5,2.5}})
end
function getState()
  return pins[1] and pins[2] and pins[3]
end
return {width=3,height=3,name="AND3",pins={{0,1,"INPUT"},{0,3,"INPUT"},{0,2,"INPUT"},{4,2,"OUTPUT"}},category="base",alt={"OR3"},description="Und-Gatter mit 3 Eingängen"}
