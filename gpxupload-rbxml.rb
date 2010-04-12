#!/usr/bin/ruby -I~/.gem/ruby/1.8/gems
#
# Call this as follows: RUBYLIB=~/.gem/ruby/1.8/gems ./gpxupload.rb
#

#require 'rest_client'
require 'rexml/document'
require 'time' # this was missing

$Server = 'http://localhost:3000/slices'
#$Server = 'http://carcomm.cstimming.de/slices'
$File = ARGV.size > 0 ? ARGV[0] : ENV["HOME"] + "/merkaator-export3.gpx"

class Trkpt
  attr_reader :time, :lat, :lon
  def initialize(elem)
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



def main
  doc = REXML::Document.new File.new($File)

  puts "File: #{$File}"

  doc.elements.each("gpx/trk/trkseg") do | track |
    puts " Num Points: #{track.size}"

    prev = nil
    track.elements.each do | elem_trkpt |
      trkpt = Trkpt.new elem_trkpt
      if prev.nil? or trkpt.time.nil?
        prev = trkpt
      else
        duration = prev.time.nil? ? 0 : trkpt.time - prev.time
        # Ignore slices smaller than 5 seconds
        if duration > 9
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
          prev = trkpt
        end
      end
    end
  end

end

main
