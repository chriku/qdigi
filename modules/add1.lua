function paintEvent(painter,color)
  painter:setPen(color)
  painter:drawRect(0.5,0.5,3,5)
  painter:setPen("black")
  painter:setFont(nil,1)
  painter:drawText("A",0.5,0.5,1,1)
  painter:drawText("B",0.5,2.5,1,1)
  painter:drawText("Q",2.5,0.5,1,1)
  painter:setFont(nil,.75)
  painter:drawText("CI",0.5,4.5,1.1,1)
  painter:drawText("CO",2.4,4.5,1.1,1)
  return splines
end
function getState(pin)
  states={}
  ina={pins[1]}
  inb={pins[2]}
  ci={pins[3],false,false,false,false}
  out={false,false,false,false}
  for i=1,1 do
    cnt=0
    if ina[i] then cnt=cnt+1 end
    if inb[i] then cnt=cnt+1 end
    if ci[i] then cnt=cnt+1 end
    if cnt%2==1 then
      out[i]=true
    end
    if cnt>=2 then
      ci[i+1]=true
    end
  end
  if pin==4 then return out[1] end
  if pin==5 then return ci[2] end
  return false
  --return pins[1] and pins[2]
end
return {width=3,height=5,name="ADD1",pins={

{0,1,"INPUT"},

{0,3,"INPUT"},

{0,5,"INPUT"},

{4,1,"OUTPUT"},
{4,5,"OUTPUT"}},category="base",description="1 Bit Volladdierer"}
