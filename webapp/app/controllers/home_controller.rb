require 'rexml/document'
require 'time'

class Trkpt
  attr_reader :time, :lat, :lon
  def initialize(elem)
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
  end
end


class HomeController < ApplicationController
  def index
  end

  def gpxUpload
    resulttext = storeGpx(params[:upload])
    render :text => "File has been uploaded successfully: #{resulttext}"
  end

  protected
  def storeGpx(file)
    originalname = file.original_filename
    content = file.read

    doc = REXML::Document.new content
    newslices = 0
    doc.elements.each("gpx/trk/trkseg") do | track |
      prev = nil
      track.elements.each do | elem_trkpt |
        trkpt = Trkpt.new elem_trkpt
        if prev.nil? or trkpt.time.nil?
          prev = trkpt
        else
          duration = prev.time.nil? ? 0 : trkpt.time - prev.time
          # Skip slices smaller than 9 seconds
          if duration > 29
            # Ignore everything above 95 seconds altogether
            if duration < 95
              slice = Slice.new(:time => trkpt.time,
                                :lat => trkpt.lat,
                                :lon => trkpt.lon,
                                :startlat => prev.lat,
                                :startlon => prev.lon,
                                :duration => duration)
              slice.save
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
