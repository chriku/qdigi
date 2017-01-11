state=false
function paintEvent()
  if pins[1] and not pins[2] then
    state=true
  end
  if (not pins[1]) and pins[2] then
    state=false
  end
  splines={}
  splines[#splines+1]={{0.5,0},{3.5,0}}
  splines[#splines+1]={{0.5,0},{0.5,4}}
  splines[#splines+1]={{3.5,0},{3.5,4}}
  splines[#splines+1]={{0.5,4},{3.5,4}}
  return splines
end
function getState(pin)
  if pin==3 then
    return state
  else
    return not state
  end
end
return {width=4,height=4,name="FlipFlop",pins={{0,1,"INPUT"},{0,3,"INPUT"},{4,1,"OUTPUT"},{4,3,"OUTPUT"}}}
