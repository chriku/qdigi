function paintEvent(painter)
  painter:setPen("black")
  painter:setBrush()
  painter:drawRect(0.5,0.5,3,2)
  painter:drawSpline({{2.5,2.5},{1.5,0.7},{2.5,0.7},{1.5,2.0},{1.5,2.5},{2.5,2.0}})
end
function getState()
  return pins[1] and pins[2]
end
return {width=3,height=2,name="AND-Flat",pins={{0,1,"INPUT"},{0,2,"INPUT"},{4,2,"OUTPUT"}},category="base"}
