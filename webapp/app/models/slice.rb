require "geo"

class Slice < ActiveRecord::Base
  validates_presence_of :time, :lat, :lon, :avgvel
  validates_numericality_of :avgvel, :greater_than_or_equal_to => 0, :less_than => 100
  validates_numericality_of :lon, :startlon,
    :greater_than_or_equal_to => -180, :less_than_or_equal_to => 180
  validates_numericality_of :lat, :startlat,
    :greater_than_or_equal_to => -90, :less_than_or_equal_to => 90

  before_validation :calc_dist_if_empty!, :calc_heading_if_empty!,
    :calc_duration_from_avgvel!, :calc_avgvel_from_duration!

  # Set the lines per page in pagination view
  cattr_reader :per_page
  @@per_page = 50

  DEG_TO_RAD = Math::PI / 180.0
  RAD_TO_DEG = 180.0 / Math::PI

  protected

  def calc_dist_if_empty!
    if dist.nil? and not (startlat.nil? or startlon.nil? or lat.nil? or lon.nil?)

      # We have the start and end point in WGS84 and want to calculate
      # the distance. This uses the Haversine formula, see
      # http://www.movable-type.co.uk/scripts/latlong.html

      self.dist = Geo.haversine_dist(startlat, lat, startlon, lon)
    end
  end

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

  def calc_duration_from_avgvel!
    if duration.nil? and not dist.nil? and not avgvel.nil?
      self.duration = dist / avgvel
    end
  end

  def calc_avgvel_from_duration!
    if avgvel.nil? and not dist.nil? and not duration.nil?
      self.avgvel  = dist / duration
    end
  end
end
