state=false
function paintEvent()
  splines={}
  splines[#splines+1]={{0.5,0.5},{1.5,0.5}}
  splines[#splines+1]={{0.5,0.5},{0.5,1.5}}
  splines[#splines+1]={{1.5,0.5},{1.5,1.5}}
  splines[#splines+1]={{0.5,1.5},{1.5,1.5}}
  if pins[1] then
    splines[#splines+1]={{0.5,0.5},{1.5,1.5}}
    splines[#splines+1]={{0.5,1.5},{1.5,0.5}}
  else
    splines[#splines+1]={true,{0.7,1},{1,0.6},{1.3,1},{1,1.4}}
  end
  --splines[#splines+1]={{0.5,1},{1,0.5},{1.5,1},{1,1.5},{0.5,1}}
  --splines[#splines+1]={{3,3},{1,1}}
  return splines
end
function getState(pin)
end
return {width=2,height=1,name="OUT",pins={{0,1,"INPUT"}}}
