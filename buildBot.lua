socket=require"ssl.https"
chat_id="-182420953"
chat_id="-213111483"
function urlencode(str)
   if (str) then
      str = string.gsub (str, "\n", "\r\n")
      str = string.gsub (str, "([^%w ])",
         function (c) return string.format ("%%%02X", string.byte(c)) end)
      str = string.gsub (str, " ", "+")
   end
   return str    
end
os.execute("nano /tmp/message")
os.execute("cd .. && sudo umount mnt")
os.execute("cd .. && sshfs root@a20-olimex-2:/media/usb-stick/site/qdigi/downloads mnt")
if os.execute("sudo ./build") and os.execute("mkdir -p lbuild && cd lbuild && /usr/local/Qt-5.8-static/bin/qmake ../qdigi.pro && make -j8") then
  os.execute("cd lbuild && tar -czf qdigi.tar.gz qdigi")
  os.execute("rm -rf win;mkdir win")
  os.execute("cp qdigi.zip ../mnt/")
  os.execute("cp lbuild/qdigi ../mnt/")
  os.execute("cp lbuild/qdigi.tar.gz ../mnt/")
  os.execute("cd win && unzip ../qdigi.zip")
  os.execute("cp -r win/qdigi/* ~/mnt/")
  U=io.open("/tmp/message")
  message=U:read("*a")
  U:close()
  if (message:gsub("\n","")):len()>0 then
    message="Neues Update:\n"..message
    print(socket.request("https://api.telegram.org/bot324897373:AAGKPfQHDUqWPIE7EyT_ScgeCLfU2iaWvHg/sendMessage?chat_id="..chat_id.."&text="..urlencode(message)))
  end
  os.execute("rm -rf win qdigi")
end
