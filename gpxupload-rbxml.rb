#!/usr/bin/ruby -I~/.gem/ruby/1.8/gems
#
# Call this as follows: RUBYLIB=~/.gem/ruby/1.8/gems ./gpxupload.rb
#

#require 'rest_client'
require 'rexml/document'
require 'time'
require 'date'

$Server = 'http://localhost:3000/slices'
#$Server = 'http://carcomm.cstimming.de/slices'
$File = ARGV.size > 0 ? ARGV[0] : nil

class Trkpt
  attr_reader :time, :lat, :lon

  def initialize(_time, _lat, _lon)
    @time = _time
    @lat = _lat
    @lon = _lon
  end

  def Trkpt.fromXml(elem)
    if not elem.attribute("lat").nil?
      @lat = elem.attribute("lat").to_s
    end
    if not elem.attribute("lon").nil?
      @lon = elem.attribute("lon").to_s
    end
    if elem.elements["time"]
      strtime = elem.elements["time"].text
      @time = Time.parse strtime
    end
  end
end

# Convert a NMEA coordinate string to a floating point number
def dmmm2dec(degrees, sw)
  deg = (degrees/100.0).floor #decimal degrees
  frac = ((degrees/100.0)-deg)/0.6 #decimal fraction
  ret = deg+frac #positive return value
  if ((sw=="S") or (sw=="W")):
      ret=ret*(-1) #flip sign if south or west
  end
  return ret
end

def sendWay(trkpt, prev)
  duration = prev.time.nil? ? 0 : trkpt.time - prev.time
  if duration < 95
    puts "Time: #{trkpt.time} Lat: #{trkpt.lat} Lon: #{trkpt.lon} Duration: #{duration}s Startlat: #{prev.lat} Startlon: #{prev.lon}"

#             RestClient.post $Server, {
#               "slice[lat]" => trkpt.lat,
#               "slice[lon]" => trkpt.lon,
#               "slice[time]" => trkpt.time,
#               "slice[duration]" => duration,
#               "slice[startlat]" => prev.lat,
#               "slice[startlon]" => prev.lon }
  end
end


def main
  if $File =~ /gpx$/
    # GPX file
    doc = REXML::Document.new File.new($File)

    puts "File: #{$File}"

    doc.elements.each("gpx/trk/trkseg") do | track |
      puts " Num Points: #{track.size}"

      prev = nil
      track.elements.each do | elem_trkpt |
        trkpt = Trkpt.fromXml elem_trkpt
        if prev.nil? or trkpt.time.nil?
          prev = trkpt
        else
          duration = prev.time.nil? ? 0 : trkpt.time - prev.time
          # Ignore slices smaller than 9 seconds
          if duration > 9
            sendWay(trkpt, prev)
            prev = trkpt
          end
        end
      end
    end
  else
    # NMEA file
    file = File.new($File)

    puts "File: #{$File}"

    date = nil
    prev = nil

    while (line = file.gets) do
      if line =~ /\$GPRMC/
        tokens = line.split(",")
        #time = Time.parse tokens[1]
        date = Date.strptime(tokens[9], '%d%m%y')
        #datetime = Time.utc(date.year, date.month, date.day,
        #                    time.hour, time.min, time.sec, time.usec)
        #puts "RMC datetime=#{datetime}"
      end
      if line =~ /\$GPGGA/
        tokens = line.split(",")
        time = Time.parse tokens[1]
        if tokens[6] != '0' and not date.nil?
          datetime = Time.utc(date.year, date.month, date.day,
                              time.hour, time.min, time.sec, time.usec)
          lat = dmmm2dec((tokens[2]).to_f,tokens[3]) #[2] is lat in deg+minutes, [3] is {N|S|W|E}
          lon = dmmm2dec((tokens[4]).to_f,tokens[5]) #[4] is long in deg+minutes, [5] is {N|S|W|E}
          #puts "Time=#{datetime} Lat=#{lat}, Lon=#{lon}"
          trkpt = Trkpt.new(datetime, lat, lon)
          #sleep 1

          if prev.nil? or trkpt.time.nil?
            prev = trkpt
          else
            duration = prev.time.nil? ? 0 : trkpt.time - prev.time
            # Ignore slices smaller than 9 seconds
            if duration > 2
              sendWay(trkpt, prev)
              prev = trkpt
            end
          end

        end
      end
    end

  end

end

main
