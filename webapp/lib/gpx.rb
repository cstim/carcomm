require 'rexml/document'
require 'trkpt'

class Gpx
  def self.storeGpx(file, minSliceDuration)
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
            if Trkpt.postSlice(trkpt, prev)
              newslices = 1 + newslices
            end
            prev = trkpt
          end
        end
      end # track.points.each
    end # gpx.tracks.each
    "Added #{newslices} new slices from #{originalname}"
  end

end