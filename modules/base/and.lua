function paintEvent()
  splines={}
  splines[#splines+1]={{0.5,0},{3.5,0}}
  splines[#splines+1]={{0.5,0},{0.5,4}}
  splines[#splines+1]={{3.5,0},{3.5,4}}
  splines[#splines+1]={{0.5,4},{3.5,4}}
--[[
  splines[#splines+1]={{2.5,3},{1.5,1.2}}
  splines[#splines+1]={{1.5,1.2},{2,1}}
  splines[#splines+1]={{2.5,1.2},{2,1}}
  splines[#splines+1]={{2.5,1.2},{1.5,2.5}}
  splines[#splines+1]={{1.5,3},{1.5,2.5}}
  splines[#splines+1]={{1.5,3},{2.5,2.5}}--]]--


  splines[#splines+1]={{2.5,3},{1.5,1.2},{2.5,1.2},{1.5,2.5},{1.5,3},{2.5,2.5}}


  --splines[#splines+1]={{3,3},{1,1}}
  return splines
end
function getState()
  return pins[1] and pins[2]
end
return {width=4,height=4,name="AND",pins={{0,1,"INPUT"},{0,3,"INPUT"},{4,2,"OUTPUT"}}}
