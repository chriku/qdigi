state={pos=0,dists={},lt=nil,jump=1,jumpdo=false,ok=true}
local function floor(painter,pos)
  for i=0,20,0.1 do
    local p=pos+i
    local c=math.sin(p)*0.1
    local gre=(math.sin(p/3.0)+1.0)*127
    painter:setPen(string.format("#FF%02X00",math.floor(gre)),0.1)
    painter:drawLine(i+0.5,9.8+c+0.5,i+0.5,10+0.5)
  end
end
function paintEvent(painter,color)
  painter:setPen(nil)
  painter:setBrush("white")
  painter:drawRect(0.5,0.5,20,10)
  state.lt=state.lt or time
  local missed=time-state.lt
  state.lt=time
  if state.ok then
    state.pos=state.pos+((missed/1000.0)*5.0)
  end
  floor(painter,state.pos)
  local dists=state.dists
  local i=1
  while i<=#dists do
    if dists[i]<(state.pos-10) then
      table.remove(dists,i)
      i=i-1
    end
    i=i+1
  end
  while #dists<35 do
    local last=(dists[#dists] or (state.pos+10))+3
    last=last+(math.random()*5.0)
    dists[#dists+1]=last
  end
  local fd=dists[1]
  painter:setPen(nil)
  painter:setBrush("gray")
  local first=1
  for i=first,#dists-1 do
    local start=dists[i]-state.pos
    if start>0 then
      if (dists[i+1]-state.pos)<22 then
        painter:drawRect(0.5+start,8,((dists[i+1]-state.pos)-2)-start,2)
      elseif start<20 then
        painter:drawRect(0.5+start,8,20-start,2)
      end
    elseif ((dists[i+1]-state.pos)-2)>0 then
      painter:drawRect(0.5,8,(dists[i+1]-state.pos)-2,2)
    end
  end
  if (state.jump<1 and state.jumpdo) or ((not state.ok) and state.jump<1.5) then
    state.jump=state.jump+(missed/1000.0)
  end
  local y=math.sin(state.jump*math.pi)
  if y<0 and state.ok then
    state.jump=0
    state.jumpdo=false
    y=0
  end
  y=math.max(y,-0.6)
  painter:setPen(nil)
  if y>-0.5 then
    painter:setBrush("black")
  else
    painter:setBrush("red")
  end
  if y<0.01 then
    for i=2,#dists do
      local x=dists[i]-state.pos
      if x>10 and x<12 then
        state.ok=false
      end
    end
  end
  painter:drawEllipse(10.5,7.5-(y*5.0),.5,.5)
  painter:setPen("black")
  if not state.ok then
    painter:drawText("Verloren",0,0,20,10)
  end
end
function getState(pin)
end
function onclick(x,y)
  math.randomseed(os.time())
  if not state.jumpdo then
    state.jumpdo=1
    state.jump=0
  end
end
return {width=20,height=10,name="Lava",pins={},category="other",description="Lava",context={reset=function()
  state={pos=0,dists={},lt=nil,jump=1,jumpdo=false,ok=true}
end}}

