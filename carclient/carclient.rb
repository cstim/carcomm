#!/usr/bin/ruby
# http://www.arachnoid.com/ruby/RubyGUIProject/index.html

require 'Qt'
require 'qtwebkit'
require 'mainwindow.ui'
require 'ressources.qrc'

def qtDateTimeToString(dateTime)
  dateTime.toString("yyyy-MM-ddThh:mm:ssZ")
end

# My class for the main window
class MyMainWindow < Qt::MainWindow
  attr_reader :ui, :server

  slots :on_buttonReload_clicked, :on_buttonSendData_clicked,
    :reloadMapMaybe, :sendDataMaybe, :on_actionResetMap_triggered

  def initialize
    super()

    # Initialize the UI
    @ui = Ui_MainWindow.new
    ui.setupUi(self)

    # Our server address
    # @server = "http://carcomm.cstimming.de"
    @server = "http://localhost:3000"

    # Read the HTML page and write it into the webview widget
    on_actionResetMap_triggered

    # Set the date to now
    ui.dateTimeEdit.setDateTime(Qt::DateTime::currentDateTime)

    # Fill the list of available time intervals
    ui.comboBoxInterval.addItem("1 Year", Qt::Variant.new(525600))
    ui.comboBoxInterval.addItem("5 Minutes", Qt::Variant.new(5))
    ui.comboBoxInterval.addItem("15 Minutes", Qt::Variant.new(15))
    ui.comboBoxInterval.addItem("2 Hours", Qt::Variant.new(120))
    ui.comboBoxInterval.addItem("1 Day", Qt::Variant.new(1440))
    ui.comboBoxInterval.addItem("1 Week", Qt::Variant.new(10080))
  end

  def sendDataMaybe
    if ui.checkBoxAutoSend.isChecked
      on_buttonSendData_clicked
    end
  end

  def reloadMapMaybe
    if ui.checkBoxAutoReload.isChecked
      on_buttonReload_clicked
    end
  end

  def on_buttonSendData_clicked
    puts "We are in on_buttonSendData_clicked"
    # ADD IMPLEMENTATION
  end

  def on_buttonReload_clicked
    lat = ui.lineLatitude.text
    lon = ui.lineLongitude.text
    ui.webView.page.mainFrame.evaluateJavaScript("setLatLon(#{lat},#{lon});")

    currentTime = ui.dateTimeEdit.dateTime
    interval = ui.comboBoxInterval.itemData(ui.comboBoxInterval.currentIndex).to_i
    min_time = qtDateTimeToString(currentTime.addSecs(-60 * interval))
    max_time = qtDateTimeToString(currentTime)
    puts "We are in on_buttonReload_clicked: loadWays(\"#{server}\", \"#{min_time}\", \"#{max_time}\");"
    ui.webView.page.mainFrame.evaluateJavaScript("loadWays(\"#{server}\", \"#{min_time}\", \"#{max_time}\");")
  end

  def on_actionResetMap_triggered
    # Read the HTML page and write it into the webview widget
    #filename = ':res/test.html'
    filename = ':res/streetmap.html'
    f = Qt::File.new(filename)
    f.open(Qt::IODevice::ReadOnly)
    ui.webView.setContent(f.readAll)
  end
end


# Create and show our main window
if $0 == __FILE__
  app = Qt::Application.new(ARGV)

  mainwindow = MyMainWindow.new

  mainwindow.show

  app.exec
end
