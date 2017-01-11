state=false
function paintEvent()
  splines={}
  splines[#splines+1]={{0.5,0.5},{1.5,0.5}}
  splines[#splines+1]={{0.5,0.5},{0.5,1.5}}
  splines[#splines+1]={{1.5,0.5},{1.5,1.5}}
  splines[#splines+1]={{0.5,1.5},{1.5,1.5}}
  if state then
    splines[#splines+1]={{0.5,0.5},{1.5,1.5}}
    splines[#splines+1]={{0.5,1.5},{1.5,0.5}}
  else
    splines[#splines+1]={true,{0.7,1},{1,0.6},{1.3,1},{1,1.4}}
  end

  --splines[#splines+1]={{3,3},{1,1}}
  return splines
end
function onclick()
  state=not state
end
function getState(pin)
  return state
end
return {width=2,height=1,name="IN",pins={{2,1,"OUTPUT"}}}
