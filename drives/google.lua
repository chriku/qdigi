--[[desktop=require"desktop"
settings=require"settings"
hserver=require"http.server"
http=require"http"
json=require"dkjson"
local token=""
lastToken=0
client_id="167107903260-s3lk7i287aidt7qj2l82ml6rsd3b2cot.apps.googleusercontent.com"
client_key="Qt3CQ9UMOkoE8TFkR-b6RZrL"
function getToken()
  if token~="" then
    if os.time()<lastToken then
      return token
    end
  end
  if settings.get("google_token")~="" then
    google_token=settings.get("google_token")
    data=json.decode(http.post("https://www.googleapis.com/oauth2/v4/token",{refresh_token=google_token,client_id=client_id,client_secret=client_key,grant_type="refresh_token"}))
    lastToken=os.time()+data["expires_in"]
    token=data["access_token"]
    return token
  end
  print("1")
  local server=hserver.listen()
  print("2")
  print(server)
  desktop.openUrl("https://accounts.google.com/o/oauth2/v2/auth?client_id="..client_id.."&redirect_uri=http://127.0.0.1:"..server:port().."&response_type=code&scope=https://www.googleapis.com/auth/drive")
  local head=""
  server:wait(function(url)
    head=url
    return "Close this Window"
  end)
  code=head:match("code=(.-) ")
  data=json.decode(http.post("https://www.googleapis.com/oauth2/v4/token",{code=code,client_id=client_id,client_secret=client_key,redirect_uri="http://127.0.0.1:"..server:port(),grant_type="authorization_code"}))
  lastToken=os.time()+data["expires_in"]
  token=data["access_token"]
  settings.set("google_token",data["refresh_token"])
  return token
end
function save(where,data)
  first="Content-Type: application/json; charset=UTF-8\r\n\r\n{\"name\":\""..where.."\"}"
  second="Content-Type: application/x-qdigi\r\n\r\n"..data
  boundary="123456789"
  while first:find(boundary) or second:find(boundary) do
    boundary=boundary..string.format("%02X",math.random(0,255))
  end
  data="--"..boundary.."\r\n"
  data=data..first.."\r\n"
  data=data.."--"..boundary.."\r\n"
  data=data..second.."\r\n"
  data=data.."--"..boundary.."\r\n"
  print(data)
  local token=getToken()
  local header={Authorization="Bearer "..token}
  header["Content-Type"]="multipart/related; boundary="..boundary
  print(http.post("https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart",data,header))
end
return {name="Google",prefix="gdrive",save=save}
--]]
