require 'rexml/document'
require 'time'
require 'date'

# Represents one point of a GPX or NMEA track
class Trkpt
  attr_reader :time, :lat, :lon,
    :avgvel, :headingdeg,
    :accuracy, :instanceid, :categoryid

  def initialize(_time, _lat, _lon)
    @time = _time
    @lat = _lat
    @lon = _lon
    @avgvel = nil
    @headingdeg = nil
    @accuracy = 0
    @instanceid = 0
    @categoryid = 0
  end

  def fromXml(elem)
    @avgvel = nil
    @headingdeg = nil
    @accuracy = 0
    @instanceid = 0
    @categoryid = 0
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
    if not elem.attribute("avgvel").nil?
      @avgvel = elem.attribute("avgvel").to_s
    end
    if not elem.attribute("headingdeg").nil?
      @headingdeg = elem.attribute("headingdeg").to_s
    end
    if not elem.attribute("accuracy").nil?
      @accuracy = elem.attribute("accuracy").to_s
    end
    if not elem.attribute("instanceid").nil?
      @instanceid = elem.attribute("instanceid").to_s
    end
    if not elem.attribute("categoryid").nil?
      @categoryid = elem.attribute("categoryid").to_s
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

# Post a slice into our database
def postSlice(trkpt, prev)
  duration = prev.time.nil? ? 0 : trkpt.time - prev.time
  if duration < 95
    slice = Slice.new(:time => trkpt.time,
                      :lat => trkpt.lat,
                      :lon => trkpt.lon,
                      :startlat => prev.lat,
                      :startlon => prev.lon,
                      :duration => duration,
                      :avgvel => trkpt.avgvel,
                      :headingdeg => trkpt.headingdeg,
                      :startaccuracy => prev.accuracy,
                      :endaccuracy => trkpt.accuracy,
                      :instanceid => trkpt.instanceid,
                      :categoryid => trkpt.categoryid)
    slice.save
    return true
  else
    return false
  end
end


# The controller class
class HomeController < ApplicationController
  attr_reader :minSliceDuration

  def initialize
    @minSliceDuration = 0  # formerly: 30
    super
  end

  protect_from_forgery :except => :gpxUpload

  def index
  end

  def gpxUpload
    resulttext = storeGpx(params[:upload])
    render :text => "File has been uploaded successfully: #{resulttext}"
  end

  def nmeaUpload
    resulttext = storeNMEA(params[:upload])
    render :text => "File has been uploaded successfully: #{resulttext}, with duration #{minSliceDuration} seconds each."
  end

  protected
  def storeGpx(file)
    originalname = (defined? file.original_filename) ? file.original_filename : "<anonymous>"
    content = file.read

    doc = REXML::Document.new content
    newslices = 0
    doc.elements.each("gpx/trk/trkseg") do | track |
      prev = nil
      track.elements.each do | elem_trkpt |
        trkpt = Trkpt.new(nil, nil, nil)
        trkpt.fromXml elem_trkpt
        #logger.info("#{Time.now} Trying trkpt, got #{trkpt} from #{elem_trkpt}")
        if prev.nil? or trkpt.time.nil?
          prev = trkpt
        else
          duration = prev.time.nil? ? 0 : trkpt.time - prev.time
          # Skip slices smaller than 30 seconds
          if duration > minSliceDuration
            # Ignore everything above 95 seconds altogether
            if postSlice(trkpt, prev)
              newslices = 1 + newslices
            end
            prev = trkpt
          end
        end
      end # track.points.each
    end # gpx.tracks.each
    "Added #{newslices} new slices from #{originalname}"
  end

  protected
  def storeNMEA(file)
    originalname = file.original_filename

    date = nil
    prev = nil
    newslices = 0

    while (line = file.gets) do
      if line =~ /\$GPRMC/
        tokens = line.split(",")
        date = Date.strptime(tokens[9], '%d%m%y')
      end
      if line =~ /\$GPGGA/
        tokens = line.split(",")
        time = Time.parse tokens[1]
        if tokens[6] != '0' and not date.nil?
          datetime = Time.utc(date.year, date.month, date.day,
                              time.hour, time.min, time.sec, time.usec)
          lat = dmmm2dec((tokens[2]).to_f,tokens[3])
          lon = dmmm2dec((tokens[4]).to_f,tokens[5])
          trkpt = Trkpt.new(datetime, lat, lon)

          if prev.nil? or trkpt.time.nil?
            prev = trkpt
          else
            duration = prev.time.nil? ? 0 : trkpt.time - prev.time
            # Skip slices smaller than the minimum duration
            if duration >= minSliceDuration
              # Ignore everything above 95 seconds altogether
              if postSlice(trkpt, prev)
                newslices = 1 + newslices
              end
              prev = trkpt
            end
          end # end else prev.nil? or trkpt.time.nil?

        end # end tokens[6] != '0' and not date.nil?
      end # end line =~ /\$GPGGA/
    end # end while (line = file.gets) do

    "Added #{newslices} new slices from #{originalname}"
  end
end
