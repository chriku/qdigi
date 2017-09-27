state=false
function paintEvent(painter,color)
  num=0
  painter:setPen(color)
  painter:drawRect(0.5,0.5,4,7)
  w=0.2
  painter:setPen("#bbbbbb",w)
  painter:drawLine(1.5,1.5,3.5,1.5)--a
  painter:drawLine(3.5,1.5,3.5,4)--b
  painter:drawLine(3.5,6.5,3.5,4)--c
  painter:drawLine(1.5,6.5,3.5,6.5)--d
  painter:drawLine(1.5,6.5,1.5,4)--e
  painter:drawLine(1.5,1.5,1.5,4)--f
  painter:drawLine(1.5,4,3.5,4)--g

  if pins[1] then painter:setPen("black",w) else painter:setPen("transparent",w) end
  painter:drawLine(1.5,1.5,3.5,1.5)--a
  if pins[2] then painter:setPen("black",w) else painter:setPen("transparent",w) end
  painter:drawLine(3.5,1.5,3.5,4)--b
  if pins[3] then painter:setPen("black",w) else painter:setPen("transparent",w) end
  painter:drawLine(3.5,6.5,3.5,4)--c
  if pins[4] then painter:setPen("black",w) else painter:setPen("transparent",w) end
  painter:drawLine(1.5,6.5,3.5,6.5)--d
  if pins[5] then painter:setPen("black",w) else painter:setPen("transparent",w) end
  painter:drawLine(1.5,6.5,1.5,4)--e
  if pins[6] then painter:setPen("black",w) else painter:setPen("transparent",w) end
  painter:drawLine(1.5,1.5,1.5,4)--f
  if pins[7] then painter:setPen("black",w) else painter:setPen("transparent",w) end
  painter:drawLine(1.5,4,3.5,4)--g
  --painter:drawLine(2.1,3,2.9,3)
end
return {width=4,height=7,name="7 Segment",pins={{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{0,4,"INPUT"},{0,5,"INPUT"},{0,6,"INPUT"},{0,7,"INPUT"}},category="out",description="7-Segment-Anzeige"}
