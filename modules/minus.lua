state=false
function paintEvent(painter,color)
  painter:setPen(color)
  painter:setFont(nil,1.0)
  painter:setBrush("black")
  painter:drawEllipse(1,1,0.5)
  if pins[1] then
    painter:setPen("red")
  else
    painter:setPen("black")
  end
  painter:drawText("-",0.5,0.5,1,1)
end
function getState(pin)
end
return {width=1,height=1,name="Minus",pins={{0,1,"INPUT"}},category="out",description="Minus LED"}
