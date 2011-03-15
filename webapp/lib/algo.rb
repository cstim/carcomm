require 'time'

class Algo
  def self.predict1(tdata)
    if tdata.empty?
      return 0
    end

    currentspeed = tdata.last[1]
    if currentspeed > 60
      2
    elsif currentspeed > 30
      1
    else
      0
    end
  end
end

def print_tdata(tdata)
  tdata.each { |t| puts "#{t[0]} - #{t[1]} km/h" }
end

def test_main1
  now = Time.now
  tdata = [
           [-60, 10],
           [-50, 10],
           [-40, 10],
           [-30, 10],
           [-20, 10],
           [-10, 10],
           [0, 10]
          ]
  print_tdata(tdata)
end

test_main1
