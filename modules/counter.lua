state=0
text={"1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"}
text[0]="0"
function paintEvent(painter,color)
  painter:setBrush(nil)
  painter:setFont(nil,3)
  painter:setPen("black")
  painter:drawText(text[state],0.5,0.5,2,4)
  painter:setFont(nil,1)
  painter:drawText("+",0.5,0.5,2,1)
  painter:drawText("-",0.5,3.5,2,1)
  painter:setPen(color)
  painter:drawRect(0.5,0.5,2,4)
end
function getState(pin)
  if (state>>(pin-1))%2==1 then
    return true
  end
  return false
end
function onclick(x,y)
  if y<1.5 then
    state=(state+1)%16
  elseif y>3.5 then
    state=(state+15)%16
  end
  print(y)
end
local context={}
context["Reset"]=function()
  state=0
end
return {width=2,height=4,name="Zaehler",pins={{3,1,"OUTPUT"},{3,2,"OUTPUT"},{3,3,"OUTPUT"},{3,4,"OUTPUT"}},category="base",description="HEX-Eingabe",context=context}
