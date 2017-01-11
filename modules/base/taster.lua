state=false
function paintEvent()
  splines={}
  splines[#splines+1]={{0.5,0.5},{1.5,0.5}}
  splines[#splines+1]={{0.5,0.5},{0.5,1.5}}
  splines[#splines+1]={{1.5,0.5},{1.5,1.5}}
  splines[#splines+1]={{0.5,1.5},{1.5,1.5}}
  c=0.2
  if state then
    c=0.25
  end
  splines[#splines+1]={true,{0.5+c,1},{1,0.5+c},{1.5-c,1},{1,1.5-c}}

  --splines[#splines+1]={{3,3},{1,1}}
  return splines
end
function onpress()
  state=true
end
function onrelease()
  state=false
end
function getState(pin)
  return state
end
return {width=2,height=1,name="Taster",pins={{2,1,"OUTPUT"}}}
