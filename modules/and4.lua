function paintEvent(painter)
  painter:setPen("black")
  painter:setBrush()
  painter:drawRect(0.5,0.5,3,4)
  painter:drawSpline({{2.5,3.5},{1.5,1.7},{2.5,1.7},{1.5,3},{1.5,3.5},{2.5,3}})
end
function getState()
  return pins[1] and pins[2] and pins[3] and pins[4]
end
return {width=3,height=4,name="AND4",pins={{0,1,"INPUT"},{0,3,"INPUT"},{0,2,"INPUT"},{0,4,"INPUT"},{4,2,"OUTPUT"}},category="base",alt={"OR4"}}
