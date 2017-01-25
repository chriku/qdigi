state=false
text={"1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"}
text[0]="0"
function paintEvent(painter)
  num=0
  for i=1,4 do
    num=num<<1
    if pins[5-i] then
      num=num|1
    end
  end
  painter:setPen("black")
  painter:setBrush(nil)
  painter:setFont(nil,3)
  painter:drawText(text[num],0.5,0.5,2,4)
  painter:drawRect(0.5,0.5,2,4)
end
function getState(pin)

end
return {width=2,height=4,name="Anzeige",pins={{0,1,"INPUT"},{0,2,"INPUT"},{0,3,"INPUT"},{0,4,"INPUT"}},category="base"}
