local opcodes=require"opcodes"
local function getCode(first,pos,second)
  local f=first
  local str=""
  local i=0
  for i=1,16 do
    if (first%2)==0 then
      str="0"..str
    else
      str="1"..str
    end
    if i%4==0 and i<16 then
      str=" "..str
    end
    first=first>>1
  end
  first=f
  for k,v in pairs(opcodes) do
    if str:find("^"..k) then
      return v(first,second)
    end
  end
  print(string.format("%08x: ",(pos-1))..str)
  os.exit(0)
end
return function(data)
  local pos=1
  local prog={}
  while pos<=data:len() do
    local first=string.unpack("<I2",data:sub(pos,pos+1))
    local second=0
    if (pos+3)<=data:len() then
      second=string.unpack("<I2",data:sub(pos+2,pos+3))
    end
    local len,data=getCode(first,pos,second)
    prog[(pos-1)/2]=data
    pos=pos+len
  end
  return prog
end
