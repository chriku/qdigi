state=false
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
function drawAmpelFus(painter,x,y,offset)
  red=pins[1+offset]
  green=pins[2+offset]
  painter:setPen(nil)
  painter:setBrush("black")
  painter:drawRect(x-0.15,y-0.2,0.3,0.4)
  if red then
    painter:setBrush("#ff0000")
  else
    painter:setBrush("#110000")
  end
  painter:drawEllipse(x,y-0.1,0.075)
  if green then
    painter:setBrush("#00ff00")
  else
    painter:setBrush("#001100")
  end
  painter:drawEllipse(x,y+0.1,0.075)
end
function onpress(x,y)
  print(x,y)
  if x>=5.2 and x<=5.8 then
    if (y>=0.7 and y<=1.3) or (y>=0.7 and y<=1.3) then
      state=true
    end
  end
end
function onrelease(x,y)
  if x>=5.2 and x<=5.8 then
    if (y>=0.7 and y<=1.3) or (y>=0.7 and y<=1.3) then
      state=false
    end
  end
end
function paintEvent(painter,color)
  painter:setPen(nil)
  painter:setBrush("#333333")
  painter:drawRect(0.5,0.5,10,6)
  painter:setBrush("green")
  painter:drawRect(0.5,0.5,4.5,2)
  painter:drawRect(6,4.5,4.5,2)
  painter:drawRect(6,0.5,4.5,2)
  painter:drawRect(0.5,4.5,4.5,2)
  painter:setPen(nil)
  painter:setBrush("white")

  painter:drawRect(0.5,3.45,10,0.1)

  drawAmpel(painter,3,4,0)
  drawAmpel(painter,8,3,0)

  drawAmpelFus(painter,5.5,5,3)
  drawAmpelFus(painter,5.5,2,3)

  painter:setPen(nil)
  painter:setBrush("white")
  painter:drawRect(4.95,2.5,0.05,2)
  painter:drawRect(6,2.5,0.05,2)
  painter:setBrush("#ffff33")
  painter:drawRect(5.2,5.7,.6,.6)
  painter:drawRect(5.2,0.7,.6,.6)
end
function getState(pin)
  return state
end
return {width=10,height=6,name="Ampel 3",pins={{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{0,5,"INPUT"},{0,6,"INPUT"},{11,1,"OUTPUT"}},category="other",description="Ampelsystem mit Fußgägerüberweg und Schaltern"}
