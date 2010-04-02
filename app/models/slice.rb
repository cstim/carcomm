class Slice < ActiveRecord::Base
  validates_presence_of :time, :lat, :lon, :avgvel
  validates_numericality_of :avgvel, :greater_than_or_equal_to => 0, :less_than => 100
  validates_numericality_of :lon, :startlon,
    :greater_than_or_equal_to => -180, :less_than_or_equal_to => 180
  validates_numericality_of :lat, :startlat,
    :greater_than_or_equal_to => -90, :less_than_or_equal_to => 90

  before_validation :calc_dist_if_empty!, :calc_heading_if_empty!

  protected
  def calc_dist_if_empty!
    if dist.nil? and not (startlat.nil? or startlon.nil? or lat.nil? or lon.nil?)

      # We have the start and end point in WGS84 and want to calculate
      # the distance. This uses the Haversine formula, see
      # http://www.movable-type.co.uk/scripts/latlong.html

      earthR = 6371 # km
      lat_rad = lat / 180 * Math::PI
      startlat_rad = startlat / 180 * Math::PI

      dLat = (lat - startlat) / 180 * Math::PI
      dLon = (lon - startlon) / 180 * Math::PI

      a = Math::sin(dLat / 2) * Math::sin(dLat / 2) \
          + Math::cos(startlat_rad) * Math::cos(lat_rad) \
          * Math::sin(dLon / 2) * Math::sin(dLon / 2)
      c = 2 * Math::atan2(Math::sqrt(a), Math::sqrt(1 - a))
      d = earthR * c

      self.dist = d
    end
  end

  def calc_heading_if_empty!
    if headingdeg.nil? and not (startlat.nil? or startlon.nil? or lat.nil? or lon.nil?)
      startlat_rad = startlat / 180 * Math::PI
      lat_rad = lat / 180 * Math::PI
      dLon = (lon - startlon) / 180 * Math::PI
      y = Math::sin(dLon) * Math::cos(startlat_rad)
      x = Math::cos(lat_rad) * Math::sin(startlat_rad) \
          - Math::sin(lat_rad) * Math::cos(startlat_rad) * Math::cos(dLon)
      bearing_rad = Math::atan2(y, x)
      self.headingdeg = ((bearing_rad + Math::PI) / Math::PI * 180) % 360
    end
  end
end
