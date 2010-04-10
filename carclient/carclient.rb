#!/usr/bin/ruby

require 'Qt'

app = Qt::Application.new(ARGV)

button=Qt::PushButton.new("Hello Ruby! &Close me.")
Qt::Object.connect button, SIGNAL('clicked()'), button, SLOT('close()')
#button.resize(100,30)
button.show

app.exec
