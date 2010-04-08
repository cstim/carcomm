class Slice < ActiveRecord::Base
  validates_presence_of :time, :lat, :lon, :avgvel
  validates_numericality_of :avgvel, :greater_than_or_equal_to => 0, :less_than => 100
  validates_numericality_of :lon, :startlon,
    :greater_than_or_equal_to => -180, :less_than_or_equal_to => 180
  validates_numericality_of :lat, :startlat,
    :greater_than_or_equal_to => -90, :less_than_or_equal_to => 90

  before_validation :calc_dist_if_empty!, :calc_heading_if_empty!,
    :calc_duration_from_avgvel!, :calc_avgvel_from_duration!

  DEG_TO_RAD = Math::PI / 180.0
  RAD_TO_DEG = 180.0 / Math::PI

  protected
  def calc_dist_if_empty!
    if dist.nil? and not (startlat.nil? or startlon.nil? or lat.nil? or lon.nil?)

      # We have the start and end point in WGS84 and want to calculate
      # the distance. This uses the Haversine formula, see
      # http://www.movable-type.co.uk/scripts/latlong.html

      earthR = 6371000 # m
      lat_rad = lat * DEG_TO_RAD
      startlat_rad = startlat * DEG_TO_RAD

      dLat = (lat - startlat) * DEG_TO_RAD
      dLon = (lon - startlon) * DEG_TO_RAD

      a = Math::sin(dLat / 2) * Math::sin(dLat / 2) \
          + Math::cos(startlat_rad) * Math::cos(lat_rad) \
          * Math::sin(dLon / 2) * Math::sin(dLon / 2)
      c = 2 * Math::atan2(Math::sqrt(a), Math::sqrt(1 - a))
      d = earthR * c

      self.dist = d
    end
  end

  protected
  def calc_heading_if_empty!
    if headingdeg.nil? and not (startlat.nil? or startlon.nil? or lat.nil? or lon.nil?)
      startlat_rad = startlat * DEG_TO_RAD
      endlat_rad = lat * DEG_TO_RAD
      dLon = (lon - startlon) * DEG_TO_RAD
      y = Math::sin(dLon) * Math::cos(startlat_rad)
      x = Math::cos(endlat_rad) * Math::sin(startlat_rad) \
          - Math::sin(endlat_rad) * Math::cos(startlat_rad) \
          * Math::cos(dLon)
      bearing_rad = Math::atan2(y, x)
      self.headingdeg = ((-bearing_rad + Math::PI) * RAD_TO_DEG) % 360
    end
  end

  protected
  def calc_duration_from_avgvel!
    if duration.nil? and not dist.nil? and not avgvel.nil?
      self.duration = dist / avgvel
    end
  end

  protected
  def calc_avgvel_from_duration!
    if avgvel.nil? and not dist.nil? and not duration.nil?
      self.avgvel  = dist / duration
    end
  end
end
