require 'nmea'
require 'gpx'
require 'congest'

# The controller class
class HomeController < ApplicationController
  attr_reader :minSliceDuration
  def initialize
    @minSliceDuration = 0  # formerly: 30
    super
  end

  protect_from_forgery :except => :gpxUpload

  def index
  end

  def gpxUpload
    rvalue = Gpx.storeGpx(params[:upload], minSliceDuration)

    # Use the instanceid and latest time to select the slices that go into the algorithm
    instanceid = rvalue[1]
    algoresult = Congest.callAlgo(instanceid, rvalue[2])

    # And return the "algoresult" as the very first value of the text. Done!
    render :text => "#{algoresult} predicted; #{rvalue[0]} successfully; id=#{instanceid}"
  end

  def nmeaUpload
    resulttext = Nmea.storeNMEA(params[:upload], minSliceDuration)
    render :text => "File has been uploaded successfully: #{resulttext}, with duration #{minSliceDuration} seconds each."
  end
  
end
