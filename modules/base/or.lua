function paintEvent()
  splines={}
  splines[#splines+1]={{0.5,0},{3.5,0}}
  splines[#splines+1]={{0.5,0},{0.5,4}}
  splines[#splines+1]={{3.5,0},{3.5,4}}
  splines[#splines+1]={{0.5,4},{3.5,4}}
  splines[#splines+1]={{1.5,1.3},{2,1.8}}
  splines[#splines+1]={{1.5,2.3},{2,1.8}}
  splines[#splines+1]={{1.5,2.7},{2,2.2}}
  splines[#splines+1]={{2.5,1.5},{2.5,2.5}} -- 1 --> (/) |
  splines[#splines+1]={{2.5,1.5},{2.25,1.75}} -- 1 --> / (|)
  return splines
end
function getState()
  return pins[1] or pins[2]
end
return {width=4,height=4,name="OR",pins={{0,1,"INPUT"},{0,3,"INPUT"},{4,2,"OUTPUT"}}}
