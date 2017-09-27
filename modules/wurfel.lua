function paintEvent(painter,color)
  painter:setBrush("#ffffff")
  painter:setPen(color)
  painter:drawRect(0.5,0.5,4,4)
  painter:setPen(nil)
  painter:setBrush("red")
  if pins[1] then
    painter:drawEllipse(2.5,2.5,.3,.3)
  end
  if pins[4] then
    painter:drawEllipse(1.5,2.5,.3,.3)
    painter:drawEllipse(3.5,2.5,.3,.3)
  end
  if pins[2] then
    painter:drawEllipse(1.5,1.5,.3,.3)
    painter:drawEllipse(3.5,3.5,.3,.3)
  end
  if pins[3] then
    painter:drawEllipse(1.5,3.5,.3,.3)
    painter:drawEllipse(3.5,1.5,.3,.3)
  end
end
return {width=4,height=4,name="Wuerfel",pins={{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{0,4,"INPUT"}},category="other",description="Würfel"}
