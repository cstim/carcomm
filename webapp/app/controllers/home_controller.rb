require 'nmea'
require 'gpx'

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
    resulttext = Gpx.storeGpx(params[:upload], minSliceDuration)
    render :text => "#{resulttext} successfully."

    #r = runMyAlgo

    #render :text => r.to_s
  end

  def nmeaUpload
    resulttext = Nmea.storeNMEA(params[:upload], minSliceDuration)
    render :text => "File has been uploaded successfully: #{resulttext}, with duration #{minSliceDuration} seconds each."
  end


end
