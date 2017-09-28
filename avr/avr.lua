local parser=require"parser"
local function doStep(chip)
  local cmd=chip.flash[chip.PC]
  --print(string.format("%08x: %s",chip.PC*2,cmd.name))
  --print(chip.ram[25],chip.ram[24],chip.ram[18])
  cmd.exec(chip)
end
local function interrupt(chip,addr)
  if chip.status.I==1 then
    chip.stack[#chip.stack+1]=chip.PC
    chip.PC=addr/2
  end
end
return function(file)
  local chip={}
  chip.PC=0
  chip.status={I=0,T=0,H=0,S=0,V=0,N=0,Z=0,C=0}
  chip.ram={}
  chip.stack={}
  chip.vstack={}
  for i=0,223 do
    chip.ram[i]=0
  end
  chip.flash=parser(file)
  chip.doStep=doStep
  chip.interrupt=interrupt
  return chip
end
