require 'slice'
require 'algo'

class Congest
  def self.callAlgo(instanceid, lasttime)
    # Build the conditions for the sub-set of slices
    conds = {}
    conds[:instanceid] = instanceid
    conds[:time] = (lasttime - 120.seconds)..lasttime

    # Query for the slices
    slices = Slice.all(:conditions => conds,
                       :order => "time ASC")

    if slices.empty?
      return 0
    end

    # Simplify the list of slices into arrays with time[s] and
    # velocity [km/h]
    now = slices.last.time
    tdata = slices.map {|slice|
      [ now - slice.time, 3.6 * slice.avgvel ]
    }

    # Pass the list of slices on to the algorithm, then return its return value
    Algo.predict1(tdata)
  end
end
