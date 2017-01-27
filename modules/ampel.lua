function drawAmpel(painter,x,y,offset)
  red=pins[1+offset]
  yellow=pins[2+offset]
  green=pins[3+offset]
  painter:setPen(nil)
  painter:setBrush("black")
  painter:drawRect(x-0.15,y-0.35,0.3,0.7)
  if red then
    painter:setBrush("#ff0000")
  else
    painter:setBrush("#110000")
  end
  painter:drawEllipse(x,y-0.2,0.075)
  if yellow then
    painter:setBrush("#ffff00")
  else
    painter:setBrush("#111100")
  end
  painter:drawEllipse(x,y,0.075)
  if green then
    painter:setBrush("#00ff00")
  else
    painter:setBrush("#001100")
  end
  painter:drawEllipse(x,y+0.2,0.075)
end
function paintEvent(painter,color)
  painter:setPen(nil)
  painter:setBrush("#333333")
  painter:drawRect(0.5,0.5,10,10)
  painter:setBrush("green")
  painter:drawRect(0.5,0.5,4,4)
  painter:drawRect(6.5,6.5,4,4)
  painter:drawRect(6.5,0.5,4,4)
  painter:drawRect(0.5,6.5,4,4)
  painter:setPen(nil)
  painter:setBrush("white")
  painter:drawRect(0.5,5.45,4,0.1)
  painter:drawRect(6.5,5.45,4,0.1)

  painter:drawRect(5.45,0.5,0.1,4)
  painter:drawRect(5.45,6.5,0.1,4)

  painter:drawRect(4.2,5.45,0.3,1.05)
  painter:drawRect(6.5,4.5,0.3,1.05)

  painter:drawRect(5.5,6.5,1.05,0.3)
  painter:drawRect(4.5,4.2,1.05,0.3)

  drawAmpel(painter,3,6,0)
  drawAmpel(painter,8,5,0)
  drawAmpel(painter,6,8,3)
  drawAmpel(painter,5,3,3)
end
function getState(pin)
  if (num>>(pin-1))%2==1 then
    return true
  end
  return false
end
function onclick(x,y)
  if y<2.5 then
    num=(num+1)%16
  else
    num=(num+15)%16
  end
end
return {width=11,height=10,name="Ampel 1",pins={{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{0,8,"INPUT"},{0,9,"INPUT"},{0,10,"INPUT"}},category="other",description="Kreuzung mit Ampel"}
