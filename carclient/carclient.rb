#!/usr/bin/ruby
# http://www.arachnoid.com/ruby/RubyGUIProject/index.html

require 'Qt'
require 'qtwebkit'
require 'carmainwindow.ui'
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
    # @server = "http://carcomm.cstimming.de/"
    @server = "http://localhost:3000/"

    # Set the date to now
    ui.dateTimeEdit.setDateTime(Qt::DateTime::currentDateTime)

    # Fill the list of available time intervals
    comboBox = ui.comboBoxInterval
    comboBox.addItem("1 Year", Qt::Variant.new(525600))
    comboBox.addItem("6 Weeks", Qt::Variant.new(60480))
    comboBox.addItem("1 Week", Qt::Variant.new(10080))
    comboBox.addItem("1 Day", Qt::Variant.new(1440))
    comboBox.addItem("2 Hours", Qt::Variant.new(120))
    comboBox.addItem("15 Minutes", Qt::Variant.new(15))
    comboBox.addItem("5 Minutes", Qt::Variant.new(5))

    # Read the HTML page and write it into the webview widget
    on_actionResetMap_triggered
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
    lat = ui.lineLatitude.text
    lon = ui.lineLongitude.text
    ui.webView.page.mainFrame.evaluateJavaScript("setLatLon(#{lat},#{lon});")
    reloadMapMaybe
  end

  def on_buttonReload_clicked
    isInited = ui.webView.page.mainFrame.evaluateJavaScript("initialized")
    if (isInited.to_i == 0)
      puts "HTML view not yet initialized - waiting 0.5 seconds."
      Qt::Timer::singleShot(500, self, SLOT(:on_buttonReload_clicked))
      return
    end

    currentTime = ui.dateTimeEdit.dateTime
    interval = ui.comboBoxInterval.itemData(ui.comboBoxInterval.currentIndex).to_i
    min_time = qtDateTimeToString(currentTime.addSecs(-60 * interval))
    max_time = qtDateTimeToString(currentTime)
    cmdString = "loadWays(\"#{min_time}\", \"#{max_time}\");"
    puts "We are in on_buttonReload_clicked: #{cmdString}"
    ui.webView.page.mainFrame.evaluateJavaScript(cmdString)
  end

  def on_actionResetMap_triggered
    ui.webView.settings.setAttribute(Qt::WebSettings::JavascriptEnabled, true)
    #ui.webView.settings.setAttribute(Qt::WebSettings::DeveloperExtrasEnabled, true)
    ui.webView.settings.setAttribute(Qt::WebSettings::JavascriptCanOpenWindows, false)
    #ui.webView.settings.setAttribute(Qt::WebSettings::JavascriptCanAccessClipboard, true)

    # Read the HTML page and write it into the webview widget
    #filename = ':res/test.html'
    #f = Qt::File.new(filename)
    #f.open(Qt::IODevice::ReadOnly)
    #ui.webView.setContent(f.readAll)
    url = Qt::Url.new(server + "streetmap.html")
    ui.webView.setUrl(url)
    on_buttonReload_clicked
  end
end


# Create and show our main window
if $0 == __FILE__
  app = Qt::Application.new(ARGV)

  mainwindow = MyMainWindow.new

  mainwindow.show

  app.exec
end
