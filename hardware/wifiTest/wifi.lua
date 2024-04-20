-- function to print to UART(delay every 5bytes)
function print_c(c)
  sdata = 1
  uart.on("data")
  while sdata <= string.len(c) do
      if(string.len(c)-sdata < 5) then
          uart.write(0,string.sub(c,sdata,string.len(c)))
      else
          uart.write(0,string.sub(c,sdata,sdata+4))
      end
      tmr.delay(1000) 
      sdata = sdata+5
  end
  uart.write(0,"\n");
  sdata = 1
end

-- function to display as a call back on receive
function display(sck, c) 
  if (c ~= nil) then 
    -- print_c(string.len(c))
    print_c(c) 
  else 
    print("error: c = nil")
  end
  collectgarbage()
end

-- function to display as a call back on receive image
function display_image(sck, c) 
  -- print_c(string.len(c))    
-- look for the \r\n\r\n that separates an http response header from the body
  if (string.find(c,"\r\n\r\n") ~= nil) then
      data = string.sub(c,string.find(c,"\r\n\r\n") + 4)
      print_c(data) 
  end
  collectgarbage()
end

-- function to send a http POST request to get the static map
function get_image(host,uri,longitude,latitude)
  socket = net.createConnection(net.TCP, 0)
  socket:on("receive",display) --todo: test to display image
  socket:on("connection", function(sck, c)
    
  payload = "{\"longitude\":"..longitude..",\"latitude\":"..latitude.."}"
  req_data = "POST /"..uri.." HTTP/1.0\r\n"..
  "Host: "..host.."\r\n"..
  "Connection: keep-alive\r\n"..
  "Accept-Charset: utf-8\r\n"..
  "Accept-Encoding: \r\n"..
  "Content-Length:"..string.len(payload).."\r\n"..
  "Content-Type: application/json\r\n".. 
  "User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n".. 
  "Accept: */*\r\n\r\n"..
  payload

    sck:send(req_data)
  end)
  tmr.delay(2000000) -- wait to connect
  socket:connect(3000,host)
end

function check_wifi()
  ip = wifi.sta.getip()

 if(ip==nil) then
   print_c("Connecting...")
 else
  tmr.stop(0)
  print_c("Connected to AP!")
  print_c(ip)
  
  get_image("192.168.137.150","submitserial","0","0"); -- todo)
 end
end

check_wifi()