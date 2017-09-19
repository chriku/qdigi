local socket=require"socket"
local avr=require"avr"
local file=io.open("avr.bin")
local chip=avr(file:read("*a"))
file:close()
while true do
  chip:doStep()
  socket.sleep(0.01)
end
