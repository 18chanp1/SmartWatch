
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

wifi.sta.config("YOGAC","cpen391l2b")
-- wifi.sta.config("TESTINGESP32","TESTINGRN")
wifi.sta.connect()
tmr.delay(3000000)
print(wifi.sta.status())
print(wifi.sta.getip())
