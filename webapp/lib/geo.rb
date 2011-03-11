class Geo
  DEG_TO_RAD = Math::PI / 180.0
  RAD_TO_DEG = 180.0 / Math::PI
  def self.haversine_dist(startlat, lat, startlon, lon)
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
    earthR * c
  end

end
