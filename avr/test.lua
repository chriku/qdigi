local socket=require"socket"
local avr=require"avr"
local file=io.open("avr.bin")
local chip=avr(file:read("*a"))
file:close()
while true do
  for i=1,250 do
    chip:doStep()
    socket.sleep(0.01)
  end
  chip:interrupt(0x0C)
end
