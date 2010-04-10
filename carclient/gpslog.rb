#!/usr/bin/ruby

# This is the 23-line example of reading the GPS data from the serial
# port and writing this to google maps.

require 'serialport'

def dmmm2dec(degrees,sw)
  deg= (degrees/100.0).floor #decimal degrees
  frac= ((degrees/100.0)-deg)/0.6 #decimal fraction
  ret = deg+frac #positive return value
  if ((sw=="S") or (sw=="W")):
      ret=ret*(-1) #flip sign if south or west
  end
  return ret
end

# '/dev/tty.holux', 4800
# /dev/ttyACM0, 9600 baud
sp = SerialPort.new('/dev/ttyACM0', 9600, 8, 1, SerialPort::NONE)

while(line=sp.readline) do
  if line =~ /\$GPGGA/
    tokens = line.split(",")
    lat = dmmm2dec((tokens[2]).to_f,tokens[3]) #[2] is lat in deg+minutes, [3] is {N|S|W|E}
    lng = dmmm2dec((tokens[4]).to_f,tokens[5]) #[4] is long in deg+minutes, [5] is {N|S|W|E}
    puts "http://maps.google.com/maps?ie=UTF8&ll=#{lat},#{lng}" 
    sleep 1
  end
end

