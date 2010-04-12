xml.gpx("version" => "1.0", "creator" => "CarComm Server",
        "xmlns:xsi" => "http://www.w3.org/2001/XMLSchema-instance",
        "xmlns" => "http://www.topografix.com/GPX/1/0",
        "xsi:schemaLocation" => "http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd") do
  for slice in @slices
    xml.rte do
      xml.extensions do
        xml.avgvel(slice.avgvel)
      end
      xml.avgvel(slice.avgvel.to_int)
      xml.rtept("lat" => slice.lat, "lon" => slice.lon) do
        xml.time(slice.time.to_datetime)
        xml.extensions do
          xml.avgvel(slice.avgvel)
          xml.startwpt("lat" => slice.startlat, "lon" => slice.startlon)
        end
      end
      xml.rtept("lat" => slice.startlat, "lon" => slice.startlon) do
        # We must have a sub-element here as otherwise the OpenLayers
        # parser doesn't like this file!
        xml.extensions do
          xml.avgvel(slice.avgvel)
        end
      end
    end
  end
end

