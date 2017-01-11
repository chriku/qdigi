state=false
num=0
font={}
font[0]={1,1,1,1,1,1,0}
font[1]={0,1,1,0,0,0,0}
font[2]={1,1,0,1,1,0,1}
font[3]={1,1,1,1,0,0,1}
font[4]={0,1,1,0,0,1,1}
font[5]={1,0,1,1,0,1,1}
font[6]={1,0,1,1,1,1,1}
font[7]={1,1,1,0,0,0,0}
font[8]={1,1,1,1,1,1,1}
font[9]={1,1,1,1,0,1,1}
font[10]={1,1,1,0,1,1,1}
font[11]={0,0,1,1,1,1,1}
font[12]={1,0,0,1,1,1,0}
font[13]={0,1,1,1,1,0,1}
font[14]={1,0,0,1,1,1,1}
font[15]={1,0,0,0,1,1,1}
function paintEvent()
  splines={}
  splines[#splines+1]={{0.5,0},{2.5,0}}
  splines[#splines+1]={{0.5,0},{0.5,5}}
  splines[#splines+1]={{2.5,0},{2.5,5}}
  splines[#splines+1]={{0.5,5},{2.5,5}}

  char=font[num]

  if char[1]==1 then splines[#splines+1]={{1.1,1},{2.1,1}} end -- a
  if char[2]==1 then splines[#splines+1]={{2.1,1},{2,2.5}} end -- b
  if char[3]==1 then splines[#splines+1]={{1.9,4},{2,2.5}} end -- c
  if char[4]==1 then splines[#splines+1]={{0.9,4},{1.9,4}} end -- d
  if char[5]==1 then splines[#splines+1]={{0.9,4},{1,2.5}} end -- e
  if char[6]==1 then splines[#splines+1]={{1.1,1},{1,2.5}} end -- f
  if char[7]==1 then splines[#splines+1]={{1,2.5},{2,2.5}} end -- g

  return splines
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
return {width=3,height=5,name="Zaehler",pins={{3,1,"OUTPUT"},{3,2,"OUTPUT"},{3,3,"OUTPUT"},{3,4,"OUTPUT"}}}
