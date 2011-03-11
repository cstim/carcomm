require 'time'
require 'date'
require 'trkpt'

class Nmea
  # Convert a NMEA coordinate string to a floating point number
  def self.dmmm2dec(degrees, sw)
    deg = (degrees/100.0).floor #decimal degrees
    frac = ((degrees/100.0)-deg)/0.6 #decimal fraction
    ret = deg+frac #positive return value
    if ((sw=="S") or (sw=="W")):
      ret=ret*(-1) #flip sign if south or west
    end
    return ret
  end

  def self.storeNMEA(file, minSliceDuration)
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
          lat = Nmea.dmmm2dec((tokens[2]).to_f,tokens[3])
          lon = Nmea.dmmm2dec((tokens[4]).to_f,tokens[5])
          trkpt = Trkpt.new(datetime, lat, lon)

          if prev.nil? or trkpt.time.nil?
            prev = trkpt
          else
            duration = prev.time.nil? ? 0 : trkpt.time - prev.time
            # Skip slices smaller than the minimum duration
            if duration >= minSliceDuration
              # Ignore everything above 95 seconds altogether
              if Trkpt.postSlice(trkpt, prev)
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