#!/usr/bin/ruby -I~/.gem/ruby/1.8/gems
#
# Call this as follows: RUBYLIB=~/.gem/ruby/1.8/gems ./gpxupload.rb
#

require 'gpx-0.6/lib/gpx.rb'
require 'rest_client'

$Server = 'http://localhost:3000/slices'
#$Server = 'http://carcomm.cstimming.de/slices'
$File = ARGV.size > 0 ? ARGV[0] : ENV["HOME"] + "/merkaator-export3.gpx"

def main
  gpx = GPX::GPXFile.new(:gpx_file => $File)

  #puts "Duration: #{gpx.duration}"
  puts "File: #{$File}"
  puts " Bounds: #{gpx.bounds}"

  gpx.tracks.each do | track |
    puts " Track Name: #{track.name}"
    puts "  Num Points: #{track.points.size}"

    prev = nil
    track.points.each do | trkpt |
      if prev.nil?
        prev = trkpt
      else
        duration = trkpt.time - prev.time
        # Ignore slices smaller than 5 seconds
        if duration > 9
          if duration < 95
            puts "Time: #{trkpt.time} Lat: #{trkpt.lat} Lon: #{trkpt.lon} Duration: #{duration}s Startlat: #{prev.lat} Startlon: #{prev.lon}"

            RestClient.post $Server, {
              "slice[lat]" => trkpt.lat,
              "slice[lon]" => trkpt.lon,
              "slice[time]" => trkpt.time,
              "slice[duration]" => duration,
              "slice[startlat]" => prev.lat,
              "slice[startlon]" => prev.lon }
          end
          prev = trkpt
        end
      end
    end
  end

end

main
