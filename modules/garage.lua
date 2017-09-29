state={0,0,guy=false,alive=true}
local function guy(painter,x,y,alive)
  painter:setPen("black")
  painter:setBrush("white")
  if alive then
    painter:drawLine(x-0.25,y,x,y-0.5)
    painter:drawLine(x+0.25,y,x,y-0.5)
    painter:drawLine(x,y-1.25,x,y-0.5)
    painter:drawLine(x,y-1.25,x,y-1.4)
    painter:drawEllipse(x,y-1.6,0.2,0.2)
    painter:drawLine(x,y-1.25,x-0.5,y-1.25)
    painter:drawLine(x,y-1.25,x+0.5,y-1.25)
  else
    painter:setPen("red")
    painter:drawLine(x-0.25,y,x,y-0.5)
    painter:drawLine(x+0.25,y,x,y-0.5)
    painter:drawLine(x,y-1.25,x,y-0.5)
    painter:drawLine(x,y-1.25,x-0.15,y-1.25)
    painter:drawEllipse(x-0.35,y-1.25,0.2,0.2)
    painter:drawLine(x,y-1.25,x-0.5,y-0.75)
    painter:drawLine(x,y-1.25,x-0.5,y-1.75)
  end
end
function paintEvent(painter,color)
  painter:setPen(nil)
  painter:setBrush("white")
  painter:setPen(color)
  painter:drawRect(0.5,0.5,10,5)
  painter:setPen(nil)
  painter:setBrush("#333333")
  painter:drawRect(0.5,0.5,5,5*state[1])
  local lost=time-state[2]
  lost=lost/1000.0
  state[2]=time
  if pins[2] then
    state[1]=state[1]+(lost*0.1)
  end
  if pins[1] then
    state[1]=state[1]-(lost*0.1)
  end
  if state[1]<0 then state[1]=0 end
  if state[1]>1 then state[1]=1 end
  if state[1]>0.58 and state.guy then
    state.alive=false
  end
  if state.guy then
    guy(painter,3,5,state.alive)
  else
    guy(painter,8.5,5,state.alive)
  end
end
function getState(pin)
  if pin==3 then
    return state[1]<0.01
  end
  if pin==4 then
    return state[1]>0.99
  end
  if pin==5 then
    return state[1]>0.5 and state.guy
  end
  return false
end
function onclick(x,y)
  state.alive=true
  state.guy=not state.guy
  if state[1]>0.5 then
    state.guy=false
  end
end
return {width=11,height=5,name="Garage",pins={{0,1,"INPUT"},{0,2,"INPUT"}
,{11,1,"OUTPUT"}
,{11,2,"OUTPUT"}
,{11,3,"OUTPUT"}
},category="other",description="Garagentor"}

