#!/usr/bin/ruby
# http://www.arachnoid.com/ruby/RubyGUIProject/index.html

require 'Qt'
require 'qtwebkit'
require 'mainwindow.ui'
require 'ressources.qrc'

# My class for the main window
class MyMainWindow < Qt::MainWindow
  attr_reader :ui

  slots :on_buttonReloadMap_clicked

  def initialize
	super()

	# Initialize the UI
	@ui = Ui_MainWindow.new
	ui.setupUi(self)

	# Read the HTML page and write it into the webview widget
	filename = ':res/test.html'
	#filename = ':res/streetmap.html'
	f = Qt::File.new(filename)
	f.open(Qt::IODevice::ReadOnly)
	ui.webView.setContent(f.readAll)
  end

  def on_buttonReloadMap_clicked
    puts "We are in on_buttonReloadMap_clicked"
    @ui.webView.page.mainFrame.evaluateJavaScript('zoomToMuenchen();')
  end
end


# Create and show our main window
if $0 == __FILE__
  app = Qt::Application.new(ARGV)

  mainwindow = MyMainWindow.new

  mainwindow.show

  app.exec
end
