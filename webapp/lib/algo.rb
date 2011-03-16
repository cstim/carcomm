require 'time'

class Algo
  def self.predict1(tdata)
    if tdata.empty?
      return 0
    end

	# Current velocity in [km/h]
    v_current = tdata.last[1]
	# The current time - probably zero.
	t_current = tdata.last[0]

	# Find the last index where the block is true
	t_30_index = tdata.rindex{|i| t_current - i[0] >= 30}

	# This is the measurement approx. 30 seconds ago, and its velocity
	t_30 = tdata[t_30_index][0]
	v_30 = tdata[t_30_index][1]
	#puts "t_30_index=#{t_30_index}, v_30=#{v_30}"

	t_diff = t_current - t_30
	v_diff = v_current - v_30

	m = v_diff.to_f / t_diff.to_f
	puts "t_diff=#{t_diff} v_diff=#{v_diff} m=#{m}"

    if v_current >= 60
      if m < -0.9
		bis_stillstand = v_current / 1.1
		return bis_stillstand.round.to_s + " Sek bis Stillstand"
	  else
		return "0 Kein Stau"
	  end
    elsif v_current >= 30
      if m < -0.9
		bis_stillstand = v_current / 1.7
		return bis_stillstand.round.to_s + " Sek bis Stillstand"
	  else
		return "0 Kein Stau"
	  end
    else
      return "-1 Stau"
    end
  end
end

def print_tdata(tdata)
  tdata.each { |t| puts "#{t[0]} - #{t[1]} km/h" }
end

def test_main1
  now = Time.now

  tdata = [	[-30, 90], [-20, 12], [-10, 11], [0, 80] ]
  #print_tdata(tdata)

  puts "Test Result1 = " + Algo.predict1(tdata).to_s

  tdata = [	[-30, 120], [-20, 12], [-10, 11], [0, 61] ]
  puts "Test Result2 = " + Algo.predict1(tdata).to_s

  tdata = [	[-30, 70], [-20, 90], [-10, 11], [0, 35] ]
  puts "Test Result3 = " + Algo.predict1(tdata).to_s

  tdata = [	[-30, 46], [-20, 12], [-10, 11], [0, 25] ]
  puts "Test Result4 = " + Algo.predict1(tdata).to_s

end

test_main1
