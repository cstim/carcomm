xml.gpx("version" => "1.0", "creator" => "CarComm Server",
        "xmlns:xsi" => "http://www.w3.org/2001/XMLSchema-instance",
        "xmlns" => "http://www.topografix.com/GPX/1/0",
        "xsi:schemaLocation" => "http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd") do
  for slice in @slices
    xml.wpt("lat" => slice.lat, "lon" => slice.lon) do
      xml.time(slice.time.to_datetime)
      xml.extension do
        xml.avgvel(slice.avgvel)
        xml.startwpt("lat" => slice.startlat, "lon" => slice.startlon)
      end
    end
  end
end

