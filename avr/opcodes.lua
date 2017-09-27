local function reg(r)
  return r
end
local function ioreg(r)
  return r+32
end
local function io(chip,reg,bit)
  print("DDRA",chip.ram[ioreg(26)])
  print("PORTA",chip.ram[ioreg(27)])
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
opcodes["0010 10"]=function(first)
  local r=first%16
  local d=(first>>4)%16
  if (first>>8)%2==1 then
    d=d|16
  end
  if (first>>9)%2==1 then
    r=r|16
  end
  return 2,{name="or",r=r,d=d,exec=function(chip)
    chip.ram[reg(d)]=chip.ram[reg(r)]|chip.ram[reg(d)]
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
     chip.stack[#chip.stack+1]={chip.PC+1}
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
opcodes["0110"]=function(first)
  local K=first%16
  local d=(first>>4)%16
  d=d+16
  K=K|(((first>>8)%16)<<4)
  return 2,{name="ori",K=K,d=d,exec=function(chip)
    local Rd3=(chip.ram[reg(d)]>>2)%2
    local K3=(K>>2)%2
    chip.ram[reg(d)]=(chip.ram[reg(d)]|K)
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
opcodes["0000"]=function(first)
  local r=first%16
  local d=(first>>4)%32
  if (first>>9)%2==1 then
    r=r|16
  end
  return 2,{name="sbc",exec=function(chip)
    local V=chip.ram[reg(d)]-chip.ram[reg(r)]-chip.status.C
    chip.status.C=0
    if V<0 then
      chip.status.C=1
    end
    while V<0 do
      V=256-V
    end
    if V~=0 then
      chip.status.Z=0
    end
    if V>127 then
      chip.status.N=0
    end
    chip.PC=chip.PC+1
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
opcodes["0000 0001"]=function(first)
  local r=first%16
  local d=(first>>4)%16
  r=r*2
  d=d*2
  return 2,{name="movw",r=r,d=d,exec=function(chip)
    chip.ram[reg(d)]=chip.ram[reg(r)]
    chip.ram[reg(d+1)]=chip.ram[reg(r+1)]
    chip.PC=chip.PC+1
  end}
end
opcodes["0011 0000"]=function(first)
  local K=first%16
  local d=(first>>4)%16
  d=d+16
  K=K|(((first>>8)%16)<<4)
  return 2,{name="cpi",r=r,d=d,exec=function(chip)
    chip.status.Z=0
    if chip.ram[reg(d)]==K then
      chip.status.Z=1
    end
    chip.status.C=0
    if chip.ram[reg(d)]<K then
      chip.status.C=1
    end
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
opcodes["0000 01"]=function(first)
  local r=first%16
  local d=(first>>4)%16
  if (first>>8)%2==1 then
    d=d|16
  end
  if (first>>9)%2==1 then
    r=r|16
  end
  return 2,{name="cpc",r=r,d=d,exec=function(chip)
    local v=chip.ram[reg(d)]-chip.ram[reg(r)]-chip.status.C
    chip.status.N=0
    if v>127 then
      chip.status.N=1
    end
    chip.status.V=0
    if v<0 then
      chip.status.V=1
    end
    chip.status.C=0
    if v<0 then
      chip.status.C=1
    end
    if v~=0 then
      chip.status.Z=1
    end
    chip.PC=chip.PC+1
  end}
end
opcodes["1001 010. .... 0000"]=function(first)
  local d=(first>>4)%32
  return 2,{name="com",d=d,exec=function(chip)
    local v=0xFF-chip.ram[reg(d)]
    chip.ram[reg(d)]=v
    chip.status.N=0
    if v>127 then
      chip.status.N=1
    end
    chip.status.V=0
    chip.status.C=1
    chip.status.Z=0
    if v~=0 then
      chip.status.Z=1
    end
    chip.PC=chip.PC+1
  end}
end
opcodes["1001 001. .... 1100"]=function(first)
  local r=(first>>4)%32
  return 2,{name="st",r=r,exec=function(chip)
    local X=chip.ram[reg(26)]|(chip.ram[reg(27)]<<8)
    chip.ram[X]=chip.ram[reg(r)]
    chip.ram[reg(26)]=X%256
    chip.ram[reg(27)]=X>>8
    chip.PC=chip.PC+1
  end}
end
opcodes["1001 0101 0001 1000"]=function(first)
  return 2,{name="reti",exec=function(chip)
    chip.PC=chip.stack[#chip.stack]
    chip.stack[#chip.stack]=nil
    chip.status.I=1
  end}
end
opcodes["1001 0101 0000 1000"]=function(first)
  return 2,{name="ret",exec=function(chip)
    chip.PC=chip.stack[#chip.stack]
    chip.stack[#chip.stack]=nil
  end}
end
opcodes["1001 0100 0111 1000"]=function(first)
  return 2,{name="sei",exec=function(chip)
    chip.status.I=1
    chip.PC=chip.PC+1
  end}
end
opcodes["1001 001. .... 1101"]=function(first)
  local r=(first>>4)%32
  return 2,{name="st",r=r,exec=function(chip)
    local X=chip.ram[reg(26)]|(chip.ram[reg(27)]<<8)
    chip.ram[X]=chip.ram[reg(r)]
    X=X+1
    chip.ram[reg(26)]=X%256
    chip.ram[reg(27)]=X>>8
    chip.PC=chip.PC+1
  end}
end
opcodes["10.0 ..1. .... 1..."]=function(first)
  local r=(first>>4)%32
  local q=(first%8)|(((first>>10)%4)<<3)|(((first>>13)%2)<<5)
  return 2,{name="std",r=r,q=q,exec=function(chip)
    local Y=chip.ram[reg(28)]|(chip.ram[reg(29)]<<8)
    chip.ram[Y+q]=chip.ram[reg(r)]
    chip.PC=chip.PC+1
  end}
end
opcodes["10.0 ..0. .... 1..."]=function(first)
  local d=(first>>4)%32
  local q=(first%8)|(((first>>10)%4)<<3)|(((first>>13)%2)<<5)
  return 2,{name="std",d=d,q=q,exec=function(chip)
    local Y=chip.ram[reg(28)]|(chip.ram[reg(29)]<<8)
    chip.ram[reg(d)]=chip.ram[Y+q]
    chip.PC=chip.PC+1
  end}
end
opcodes["0011"]=function(first)
  local d=(first>>4)%16
  local K=(first%16)|(((first>>8)%16)<<4)
  return 2,{name="cpi",r=r,K=K,exec=function(chip)
    local v=chip.ram[reg(d)]
    chip.status.H=0
    chip.status.N=((v>127) and 1) or 0
    chip.status.Z=((v==0) and 1) or 0
    chip.status.C=((K>v) and 1) or 0
    chip.status.S=chip.status.N*chip.status.V
    chip.PC=chip.PC+1
  end}
end
opcodes["1001 001. .... 1110"]=function(first)
  local r=(first>>4)%32
  return 2,{name="st",r=r,exec=function(chip)
    local X=chip.ram[reg(26)]|(chip.ram[reg(27)]<<8)
    chip.ram[X]=chip.ram[reg(r)]
    X=X-1
    chip.ram[reg(26)]=X%256
    chip.ram[reg(27)]=X>>8
    chip.PC=chip.PC+1
  end}
end
opcodes["1001 001. .... 1111"]=function(first)
  local d=(first>>4)%32
  return 2,{name="push",d=d,exec=function(chip)
    local X=chip.ram[reg(d)]
    chip.vstack[#chip.vstack+1]=X
    chip.PC=chip.PC+1
  end}
end
opcodes["1011 0"]=function(first)
  local d=(first>>4)%32
  local A=first%16
  A=A|(((first>>9)%4)<<4)
  return 2,{name="in",d=d,exec=function(chip)
    chip.ram[reg(d)]=chip.ram[ioreg(A)]
    chip.PC=chip.PC+1
  end}
end
opcodes["1001 000. .... 0000"]=function(first,second)
  local d=(first>>4)%32
  local k=second
  return 4,{name="lds",d=d,exec=function(chip)
    chip.ram[reg(d)]=chip.ram[k]
    chip.PC=chip.PC+2
  end}
end
opcodes["1001 001. .... 0000"]=function(first,second)
  local d=(first>>4)%32
  local k=second
  return 4,{name="sts",d=d,exec=function(chip)
    chip.ram[k]=chip.ram[reg(d)]
    chip.PC=chip.PC+2
  end}
end
opcodes["1001 000. .... 1111"]=function(first)
  local d=(first>>4)%32
  return 2,{name="push",d=d,exec=function(chip)
    chip.ram[reg(d)]=chip.vstack[#chip.vstack]
    chip.vstack[#chip.vstack]=nil
    chip.PC=chip.PC+1
  end}
end
return opcodes
