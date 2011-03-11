require 'time'

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
  
  # Post a slice into our database
  def self.postSlice(trkpt, prev)
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
  
end
