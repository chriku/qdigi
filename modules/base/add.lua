function paintEvent()
  splines={}
  splines[#splines+1]={{0.5,0},{3.5,0}}
  splines[#splines+1]={{0.5,0},{0.5,12}}
  splines[#splines+1]={{3.5,0},{3.5,12}}
  splines[#splines+1]={{0.5,12},{3.5,12}}

  splines[#splines+1]={{0.7,7},{1.1,5}}
  splines[#splines+1]={{1.5,7},{1.1,5}}
  splines[#splines+1]={{1.3,6},{0.9,6}}

  splines[#splines+1]={{1.8,5},{1.8,7}}
  splines[#splines+1]={{1.8,5},{2.4,6},{1.8,7}}

  splines[#splines+1]={{2.8,5},{2.8,7}}
  splines[#splines+1]={{2.8,5},{3.4,6},{2.8,7}}
--[[
  splines[#splines+1]={{2.5,3},{1.5,1.2}}
  splines[#splines+1]={{1.5,1.2},{2,1}}
  splines[#splines+1]={{2.5,1.2},{2,1}}
  splines[#splines+1]={{2.5,1.2},{1.5,2.5}}
  splines[#splines+1]={{1.5,3},{1.5,2.5}}
  splines[#splines+1]={{1.5,3},{2.5,2.5}}--]]--


  --splines[#splines+1]={{2.5,3},{1.5,1.2},{2.5,1.2},{1.5,2.5},{1.5,3},{2.5,2.5}}


  --splines[#splines+1]={{3,3},{1,1}}
  return splines
end
function getState(pin)
  states={}
  ina={pins[1],pins[2],pins[3],pins[4]}
  inb={pins[5],pins[6],pins[7],pins[8]}
  ci={pins[9],false,false,false,false}
  out={false,false,false,false}
  for i=1,4 do
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
  if pin==10 then return out[1] end
  if pin==11 then return out[2] end
  if pin==12 then return out[3] end
  if pin==13 then return out[4] end
  if pin==14 then return ci[5] end
  return false
  --return pins[1] and pins[2]
end
return {width=4,height=12,name="ADD",pins={

{0,1,"INPUT"},
{0,2,"INPUT"},
{0,3,"INPUT"},
{0,4,"INPUT"},

{0,6,"INPUT"},
{0,7,"INPUT"},
{0,8,"INPUT"},
{0,9,"INPUT"},

{0,11,"INPUT"},

{4,1,"OUTPUT"},
{4,2,"OUTPUT"},
{4,3,"OUTPUT"},
{4,4,"OUTPUT"},
{4,11,"OUTPUT"}}}
