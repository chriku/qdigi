local function reg(r)
  return r
end
local function ioreg(r)
  return r+32
end
local function io(chip,reg,bit)
  print("OUT",reg,chip.ram[ioreg(reg)],bit)
end
local function jump(chip,k)
  if k>=0 then
    chip.PC=chip.PC+1+k
  else
    chip.PC=chip.PC+k
  end
end
local opcodes={}
opcodes["1100"]=function(first)
  local k=first%(4096)
  if k>2048 then
    k=-(4095~k)
  end
  return 2,{name="rjmp",k=k,exec=function(chip)
    jump(chip,k)
  end}
end
opcodes["0010 01"]=function(first)
  local r=first%16
  local d=(first>>4)%16
  if (first>>8)%2==1 then
    d=d|16
  end
  if (first>>9)%2==1 then
    r=r|16
  end
  return 2,{name="eor",r=r,d=d,exec=function(chip)
    chip.ram[reg(d)]=chip.ram[reg(r)]~chip.ram[reg(d)]
    chip.status.V=0
    if chip.ram[reg(d)]>127 then
      chip.status.N=1
    else
      chip.status.N=0
    end
    if chip.ram[reg(d)]==0 then
      chip.status.Z=1
    else
      chip.status.Z=0
    end
    chip.status.S=chip.status.N
    chip.PC=chip.PC+1
  end}
end
opcodes["1011 1"]=function(first)
  local A=first%16
  local r=(first>>4)%16
  if (first>>8)%2==1 then
    r=r|16
  end
  if (first>>9)%2==1 then
    A=A|16
  end
  if (first>>10)%2==1 then
    A=A|32
  end
  return 2,{name="out",A=A,r=r,exec=function(chip)
    chip.ram[ioreg(A)]=chip.ram[reg(r)]
    io(chip,A)
    chip.PC=chip.PC+1
  end}
end
opcodes["1110"]=function(first)
  local K=first%16
  local d=(first>>4)%16
  d=d+16
  K=K|(((first>>8)%16)<<4)
  return 2,{name="ldi",K=K,d=d,exec=function(chip)
    chip.ram[reg(d)]=K
    chip.PC=chip.PC+1
  end}
end
opcodes["1101"]=function(first)
  local k=first%(4096)
  if k>2048 then
    k=-(4095~k)
  end
   return 2,{name="rcall",k=k,exec=function(chip)
     chip.stack[#chip.stack+1]=chip.PC
     jump(chip,k)
   end}
end
opcodes["1001 1010"]=function(first)
  local b=first%8
  local A=(first>>3)%32
  return 2,{name="sbi",A=A,b=b,exec=function(chip)
    chip.ram[ioreg(A)]=chip.ram[ioreg(A)]|(1<<b)
    io(chip,A,b)
    chip.PC=chip.PC+1
  end}
end
opcodes["0101"]=function(first)
  local K=first%16
  local d=(first>>4)%16
  d=d+16
  K=K|(((first>>8)%16)<<4)
  return 2,{name="subi",K=K,d=d,exec=function(chip)
    local Rd3=(chip.ram[reg(d)]>>2)%2
    local K3=(K>>2)%2
    chip.ram[reg(d)]=(chip.ram[reg(d)]-K)%256
    chip.status.H=((~Rd3)&K3)|(Rd3&K3)|((Rd3)&K3)
    chip.PC=chip.PC+1
    chip.status.V=0
    chip.status.N=0
    if chip.ram[reg(d)]>127 then
      chip.status.N=1
    end
    chip.status.N=0
    chip.status.C=0
    if chip.ram[reg(d)] < K then
      chip.status.C=1
    end
    chip.status.Z=1
    if chip.ram[reg(d)] ~= 0 then
      chip.status.Z=0
    end
  end}
end
opcodes["0100"]=function(first)
  local K=first%16
  local d=(first>>4)%16
  d=d+16
  K=K|(((first>>8)%16)<<4)
  return 2,{name="sbci",exec=function(chip)
    local Rd3=(chip.ram[reg(d)]>>2)%2
    local K3=(K>>2)%2
    chip.ram[reg(d)]=chip.ram[reg(d)]-K
    chip.ram[reg(d)]=chip.ram[reg(d)]-chip.status.C
    chip.PC=chip.PC+1
    chip.status.H=((~Rd3)&K3)|(Rd3&K3)|((Rd3)&K3)
    chip.status.V=0
    chip.status.N=0
    chip.status.C=0
    if chip.ram[reg(d)] <0 then
      chip.status.C=1
    end
    if chip.ram[reg(d)] ~= 0 then
      chip.status.Z=0
    end
    chip.ram[reg(d)]=chip.ram[reg(d)] % 256
    if chip.ram[reg(d)]>127 then
      chip.status.N=1
    end
    chip.status.N=0
  end}
end
opcodes["1111 01.. .... .001"]=function(first)
  local k=(first>>3)%128
  if k>64 then
    k=-(k~127)
  end
  return 2,{name="brne",k=k,exec=function(chip)
    if chip.status.Z==0 then
      jump(chip,k)
    else
      chip.PC=chip.PC+1
    end
  end}
end
opcodes["0000 0000 0000 0000"]=function(first)
   return 2,{name="nop",exec=function(chip)
     chip.PC=chip.PC+1
   end}
end
opcodes["1001 1000"]=function(first)
  local b=first%8
  local A=(first>>3)%32
  return 2,{name="cbi",b=b,A=A,exec=function(chip)
    chip.ram[ioreg(A)]=chip.ram[ioreg(A)]&(0xFF~(1<<b))
    io(chip,A,b)
    chip.PC=chip.PC+1
  end}
end
opcodes["1001 0100 1111 1000"]=function(first)
  return 2,{name="cli"}
end
opcodes["1001 010. .... 1010"]=function(first)
  local d=(first>>4)%32
  return 2,{name="dec",d=d,exec=function(chip)
    chip.ram[reg(d)]=(chip.ram[reg(d)]-1)%256
    if chip.ram[reg(d)]==0 then
      chip.status.Z=1
    else
      chip.status.Z=0
    end
    chip.PC=chip.PC+1
  end}
end
local function getCode(first,pos)
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
      return v(first)
    end
  end
  print(string.format("%08x: ",(pos-1))..str)
  os.exit(0)
end
function parse(data)
  local pos=1
  local prog={}
  while pos<=data:len() do
    local first=string.unpack("<I2",data:sub(pos,pos+1))
    local len,data=getCode(first,pos)
    prog[(pos-1)/2]=data
    pos=pos+len
  end
  return prog
end

