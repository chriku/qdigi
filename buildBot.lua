socket=require"ssl.https"
chat_id="-182420953"
function urlencode(str)
   if (str) then
      str = string.gsub (str, "\n", "\r\n")
      str = string.gsub (str, "([^%w ])",
         function (c) return string.format ("%%%02X", string.byte(c)) end)
      str = string.gsub (str, " ", "+")
   end
   return str    
end
os.execute("sudo ./build; cp qdigi.exe ~/mnt/")
os.execute("nano /tmp/message")
U=io.open("/tmp/message")
message="Neues Update:\n"..U:read("*a")
U:close()
print(socket.request("https://api.telegram.org/bot324897373:AAGKPfQHDUqWPIE7EyT_ScgeCLfU2iaWvHg/sendMessage?chat_id="..chat_id.."&text="..urlencode(message)))
