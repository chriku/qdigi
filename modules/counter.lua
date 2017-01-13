state=false
num=0
text={"1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"}
text[0]="0"
function paintEvent(painter)
  painter:setPen("black")
  painter:setBrush(nil)
  painter:setFont(nil,3)
  painter:drawText(text[num],0.5,0.5,2,4)
  painter:drawRect(0.5,0.5,2,4)
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
return {width=2,height=4,name="Zaehler",pins={{3,1,"OUTPUT"},{3,2,"OUTPUT"},{3,3,"OUTPUT"},{3,4,"OUTPUT"}}}
