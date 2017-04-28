state=false
function paintEvent(painter,color)
  painter:setBrush()
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,1)
  painter:setPen("black")
  painter:drawText("NOT",0.5,0.5,3,1)
end
function getState()
  return not pins[1]
end
return {width=3,height=1,name="NOT",pins={{0,1,"INPUT"},{4,1,"OUTPUT"}},category="base",alt={},description="Nicht"}
