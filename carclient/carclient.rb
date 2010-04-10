#!/usr/bin/ruby
# http://www.arachnoid.com/ruby/RubyGUIProject/index.html

require 'Qt'
require 'qtwebkit'
require 'mainwindow.ui'
require 'ressources.qrc'

# My class for the main window
class MyMainWindow < Qt::MainWindow
  attr_reader :ui
  attr_writer :ui

  slots :on_buttonReloadMap_clicked

  def on_buttonReloadMap_clicked
    puts "We are in on_buttonReloadMap_clicked"
    @ui.webView.page.mainFrame.evaluateJavaScript('zoomToMuenchen();')
  end
end


# Create and show our main window
if $0 == __FILE__
  app = Qt::Application.new(ARGV)

  mainwindow = MyMainWindow.new
  mainwindow.ui = Ui_MainWindow.new
  mainwindow.ui.setupUi(mainwindow)

  mainwindow.show

  # Read the HTML page and write it into the webview widget
  f = Qt::File.new(':res/streetmap.html')
  f.open(Qt::IODevice::ReadOnly)
  x = f.readAll
  mainwindow.ui.webView.setContent(x)

  app.exec
end
