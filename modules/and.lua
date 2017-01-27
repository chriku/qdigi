function paintEvent(painter,color)
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,3)
  painter:setPen("black")
  painter:drawSpline({{2.5,3},{1.5,1.2},{2.5,1.2},{1.5,2.5},{1.5,3},{2.5,2.5}})
end
function getState()
  return pins[1] and pins[2]
end
return {width=3,height=3,name="AND",pins={{0,1,"INPUT"},{0,3,"INPUT"},{4,2,"OUTPUT"}},category="base",alt={"OR"}}
